<?php
/**
 * @file
 * Group related helper functions.
 */

/**
 * Determine whether the user has a given privilege for a group.
 *
 * Important: Group access should always trump user access. If a user has the
 * right to view nodes of type Page, he can still face an 'Access denied'
 * message if he tries to view a Page of a private group.
 *
 * @param string $permission
 *   The permission, such as "administer group", being checked for.
 * @param Group $group
 *   The group to check the permission on.
 * @param object $account
 *   (optional) The account to check, if not given use currently logged in user.
 *
 * @return bool
 *   Wether the user has the requested permission.
 */
function group_access($permission, $group, $account = NULL) {
  global $user;

  if (!isset($account)) {
    $account = $user;
  }

  return user_access('bypass group access', $account)
    || $group->userHasPermission($account->uid, $permission);
}

/**
 * Get the groups a group entity belongs to.
 *
 * @param string $entity_type
 *   The entity type of the group entity.
 * @param int $entity_id
 *   The entity id of the group entity.
 *
 * @return array
 *   A list of all groups that the entity belongs to.
 */
function group_get_entity_groups($entity_type, $entity_id) {
  $query = db_select('group_entity', 'ge');
  $query->condition('ge.entity_type', $entity_type);
  $query->condition('ge.entity_id', $entity_id);
  $query->addfield('ge', 'gid');

  return group_load_multiple($query->execute()->fetchCol());
}

/**
 * Retrieves the group id from an autocomplete result.
 *
 * @param string $result
 *   The result returned by a group autocomplete field.
 *
 * @return int|false
 *   The group id (gid) of the result or FALSE on failure.
 *
 * @see group_autocomplete()
 * @see group_autocomplete_by_type()
 */
function group_autocomplete_result_gid($result) {
  if (preg_match('/(\d+)\)$/', $result, $matches)) {
    return $matches[1];
  }

  return FALSE;
}

/**
 * Check whether a group role machine name exists.
 */
function group_role_exists($name) {
  return in_array($name, array('outsider', 'member')) || !!group_role_load($name);
}
