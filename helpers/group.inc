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
 * Retrieve group ids for a list of group entities.
 *
 * @param string $entity_type
 *   The entity type of the group entities.
 * @param array $entity_ids
 *   The entity ids of the group entities.
 *
 * @return array
 *   A list of group ids that the entities belong to, keyed by entity id.
 */
function group_get_entities_group_ids($entity_type, $entity_ids) {
  $query = db_select('group_entity', 'ge');
  $query->fields('ge', array('entity_id', 'gid'));
  $query->condition('ge.entity_type', $entity_type);
  $query->condition('ge.entity_id', $entity_ids, 'IN');

  $gids = array();
  foreach ($query->execute() as $record) {
    $gids[$record->entity_id][] = $record->gid;
  }

  return $gids;
}

/**
 * Retrieve group ids for a single group entity.
 *
 * @param string $entity_type
 *   The entity type of the group entity.
 * @param int $entity_id
 *   The entity id of the group entity.
 *
 * @return array
 *   A list of group ids that the entity belongs to.
 */
function group_get_entity_group_ids($entity_type, $entity_id) {
  $gids = group_get_entities_group_ids($entity_type, array($entity_id));
  return !empty($gids[$entity_id]) ? $gids[$entity_id] : array();
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
 * Get the ids of the groups a user is an outsider to.
 *
 * @param int $uid
 *   The id of the user to retrieve group ids for.
 * @param string $type
 *   (optional) Filters the results for a given group type.
 *
 * @return array
 *   An array of group ids (gids).
 */
function group_outsider_gids($uid, $type = '') {
  $query = db_select('groups', 'g');
  $query->addField('g', 'gid');
  $query->where('g.gid NOT IN (SELECT gid FROM {group_membership} WHERE uid = :uid)', array(':uid' => $uid));

  if ($type) {
    $query->condition('g.type', $type);
  }

  return $query->execute()->fetchCol();
}

/**
 * Check whether a group role machine name exists.
 */
function group_role_exists($name) {
  return in_array($name, array('outsider', 'member')) || !!group_role_load($name);
}

/**
 * Return the possible subscription types.
 *
 * @return array
 *   An array where the keys are subscription type machine names and the values
 *   are translated strings describing what the subscription type really means.
 *
 * @todo Expose this with hooks?
 */
function group_subscription_types() {
  $types = array(
    'public' => t('<strong>Public</strong>: Users can freely subscribe to a group with the click of a button.'),
    'private' => t('<strong>Private</strong>: Users can issue a membership request which has to be approved.'),
    'invite' => t('<strong>Invite only</strong>: Users can only become a member after invitation.'),
  );

  return $types;
}

/**
 * Return the possible membership statuses.
 *
 * @return array
 *   An array where the keys are the integer constants that are saved to the
 *   database and the values are translated strings describing what the status
 *   code really means.
 *
 * @todo Expose this with hooks?
 */
function group_membership_statuses() {
  $statuses = array(
    GROUP_MEMBERSHIP_BLOCKED => t('Blocked'),
    GROUP_MEMBERSHIP_ACTIVE  => t('Active'),
    GROUP_MEMBERSHIP_INVITED => t('Invited'),
  );

  return $statuses;
}

/**
 * Return the possible group permissions.
 *
 * @return array
 *   An array where the keys are permission machine names and the values are
 *   the permission 'title' attributes as set in hook_group_permission().
 */
function group_permissions() {
  $permissions = module_invoke_all('group_permission');
  foreach ($permissions as &$permission) {
    $permission = $permission['title'];
  }

  return $permissions;
}
