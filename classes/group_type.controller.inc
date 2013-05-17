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
      $type->removeRoles();
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

    $return = parent::save($entity, $transaction);

    // If the roles property is set, we are dealing with a group type
    // that was imported in code and we need to take care of the roles.
    if (isset($entity->roles)) {
      // Delete attached group roles if it is a rebuild.
      if (!$is_new) {
        $entity->removeRoles();
      }

      // Create the group roles that were imported along with the type.
      if ($entity->roles) {
        foreach ($entity->roles as $role) {
          $group_role = group_role_import(entity_var_json_export($role));

          // Let GroupRoleController::save() know that it got called
          // while saving a group type.
          $group_role->type_is_saved = TRUE;

          group_role_save($group_role);
        }

        // Remove the roles property before saving the group type.
        unset($entity->roles);
      }
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
    // Provide defaults that are needed in group_type_form().
    $values += array(
      'name' => '',
      'label' => '',
    );

    return parent::create($values);
  }

  /**
   * Export a group type.
   *
   * Exports a group type and its group roles.
   */
  public function export($entity, $prefix = '') {
    // Get the GroupRole controller.
    $controller = entity_get_controller('group_role');

    // Add group roles to the group type.
    foreach ($entity->getRoles() as $role) {
      // Global group roles are never exported along with a group type.
      if ($role->global) {
        continue;
      }

      // @todo Remove this and figure out what to do with anonymous and member.
      if (in_array($role->name, array('anonymous', 'member'))) {
        continue;
      }

      $entity->roles[] = drupal_json_decode($controller->export($role, $prefix));
    }

    return parent::export($entity, $prefix);
  }
}
