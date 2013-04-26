<?php
/**
 * @file
 * Defines the Entity API class for group roles.
 */

/**
 * Main class for group role entities.
 */
class GroupRole extends Entity {

  /**
   * The internal id of the group role.
   *
   * @var int
   */
  public $rid;

  /**
   * The group type this role is attached to.
   *
   * @var string
   */
  public $type;

  /**
   * The machine-readable name of the group role.
   *
   * @var string
   */
  public $name;

  /**
   * The human-readable name of the group role.
   *
   * @var string
   */
  public $label;

  /**
   * The internal weight of the group role.
   *
   * @var int
   */
  public $weight;

  /**
   * The entity status of the group role.
   *
   * @var int
   */
  public $status;

  /**
   * The module that provided the group role.
   *
   * @var string
   */
  public $module;

  /**
   * Class constructor.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, 'group_role');
  }

  /**
   * Return whether a group role is locked.
   */
  public function isLocked() {
    return isset($this->status)
      && empty($this->is_new)
      && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }

  /**
   * Flag the parent group type as ENTITY_CUSTOM.
   *
   * If the group role belongs to a group type, the group type it belongs
   * to will be flagged as ENTITY_CUSTOM.
   *
   * @todo Conditional check for global roles.
   */
  public function flagTypeCustom() {
    // Saving without $entity->is_rebuild automatically sets ENTITY_CUSTOM.
    group_type_save(group_type_load($this->type));
  }

  /**
   * Retrieve all permissions for a group role.
   *
   * @return array
   *   An array of group permission names.
   */
  public function getPermissions() {
    $cache = &drupal_static('GroupRole::getPermissions', array());

    // Check if the permissions have been fetched before.
    if (isset($cache[$this->name])) {
      $permissions = $cache[$this->name];
    }

    // Otherwise, retrieve the permissions from the database.
    else {
      $query = db_select('group_permission', 'gp');
      $query->addField('gp', 'permission');
      $query->condition('gp.role', $this->name);

      $permissions = $query->execute()->fetchCol();
      $cache[$this->name] = $permissions;
    }

    return $permissions;
  }

  /**
   * Clear the permission cache for a group role.
   */
  public function clearPermissionCache() {
    $cache = &drupal_static('GroupRole::getPermissions', array());
    unset($cache[$this->name]);
  }

  /**
   * Grant permissions to a role.
   *
   * When calling this function through code, you need to check for yourself
   * whether you need to flag the group role as ENTITY_CUSTOM. To do so, call
   * group_role_save() on it.
   *
   * @param array $permissions
   *   A list of permission names to grant.
   */
  public function grantPermissions(array $permissions) {
    $new = array_diff($permissions, $this->getPermissions());

    $query = db_insert('group_permission');
    $query->fields(array('role', 'permission'));

    foreach ($new as $permission) {
      $query->values(array(
        'role' => $this->name,
        'permission' => $permission,
      ));
    }

    $query->execute();
    $this->clearPermissionCache();
  }

  /**
   * Revoke permissions from a role.
   *
   * When calling this function through code, you need to check for yourself
   * whether you need to flag the group role as ENTITY_CUSTOM. To do so, call
   * group_role_save() on it.
   *
   * @param array $permissions
   *   (Optional) A list of permission names to revoke. Will revoke all
   *   permissions from the group role if $permissions is left blank.
   */
  public function revokePermissions(array $permissions = array()) {
    $query = db_delete('group_permission');
    $query->condition('role', $this->name);

    if ($permissions) {
      $query->condition('permission', $permissions, 'IN');
    }
    $query->execute();

    $this->clearPermissionCache();
  }

  /**
   * Change permissions for a role.
   *
   * This function may be used to grant and revoke multiple permissions at once.
   * For example, when a form exposes checkboxes to configure permissions for a
   * role, the form submit handler may directly pass the submitted values for the
   * checkboxes form element to this function.
   *
   * If the permissions were imported along with a group role, that role will be
   * flagged as ENTITY_CUSTOM. If the role itself was imported along with a group
   * type, that group type will be flagged as ENTITY_CUSTOM as well.
   *
   * @param $permissions
   *   An associative array, where the key holds the permission name and the value
   *   determines whether to grant or revoke that permission. Any value that
   *   evaluates to TRUE will cause the permission to be granted. Any value that
   *   evaluates to FALSE will cause the permission to be revoked.
   *   @code
   *     array(
   *       'administer users' => 0,      // Revoke 'administer users'
   *       'administer group' => FALSE,  // Revoke 'administer group'
   *       'create subgroups' => 1,      // Grant 'create subgroups'
   *       'edit group' => TRUE,         // Grant 'edit group'
   *       'view group' => 'view group', // Grant 'view group'
   *     )
   *   @endcode
   *   Existing permissions are not changed, unless specified in $permissions.
   *
   * @see GroupRole::grantPermissions()
   * @see GroupRole::revokePermissions()
   */
  public function changePermissions(array $permissions) {
    // Grant new permissions for the role.
    $grant = array_filter($permissions);
    if (!empty($grant)) {
      $this->grantPermissions(array_keys($grant));
    }

    // Revoke permissions for the role.
    $revoke = array_diff_assoc($permissions, $grant);
    if (!empty($revoke)) {
      $this->revokePermissions(array_keys($revoke));
    }

    // By calling group_role_save(), we flag the role as ENTITY_CUSTOM.
    group_role_save($this);
  }
}
