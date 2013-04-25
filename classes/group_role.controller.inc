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
    //@todo Clean up permissions before deletion.

    parent::delete($ids, $transaction);
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

  /**
   * Import a group role.
   *
   * Imports a group role and its permissions.
   */
  public function import($export) {
    $vars = drupal_json_decode($export);

    if (is_array($vars)) {
      // Store the permissions in a temporary variable.
      $permissions =  $vars['permissions'];

      // Remove the permissions property before creating the group role.
      unset($vars['permissions']);

      if ($group_role = $this->create($vars)) {
        // Store the exported permissions on the group role.
        $group_role->grantPermissions($permissions);

        return $group_role;
      }
    }

    return FALSE;
  }
}
