<?php
/**
 * @file
 * Entity API related helper functions for groups.
 */

/**
 * Load multiple groups.
 */
function group_load_multiple($gids = FALSE, $reset = FALSE) {
  return entity_load('group', $gids, array(), $reset);
}

/**
 * Load a group.
 */
function group_load($gid, $reset = FALSE) {
  $groups = group_load_multiple(array($gid), array(), $reset);
  return reset($groups);
}

/**
 * Load all groups with a given property value.
 *
 * @param string $property
 *   The property name you wish to filter on.
 * @param string $value
 *   The property value you wish to filter on.
 *
 * @return array
 *   An array of Group entities, keyed by their group ids.
 */
function group_load_by_property($property, $value) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'group');
  $query->propertyCondition($property, $value);
  $result = $query->execute();

  return isset($result['group'])
    ? group_load_multiple(array_keys($result['group']))
    : array();
}

/**
 * Load all groups with a given name.
 */
function group_load_by_name($name) {
  return group_load_by_property('title', $name);
}

/**
 * Load all groups of a given type.
 */
function group_load_by_type($type) {
  return group_load_by_property('type', $type);
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
