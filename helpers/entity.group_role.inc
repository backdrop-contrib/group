<?php
/**
 * @file
 * Entity API related helper functions for group roles.
 */

/**
 * Gets an array of all group roles, keyed by the name.
 *
 * @param $name
 *   If set, the type with the given name is returned.
 */
function group_roles($name = NULL) {
  $types = entity_load_multiple_by_name('group_role', isset($name) ? array($name) : FALSE);
  return isset($name) ? reset($types) : $types;
}

/**
 * Load a group role.
 */
function group_role_load($type) {
  return group_roles($type);
}

/**
 * Save a group role.
 */
function group_role_save($type) {
  entity_save('group_role', $type);
}

/**
 * Delete a group role.
 */
function group_role_delete($type) {
  entity_delete('group_role', entity_id('group_role', $type));
}

/**
 * Delete multiple group roles.
 */
function group_role_delete_multiple($rids) {
  entity_delete_multiple('group_role', $rids);
}
