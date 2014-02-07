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
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // Save the group membership so we are sure to have an id.
    $return = parent::save($entity, $transaction);

    // Save all roles to the db for new memberships.
    if ($return == SAVED_NEW) {
      $entity->grantRoles($entity->roles, FALSE);
    }

    // Act more carefully for updated memberships.
    else {
      // Load the original entity to detect changes.
      $original = entity_load_unchanged('group_membership', $entity->mid);

      // Remove revoked roles from the db.
      $revoke = array_diff($original->roles, $entity->roles);
      if (!empty($revoke)) {
        $this->revokeRoles($revoke);
      }

      // Add new roles to the db.
      $grant = array_diff($entity->roles, $original->roles);
      if (!empty($grant)) {
        $this->grantRoles($grant, FALSE);
      }
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
   * @return
   *   A new instance of the group entity type.
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
