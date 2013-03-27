<?php
/**
 * @file
 * Defines the Entity API CRUD class for groups.
 */

/**
 * Controller for group entities.
 */
class GroupController extends EntityAPIController {

  /**
   * Class constructor.
   */
  public function __construct($entityType) {
    parent::__construct($entityType);
  }

  /**
   * Creates a group.
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
    // Add values that are specific to a group.
    $values += array(
      'type' => '',
      'title' => '',
    );

    return parent::create($values);
  }
}