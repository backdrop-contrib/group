<?php
/**
 * @file
 * Defines the Entity API class for group memberships.
 */

/**
 * Main class for group memberships.
 *
 * @todo Validation of added roles.
 */
class GroupMembership extends Entity {

  /**
   * Grant roles for a membership.
   *
   * @param array $roles
   *   A list of role names to grant.
   */
  public function grantRoles(array $roles) {
    $roles = array_merge($this->roles, $roles);
    $this->roles = array_values(array_unique($roles));

    $this->save();
  }

  /**
   * Revoke roles from a membership.
   *
   * @param array $roles
   *   (optional) A list of role names to revoke. Will revoke all roles
   *   from the group membership if $permissions is left blank.
   */
  public function revokeRoles(array $roles = array()) {
    $this->roles = !empty($roles)
      ? array_values(array_diff($this->roles, $roles))
      : array();

    $this->save();
  }

  /**
   * Change roles for a membership.
   *
   * This function may be used to grant and revoke multiple roles at once. For
   * example, when a form exposes checkboxes to configure roles for a
   * membership, the form submit handler may directly pass the submitted values
   * for the checkboxes form element to this function.
   *
   * @param $roles
   *   An associative array, where the key holds the role name and the value
   *   determines whether to grant or revoke that role. Any value that
   *   evaluates to TRUE will cause the role to be granted. Any value that
   *   evaluates to FALSE will cause the role to be revoked.
   *   @code
   *     array(
   *       'group_admin' => 0,         // Revoke 'group_admin'
   *       'group_admin' => FALSE,     // Revoke 'group_admin'
   *       'sub_admin' => 1,           // Grant 'sub_admin'
   *       'sub_admin' => TRUE,        // Grant 'sub_admin'
   *       'sub_admin' => 'sub_admin', // Grant 'sub_admin'
   *     )
   *   @endcode
   *   Existing roles are not changed, unless specified in $roles.
   *
   * @see GroupMembership::grantRoles()
   * @see GroupMembership::revokeRoles()
   */
  public function changeRoles(array $roles) {
    // Grant new roles for the membership.
    $grant = array_filter($roles);
    if (!empty($grant)) {
      $this->grantRoles(array_keys($grant));
    }

    // Revoke roles from the membership.
    $revoke = array_diff_assoc($roles, $grant);
    if (!empty($revoke)) {
      $this->revokeRoles(array_keys($revoke));
    }
  }

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

  /**
   * Get all pending membership invitations for a user.
   *
   * @param int $uid
   *   The uid of the user to check invites for.
   *
   * @return array
   *   An array of GroupMembership objects.
   */
  public static function getUserInvites($uid) {
    return group_memberships(array('uid' => $uid, 'status' => GROUP_MEMBERSHIP_INVITED));
  }
}
