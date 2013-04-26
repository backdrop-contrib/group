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
      // we need to flag the parent group type as ENTITY_CUSTOM.
      if ($role->status == ENTITY_CUSTOM) {
        $role->typeIsCustom();
      }
    }

    parent::delete($ids, $transaction);
  }

  /**
   * Save a group role.
   *
   * @see EntityAPIController::save()
   *
   * @todo Check for is_rebuild for global roles.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $is_new = !empty($entity->is_new) || empty($entity->{$this->idKey});
    $is_type_save = !empty($entity->type_is_saved);

    // Delete all attached permissions if the group type is being saved.
    if ($is_type_save && !$is_new) {
      $entity->revokePermissions();
    }

    // Create (or recreate) permissions if the group role is new or the
    // group type the roles belong to is being saved.
    if (($is_new || $is_type_save) && isset($entity->permissions)) {
      // Store the exported permissions on the group role.
      $entity->grantPermissions($entity->permissions);

      // Remove the permissions property before creating the group role.
      unset($entity->permissions);
    }

    // If we manually save a group role we flag the group type the role
    // belongs to as ENTITY_CUSTOM.
    if (!$is_type_save) {
      $entity->typeIsCustom();

      // @todo Is this necessary here?
      // Rebuild the menu cache so the group/add page works.
      menu_rebuild();
    }

    // Remove the type_is_saved property before creating the group role.
    else {
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
      'weight' => 0,
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
