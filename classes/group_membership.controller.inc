<?php
/**
 * @file
 * Defines the Entity API CRUD class for group memberships.
 */

/**
 * Controller for group membership entities.
 */
class GroupMembershipController extends EntityAPIController {

  /**
   * Load a group membership.
   *
   * Adds group roles to the GroupMembership instance.
   */
  public function load($ids = array(), $conditions = array()) {
    $return = parent::load($ids, $conditions);

    if ($return) {
      // Gather all loaded memberships's role data.
      $query = db_select('group_membership_role', 'gmr')
        ->fields('gmr', array('mid', 'role'))
        ->condition('mid', array_keys($return), 'IN');

      // Gather the roles per group membership id.
      foreach ($query->execute() as $record) {
        $roles[$record->mid][] = $record->role;
      }

      // Add the group roles onto the memberships.
      foreach ($return as $mid => $group_membership) {
        $group_membership->roles = isset($roles[$mid]) ? $roles[$mid] : array();
      }
    }

    return $return;
  }

  /**
   * Delete a group membership.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    if (!empty($ids)) {
      // Delete all associated group membership roles.
      db_delete('group_membership_role')
        ->condition('mid', $ids, 'IN')
        ->execute();
    }

    parent::delete($ids, $transaction);
  }

  /**
   * Save a group membership.
   *
   * @todo Validation of added roles.
   */
  public function save($group_membership, DatabaseTransaction $transaction = NULL) {
    // First of all we clean up the 'roles' property to avoid duplicates.
    $group_membership->roles = array_unique($group_membership->roles);

    // Save the group membership so we are sure to have an id.
    $return = parent::save($group_membership, $transaction);

    // Below we take care of the 'roles' property that was set on the
    // GroupMembership; saving new roles to and removing revoked roles from the
    // database. For new GroupMembership entities, we can simply grant all of
    // the roles. For existing ones, we need to run some integrity checks.
    if ($return == SAVED_NEW) {
      $grant = $group_membership->roles;
    }
    else {
      // Load the original entity to detect changes.
      $original = entity_load_unchanged('group_membership', $group_membership->mid);

      // Flag new roles for granting.
      $grant = array_diff($group_membership->roles, $original->roles);

      // Flag old roles for revoking.
      $revoke = array_diff($original->roles, $group_membership->roles);
    }

    // Grant the roles by inserting them into {group_membership_role}.
    if (!empty($grant)) {
      $query = db_insert('group_membership_role')->fields(array('mid', 'role'));

      foreach ($grant as $role) {
        $query->values(array(
          'mid' => $group_membership->mid,
          'role' => $role,
        ));
      }

      $query->execute();
    }

    // Revoke the roles by deleting them from {group_membership_role}.
    if (!empty($revoke)) {
      $query = db_delete('group_membership_role')->condition('mid', $group_membership->mid);
      $query->condition('role', $revoke, 'IN');
      $query->execute();
    }

    return $return;
  }

  /**
   * Create a group membership.
   *
   * We first set up the values that are specific to the group schema
   * but then also run the EntityAPIController counterpart.
   *
   * @param $values
   *   An array of values to set, keyed by property name.
   *
   * @return GroupMembership
   *   A new GroupMembership instance.
   */
  public function create(array $values = array()) {
    // Provide defaults that are needed for the db.
    $values += array(
      'roles' => array(),
      'status' => 'active',
    );

    return parent::create($values);
  }
}
