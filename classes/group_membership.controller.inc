<?php
/**
 * @file
 * Defines the Entity API CRUD class for group memberships.
 */

/**
 * Controller for group membership entities.
 */
class GroupMembershipController extends EntityAPIController {

  /**
   * Create a group membership.
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
    // Provide defaults that are needed for the db.
    $values += array(
      'roles' => array(),
      'status' => 'active',
    );

    return parent::create($values);
  }
}
