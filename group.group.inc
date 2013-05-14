<?php
/**
 * @file
 * Default global group roles, provided by the Group module itself.
 */

/**
 * Implements hook_default_group_role().
 */
function group_default_group_role() {
  $roles = array();

  $group_admin = entity_create('group_role', array(
    'name' => 'group_admin',
    'label' => 'Administrator',
    'weight' => 100,
    'permissions' => array_keys(module_invoke_all('group_permission')),
    'status' => ENTITY_FIXED,
  ));
  $roles['group_admin'] = group_role_import(entity_var_json_export($group_admin));

  return $roles;
}
