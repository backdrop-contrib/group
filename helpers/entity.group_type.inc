<?php
/**
 * @file
 * Entity API related helper functions for group types.
 */

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
