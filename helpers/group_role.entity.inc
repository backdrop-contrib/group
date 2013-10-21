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
 * @return array
 *   An array of GroupRole objects, keyed by their machine names.
 *
 * @see entity_load_multiple_by_name()
 */
function group_roles($names = FALSE) {
  return entity_load_multiple_by_name('group_role', $names);
}

/**
 * Load a group role by name.
 */
function group_role_load($name) {
  $group_roles = group_roles(array($name));
  return reset($group_roles);
}

/**
 * Load multiple group roles by id.
 */
function group_role_load_multiple($rids = FALSE, $reset = FALSE) {
  return entity_load('group_role', $rids, array(), $reset);
}

/**
 * Import a group role.
 */
function group_role_import($export) {
  return entity_import('group_role', $export);
}

/**
 * Save a group role.
 */
function group_role_save($group_role) {
  return entity_save('group_role', $group_role);
}

/**
 * Delete a group role.
 */
function group_role_delete($group_role) {
  entity_delete('group_role', entity_id('group_role', $group_role));
}

/**
 * Delete multiple group roles.
 */
function group_role_delete_multiple($rids) {
  entity_delete_multiple('group_role', $rids);
}
