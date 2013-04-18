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
 * Load multiple groups.
 */
function group_load_multiple($gids = FALSE, $reset = FALSE) {
  return entity_load('group', $gids, array(), $reset);
}

/**
 * Load all groups of a given type.
 *
 * @param string $type
 *   The machine name of the GroupType you wish to filter on.
 *
 * @return array
 *   An array of Group entities, keyed by their group ids.
 */
function group_load_by_type($type) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'group');
  $query->entityCondition('bundle', $type);
  $result = $query->execute();

  return isset($result['group'])
    ? group_load_multiple(array_keys($result['group']))
    : array();
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
