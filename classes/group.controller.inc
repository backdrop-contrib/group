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
   * Delete a group.
   *
   * @see EntityAPIController::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    //@todo Potentially clean up before deletion.

    parent::delete($ids, $transaction);
  }

  /**
   * Create a group.
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
    // Provide defaults that are needed in group_form().
    $values += array(
      'type' => '',
      'title' => '',
    );

    return parent::create($values);
  }
}
