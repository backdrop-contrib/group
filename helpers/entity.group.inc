<?php
/**
 * @file
 * Entity API related helper functions for groups.
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
