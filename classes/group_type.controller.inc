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
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $is_new = !empty($entity->is_new) || empty($entity->{$this->idKey});
    $is_rebuild = !empty($entity->is_rebuild);

    // Delete all attached group roles if it is a rebuild.
    if (!$is_new && $is_rebuild) {
      foreach($entity->getRoles() as $role) {
        group_role_delete($role);
      }
    }

    // Create (or recreate) group roles if the group type is new or rebuilt.
    if (($is_new || $is_rebuild) && isset($entity->roles)) {
      // Get the GroupRole controller.
      $controller = entity_get_controller('group_role');

      // Import the group roles for this group type.
      // @todo group_role_import() and group_role_save().
      foreach ($entity->roles as $role) {
        $controller->save($controller->import(entity_var_json_export($role)));
      }

      // Remove the roles property before saving the group type.
      unset($entity->roles);
    }

    $return = parent::save($entity, $transaction);

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
