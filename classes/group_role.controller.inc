<?php
/**
 * @file
 * Defines the Entity API CRUD class for group roles.
 */

/**
 * Controller for group role entities.
 */
class GroupRoleController extends EntityAPIControllerExportable {

  /**
   * Class constructor.
   */
  public function __construct($entityType) {
    parent::__construct($entityType);
  }

  /**
   * Delete a group role.
   *
   * @see EntityAPIController::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    // Clean up permissions before deletion.
    foreach (group_roles($ids) as $role) {
      $role->revokePermissions();

      // If we delete a role that was not flagged as ENTITY_CUSTOM,
      // we need to call GroupRole::flagTypeCustom().
      if ($role->status == ENTITY_CUSTOM) {
        $role->flagTypeCustom();
      }
    }

    parent::delete($ids, $transaction);
  }

  /**
   * Save a group role.
   *
   * @see EntityAPIController::save()
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $is_new = !empty($entity->is_new) || empty($entity->{$this->idKey});

    // If the permissions property is set, we are dealing with a group role
    // that was imported in code and we need to take care of the permissions.
    if (isset($entity->permissions)) {
      // Delete all attached permissions if the group type is being rebuilt.
      if (!$is_new) {
        $entity->revokePermissions();
      }

      // Create the permissions that were imported along with the role.
      if ($entity->permissions) {
        $entity->grantPermissions($entity->permissions);
      }

      // Remove the permissions property before creating the group role.
      unset($entity->permissions);
    }

    // If we manually save a group role we call GroupRole::flagTypeCustom().
    if (empty($entity->type_is_saved)) {
      $entity->flagTypeCustom();
    }
    else {
      // Remove the type_is_saved property before creating the group role.
      unset($entity->type_is_saved);
    }

    return parent::save($entity, $transaction);
  }

  /**
   * Create a group role.
   *
   * We first set up the values that are specific to the group role schema
   * but then also run the EntityAPIControllerExportable counterpart.
   *
   * @param $values
   *   An array of values to set, keyed by property name.
   *
   * @return
   *   A new instance of the group role entity type.
   */
  public function create(array $values = array()) {
    // Add values that are specific to a group.
    $values += array(
      'type' => '',
      'name' => '',
      'label' => '',
      'global' => 1,
      'weight' => 0,
      'status' => ENTITY_CUSTOM,
    );

    return parent::create($values);
  }

  /**
   * Export a group role.
   *
   * Exports a group role and its permissions.
   */
  public function export($entity, $prefix = '') {
    $entity->permissions = $entity->getPermissions();

    return parent::export($entity, $prefix);
  }
}
