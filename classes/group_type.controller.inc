<?php
/**
 * @file
 * Defines the Entity API CRUD class for group types.
 */

/**
 * Controller for group type entities.
 */
class GroupTypeController extends EntityAPIControllerExportable {

  /**
   * Class constructor.
   */
  public function __construct($entityType) {
    parent::__construct($entityType);
  }

  /**
   * Delete a group type.
   *
   * @see EntityAPIController::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    // Clean up local roles before deletion.
    foreach (group_types($ids) as $type) {
      foreach($type->getRoles() as $role) {
        group_role_delete($role);
      }
    }

    parent::delete($ids, $transaction);

    // Rebuild the menu cache so the group/add page works.
    menu_rebuild();
  }

  /**
   * Save a group type.
   *
   * @see EntityAPIController::save()
   *
   * @todo Keep overridden roles overridden?
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $is_new = !empty($entity->is_new) || empty($entity->{$this->idKey});
    $is_rebuild = !empty($entity->is_rebuild);

    $return = parent::save($entity, $transaction);

    // Delete attached group roles if it is a rebuild.
    if (!$is_new && $is_rebuild) {
      foreach ($entity->getRoles() as $role) {
        // We only delete those group roles that were not originally provided
        // by the imported group type. Overridden group roles will be resaved
        // to their default state further below.
        if ($role->status == ENTITY_CUSTOM) {
          group_role_delete($role);
        }
      }
    }

    // Create (or recreate) group roles if the group type is new or rebuilt.
    if (($is_new || $is_rebuild) && isset($entity->roles)) {
      // Get the existing group roles.
      $existing_roles = group_roles();

      // Get the GroupRole controller.
      $controller = entity_get_controller('group_role');

      // Import the group roles for this group type. The type_is_saved flag
      // lets GroupRoleController::save() know that it got called during the
      // saving of a group type.
      foreach ($entity->roles as $role) {
        // @todo group_role_import() and group_role_save().
        $group_role = $controller->import(entity_var_json_export($role));

        $group_role->is_rebuild = TRUE;
        $group_role->type_is_saved = TRUE;
        $group_role->status |= ENTITY_IN_CODE;

        // Update a role if it already existed.
        if (!empty($existing_roles[$group_role->name])) {
          $group_role->rid = $existing_roles[$group_role->name]->rid;
          unset($group_role->is_new);
        }

        $controller->save($group_role);
      }

      // Remove the roles property before saving the group type.
      unset($entity->roles);
    }

    // Rebuild the menu cache so the group/add page works.
    menu_rebuild();

    return $return;
  }

  /**
   * Create a group type.
   *
   * We first set up the values that are specific to the group type schema
   * but then also run the EntityAPIControllerExportable counterpart.
   *
   * @param $values
   *   An array of values to set, keyed by property name.
   *
   * @return
   *   A new instance of the group type entity type.
   */
  public function create(array $values = array()) {
    // Add values that are specific to a group.
    $values += array(
      'name' => '',
      'label' => '',
      'weight' => 0,
    );

    return parent::create($values);
  }

  /**
   * Export a group type.
   *
   * Exports a group type and its group roles.
   *
   * @todo Only export local roles.
   */
  public function export($entity, $prefix = '') {
    // Get the GroupRole controller.
    $controller = entity_get_controller('group_role');

    // Add group roles to the group type.
    foreach ($entity->getRoles() as $role) {
      $entity->roles[] = drupal_json_decode($controller->export($role, $prefix));
    }

    return parent::export($entity, $prefix);
  }
}
