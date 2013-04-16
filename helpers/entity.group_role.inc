<?php
/**
 * @file
 * Entity API related helper functions for group roles.
 */

/**
 * Load group roles.
 *
 * @param array $names
 *   (optional) An array of machine names to filter on.
 *
 * @return GroupRole|array
 *   Returns a single GroupRole object when $names contains only one name.
 *   When an array of multiple names or FALSE is provided, the return value
 *   becomes an array of GroupRole objects, keyed by their corresponding
 *   machine names.
 *
 * @see entity_load_multiple_by_name()
 */
function group_roles($names = FALSE) {
  $roles = entity_load_multiple_by_name('group_role', $names);
  return (is_array($names) && count($names) == 1) ? reset($roles) : $roles;
}

/**
 * Load a group role by name.
 */
function group_role_load($name) {
  return group_roles(array($name));
}

/**
 * Load multiple group roles by id.
 */
function group_role_load_multiple($rids = FALSE, $reset = FALSE) {
  return entity_load('group_role', $rids, array(), $reset);
}

/**
 * Save a group role.
 */
function group_role_save($role) {
  entity_save('group_role', $role);
}

/**
 * Delete a group role.
 */
function group_role_delete($role) {
  entity_delete('group_role', entity_id('group_role', $role));
}

/**
 * Delete multiple group roles.
 */
function group_role_delete_multiple($rids) {
  entity_delete_multiple('group_role', $rids);
}
