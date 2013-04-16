<?php
/**
 * @file
 * Entity API related helper functions for group types.
 */

/**
 * Load group types.
 *
 * @param array $names
 *   (optional) An array of machine names to filter on.
 *
 * @return GroupType|array
 *   Returns a single GroupType object when $names contains only one name.
 *   When an array of multiple names or FALSE is provided, the return value
 *   becomes an array of GroupType objects, keyed by their corresponding
 *   machine names.
 *
 * @see entity_load_multiple_by_name()
 */
function group_types($names = FALSE) {
  $types = entity_load_multiple_by_name('group_type', $names);
  return (is_array($names) && count($names) == 1) ? reset($types) : $types;
}

/**
 * Load a group type by name.
 */
function group_type_load($name) {
  return group_types(array($name));
}

/**
 * Load multiple group types by id.
 */
function group_type_load_multiple($tids = FALSE, $reset = FALSE) {
  return entity_load('group_type', $tids, array(), $reset);
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
