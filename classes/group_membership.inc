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
   */
  public function getRoles() {
    return group_roles($this->roles);
  }

  /**
   * Get all permissions for a group membership.
   *
   * @return array
   *   An array of group permission names.
   */
  public function getPermissions() {
    // Get the member permissions for the related group type.
    $wrapper = entity_metadata_wrapper('group_membership', $this);
    $permissions = $wrapper->group->group_type->member_permissions->value();

    // Add the permissions that belong to the attached roles.
    foreach ($this->getRoles() as $group_role) {
      $permissions = array_merge($permissions, $group_role->permissions);
    }

    return array_values(array_unique($permissions));
  }
}
