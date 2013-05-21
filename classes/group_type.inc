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
   * Class constructor.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, 'group_type');
  }

  /**
   * Returns whether a group type is locked.
   */
  public function isLocked() {
    return isset($this->status)
      && empty($this->is_new)
      && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }

  /**
   * Retrieve all possible roles for a group type.
   *
   * @return array
   *   A list of GroupRole entities.
   *
   * @see group_roles()
   */
  public function getRoles() {
    $roles = array();

    // @todo Add the anonymous and member role as a GroupRole.

    // Add the local roles for this group type.
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'group_role');
    $query->propertyCondition('type', $this->name);
    $query->propertyOrderBy('weight');
    $result = $query->execute();

    if (isset($result['group_role'])) {
      $roles += group_roles(array_keys($result['group_role']));
    }

    // Add global roles.
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'group_role');
    $query->propertyCondition('global', 1);
    $query->propertyOrderBy('weight');
    $result = $query->execute();

    if (isset($result['group_role'])) {
      $roles += group_roles(array_keys($result['group_role']));
    }

    return $roles;
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

    foreach ($this->getRoles() as $name => $role) {
      // Skip anonymous, member and global roles.
      if (in_array($name, array('anonymous', 'member')) || $role->global) {
        continue;
      }

      $roles[] = $name;
    }

    group_role_delete_multiple($roles);
  }
}
