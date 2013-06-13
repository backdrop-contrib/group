<?php
/**
 * @file
 * Defines the Entity API class for group memberships.
 */

/**
 * Main class for group memberships.
 */
class GroupMembership extends Entity {

  /**
   * Get all roles for a group membership.
   *
   * @return array
   *   An array of GroupRole entities.
   *
   * @todo Anonymous and Registered.
   */
  public function getRoles() {
    return group_roles($this->roles);
  }
}
