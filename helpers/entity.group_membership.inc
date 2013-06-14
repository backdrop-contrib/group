<?php
/**
 * @file
 * Entity API related helper functions for group memberships.
 */

/**
 * Load multiple group memberships by id.
 */
function group_membership_load_multiple($mids = FALSE, $reset = FALSE) {
  return entity_load('group_membership', $mids, array(), $reset);
}

/**
 * Load all group memberships, optionally filtered by property values.
 *
 * @param array $properties
 *   (optional) An associative array with property names for keys and the
 *   values you wish to filter on as accompaning array values.
 *
 * @return array
 *   An array of GroupMembership entities, keyed by their mid.
 */
function group_memberships($filters = array()) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'group_membership');

  foreach ($filters as $property => $value) {
    $query->propertyCondition($property, $value);
  }
  $result = $query->execute();

  return isset($result['group_membership'])
    ? group_membership_load_multiple(array_keys($result['group_membership']))
    : array();
}

/**
 * Load a group membership by group and user id.
 */
function group_membership_load($gid, $uid) {
  $group_memberships = group_memberships(array('gid' => $gid, 'uid' => $uid));
  return reset($group_memberships);
}

/**
 * Load all group memberships for a given group.
 */
function group_membership_load_by_group($gid) {
  return group_memberships(array('gid' => $gid));
}

/**
 * Load all group memberships for a given user.
 */
function group_membership_load_by_user($uid) {
  return group_memberships(array('uid' => $uid));
}

/**
 * Save a group.
 */
function group_membership_save($group_membership) {
  entity_save('group_membership', $group_membership);
}

/**
 * Delete a group.
 */
function group_membership_delete($group_membership) {
  entity_delete('group_membership', entity_id('group_membership', $group_membership));
}

/**
 * Delete multiple groups.
 */
function group_membership_delete_multiple($mids) {
  entity_delete_multiple('group_membership', $mids);
}
