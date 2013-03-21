<?php
/**
 * @file
 * Entity API related helper functions.
 */

/**
 * Load a group.
 */
function group_load($gid, $reset = FALSE) {
  $groups = group_load_multiple(array($gid), array(), $reset);
  return reset($groups);
}

/**
 * Load multiple groups based on certain conditions.
 */
function group_load_multiple($gids = array(), $conditions = array(), $reset = FALSE) {
  return entity_load('group', $gids, $conditions, $reset);
}

/**
 * Save a group.
 */
function group_save($group) {
  entity_save('group', $group);
}

/**
 * Delete a group.
 */
function group_delete($group) {
  entity_delete('group', entity_id('group', $group));
}

/**
 * Delete multiple groups.
 */
function group_delete_multiple($gids) {
  entity_delete_multiple('group', $gids);
}

/**
 * Gets an array of all group types, keyed by the name.
 *
 * @param $name
 *   If set, the type with the given name is returned.
 */
function group_types($name = NULL) {
  $types = entity_load_multiple_by_name('group_type', isset($name) ? array($name) : FALSE);
  return isset($name) ? reset($types) : $types;
}

/**
 * Load a group type.
 */
function group_type_load($type) {
  return group_types($type);
}

/**
 * Save a group type.
 */
function group_type_save($type) {
  entity_save('group_type', $type);
}

/**
 * Delete a group type.
 */
function group_type_delete($type) {
  entity_delete('group_type', entity_id('group_type', $type));
}

/**
 * Delete multiple group types.
 */
function group_type_delete_multiple($tids) {
  entity_delete_multiple('group_type', $tids);
}
