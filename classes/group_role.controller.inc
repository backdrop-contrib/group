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
    //@todo Clean up before deletion.

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
}
