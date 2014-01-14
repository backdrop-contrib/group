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
   * @param bool $safety
   *   (optional) Whether to check for primary key integrity. Defaults to TRUE.
   *   Set this to FALSE if you know for sure that all of the roles you are
   *   granting are not already related to the membership. It gives a small
   *   performance boost.
   */
  public function grantRoles(array $roles, $safety = TRUE) {
    // Only save roles to the db if the membership exists in it. If it doesn't
    // exist yet, but gets saved later on, grantRoles() will be invoked again
    // from GroupMembershipController::save().
    if (isset($this->mid)) {
      if ($safety) {
        // Load the original entity to only retain new roles.
        $original = entity_load_unchanged('group_membership', $this->mid);
        $roles = array_diff($roles, $original->roles);
      }

      // Add the roles to {group_membership_role} for the membership.
      if (!empty($roles)) {
        $query = db_insert('group_membership_role')->fields(array('mid', 'role'));

        foreach ($roles as $role) {
          $query->values(array(
            'mid' => $this->mid,
            'role' => $role,
          ));
        }

        $query->execute();
      }
    }

    // Update the membership's roles property.
    $roles = array_merge($this->roles, $roles);
    $this->roles = array_values(array_unique($roles));
  }

  /**
   * Revoke roles from a membership.
   *
   * @param array $roles
   *   (optional) A list of role names to revoke. Will revoke all roles
   *   from the group membership if $permissions is left blank.
   */
  public function revokeRoles(array $roles = array()) {
    $query = db_delete('group_membership_role')->condition('mid', $this->mid);

    if (!empty($roles)) {
      $query->condition('role', $roles, 'IN');
    }
    $query->execute();

    // Update the membership's roles property.
    $this->roles = !empty($roles)
      ? array_values(array_diff($this->roles, $roles))
      : array();
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
   * Get all active memberships for a user.
   *
   * @param int $uid
   *   The uid of the user to retrieve memberships for.
   * @param bool $active
   *   (optional) Whether to return active or inactive memberships. Defaults
   *   to TRUE.
   *
   * @return array
   *   An array of GroupMembership objects.
   */
  public static function getByActiveStatus($uid, $active = TRUE) {
    // Gather all statuses that match $active.
    foreach (group_membership_status_info() as $status => $info) {
      if ($info['active'] === $active) {
        $statuses[] = $status;
      }
    }

    // Only query the database if there are statuses to search for.
    if (!empty($statuses)) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'group_membership');
      $query->propertyCondition('uid', $uid);
      $query->propertyCondition('status', $statuses, 'IN');
      $result = $query->execute();

      if (isset($result['group_membership'])) {
        return group_membership_load_multiple(array_keys($result['group_membership']));
      }
    }

    return array();
  }

  /**
   * Get all memberships by status for a user.
   *
   * @param int $uid
   *   The uid of the user to retrieve memberships for.
   * @param string $status
   *   The machine name of the membership status.
   *
   * @return array
   *   An array of GroupMembership objects.
   */
  public static function getByStatus($uid, $status) {
    return group_memberships(array('uid' => $uid, 'status' => $status));
  }
}
