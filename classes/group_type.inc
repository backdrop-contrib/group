<?php
/**
 * @file
 * Defines the Entity API class for group types.
 */

/**
 * Main class for group type entities.
 */
class GroupType extends Entity {

  /**
   * Retrieve all possible roles for a group type.
   *
   * @return array
   *   A list of GroupRole entities.
   *
   * @see group_roles()
   */
  public function getRoles() {
    $group_roles = array();

    // @todo Add the anonymous and member role as a GroupRole.

    // Add the local roles for this group type.
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'group_role');
    $query->propertyCondition('type', $this->name);
    $query->propertyOrderBy('weight');
    $result = $query->execute();

    if (isset($result['group_role'])) {
      $group_roles += group_roles(array_keys($result['group_role']));
    }

    // Add global roles.
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'group_role');
    $query->propertyCondition('global', 1);
    $query->propertyOrderBy('weight');
    $result = $query->execute();

    if (isset($result['group_role'])) {
      $group_roles += group_roles(array_keys($result['group_role']));
    }

    return $group_roles;
  }

  /**
   * Delete all possible roles for a group type.
   *
   * Does not delete global roles, nor the special roles anonymous and member.
   *
   * @see group_roles()
   */
  public function removeRoles() {
    $roles = array();

    foreach ($this->getRoles() as $name => $group_role) {
      // Skip anonymous, member and global roles.
      if (in_array($name, array('anonymous', 'member')) || $group_role->global) {
        continue;
      }

      $roles[] = $name;
    }

    group_role_delete_multiple($roles);
  }
}
