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
    //@todo Clean up before deletion.

    parent::delete($ids, $transaction);
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
}
