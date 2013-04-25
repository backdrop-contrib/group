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
    $is_rebuild = !empty($entity->is_rebuild);

    // Delete all attached permissions if it is a rebuild.
    if (!$is_new && $is_rebuild) {
      $entity->revokePermissions();
    }

    // Create (or recreate) permissions if the group type is new or rebuilt.
    if ($is_new && isset($entity->permissions)) {
      // Store the exported permissions on the group role.
      $entity->grantPermissions($entity->permissions);

      // Remove the permissions property before creating the group role.
      unset($entity->permissions);
    }

    // Add the ENTITY_CUSTOM flag the group type the role belongs to.
    $group_type = group_type_load($entity->type);
    $group_type->status |= ENTITY_CUSTOM;
    group_type_save($group_type);

    $return = parent::save($entity, $transaction);

    // Rebuild the menu cache so the group/add page works.
    menu_rebuild();

    return $return;
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
