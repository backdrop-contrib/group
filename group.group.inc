<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_group_permission().
 */
function group_group_permission() {
  $permissions = array(
    'administer group' => array(
      'title' => t('Administer group'),
      'description' => t('Administer the group, its content and members'),
      'restrict access' => TRUE,
    ),
    'administer members' => array(
      'title' => t('Administer group members'),
      'description' => t('Administer the group members'),
      'restrict access' => TRUE,
    ),
    'access member overview' => array(
      'title' => t('Access the member overview page'),
    ),
    'edit group' => array(
      'title' => t('Edit group'),
      'description' => t('Edit the group information'),
    ),
    'delete group' => array(
      'title' => t('Delete group'),
      'description' => t('Delete the group'),
    ),
    'join group' => array(
      'title' => t('Join group'),
      'description' => t('Should not be used in combination with "Request group membership"'),
      'warning' => t('This permission has no effect when given to members.'),
    ),
    'request membership' => array(
      'title' => t('Request group membership'),
      'description' => t('Should not be used in combination with "Join group"'),
      'warning' => t('This permission has no effect when given to members.'),
    ),
    'leave group' => array(
      'title' => t('Leave group'),
      'warning' => t('This permission has no effect when given to outsiders.'),
    ),
    'view group' => array(
      'title' => t('View group'),
    ),
    'view members' => array(
      'title' => t('View group members'),
    ),
  );

  return $permissions;
}

/**
 * Implements hook_group_filters().
 */
function group_group_filters() {
  // Get a list of all group types.
  $group_types = array();
  foreach (group_types() as $name => $group_type) {
    $group_types[$name] = $group_type->label;
  }

  // Build a group type filter.
  $filters['type'] = array(
    'title' => t('Group type'),
    'options' => array(
      '[any]' => t('any'),
    ) + $group_types,
  );

  return $filters;
}

/**
 * Implements hook_group_operations().
 */
function group_group_operations() {
  $operations['delete'] = array(
    'label' => t('Delete selected groups'),
    'callback' => 'group_multiple_delete_confirm',
    'form callback' => TRUE,
  );

  return $operations;
}

/**
 * Implements hook_group_operation_links().
 */
function group_group_operation_links(Group $group) {
  $operations = array();

  // Retrieve the destination url.
  $destination = drupal_get_destination();

  // Add an 'edit' link if available.
  if (group_access('update', $group)) {
    $operations['edit'] = array(
      'title' => t('edit'),
      'href' => "group/$group->gid/edit",
      'query' => $destination,
    );
  }

  // Add a 'delete' link if available.
  if (group_access('delete', $group)) {
    $operations['delete'] = array(
      'title' => t('delete'),
      'href' => "group/$group->gid/delete",
      'query' => $destination,
    );
  }

  return $operations;
}

/**
 * Implements hook_group_member_filters().
 */
function group_group_member_filters() {
  // Build a status filter.
  $filters['status'] = array(
    'title' => t('Status'),
    'options' => array('[any]' => t('any')) + group_membership_status_types(),
  );

  return $filters;
}

/**
 * Implements hook_group_member_operations().
 *
 * Callbacks are situated in forms/group.members.inc.
 */
function group_group_member_operations(Group $group) {
  $operations['remove'] = array(
    'label' => t('Remove selected members'),
    'callback' => 'group_membership_multiple_delete_confirm',
    'form callback' => TRUE,
  );

  $operations['block'] = array(
    'label' => t('Block selected members'),
    'callback' => 'group_membership_block',
    'optgroup' => t('Block or unblock selected members'),
  );

  $operations['unblock'] = array(
    'label' => t('Unblock selected members'),
    'callback' => 'group_membership_unblock',
    'optgroup' => t('Block or unblock selected members'),
  );

  foreach (group_type_load($group->type)->getRoles(FALSE) as $group_role) {
    $operations["add-$group_role->name"] = array(
      'label' => $group_role->label,
      'callback' => 'group_membership_add_role',
      'callback arguments' => array($group_role->name),
      'optgroup' => t('Add role to selected members'),
    );

    $operations["remove-$group_role->name"] = array(
      'label' => $group_role->label,
      'callback' => 'group_membership_remove_role',
      'callback arguments' => array($group_role->name),
      'optgroup' => t('Remove role from selected members'),
    );
  }

  return $operations;
}

/**
 * Implements hook_group_member_operation_links().
 */
function group_group_member_operation_links(GroupMembership $group_membership) {
  $operations = array();

  // Retrieve the destination url.
  $destination = drupal_get_destination();

  // Add membership management links.
  if (group_access('administer members', group_load($group_membership->gid))) {
    $operations['edit-membership'] = array(
      'title' => t('edit'),
      'href' => 'group/' . $group_membership->gid . '/member/' . $group_membership->uid . '/edit',
      'query' => $destination,
    );

    $operations['cancel-membership'] = array(
      'title' => t('cancel'),
      'href' => 'group/' . $group_membership->gid . '/member/' . $group_membership->uid . '/cancel',
      'query' => $destination,
    );
  }

  return $operations;
}

/**
 * Implements hook_group_membership_status_info().
 */
function group_group_membership_status_info() {
  $info['active'] = array(
    'title' => t('Active'),
    'active' => TRUE,
  );

  $info['requested'] = array(
    'title' => t('Requested'),
    'active' => FALSE,
  );

  $info['blocked'] = array(
    'title' => t('Blocked'),
    'active' => FALSE,
  );

  return $info;
}

/**
 * Implements hook_group_membership_action_info().
 */
function group_group_membership_action_info() {
  $info['group_join'] = array(
    'label' => t('Join'),
    'description' => t('This group is free for anyone to join'),
    'access callback' => 'group_user_can_join',
    'action callback' => 'group_user_join',
  );

  $info['group_request_membership'] = array(
    'label' => t('Request membership'),
    'description' => t('This group requires admin approval to join'),
    'access callback' => 'group_user_can_request_membership',
    'action callback' => 'group_user_request_membership',
  );

  $info['group_leave'] = array(
    'label' => t('Leave'),
    'description' => t('You are member of this group'),
    'access callback' => 'group_user_can_leave',
    'action callback' => 'group_user_leave',
  );

  return $info;
}

/**
 * Check whether a user can join a group.
 *
 * @param Group $group
 *   The group to check access for.
 * @param object $account
 *   The user to check access for.
 * @param GroupMembership|false $group_membership
 *   The group membership if one exists already.
 *
 * @return bool
 *   Whether the user can join the group.
 */
function group_user_can_join(Group $group, $account, $group_membership) {
  return empty($group_membership) && group_access('join group', $group, $account);
}

/**
 * Join a group.
 *
 * @param Group $group
 *   The group to join.
 * @param object $account
 *   The user that wants to join.
 */
function group_user_join(Group $group, $account) {
  $group->addMember($account->uid);
  drupal_set_message(t('You have joined the group.'));
}

/**
 * Check whether a user can request to join a group.
 *
 * @param Group $group
 *   The group to check access for.
 * @param object $account
 *   The user to check access for.
 * @param GroupMembership|false $group_membership
 *   The group membership if one exists already.
 *
 * @return bool
 *   Whether the user can request a membership for the group.
 */
function group_user_can_request_membership(Group $group, $account, $group_membership) {
  return empty($group_membership) && group_access('request membership', $group, $account);
}

/**
 * Request to join a group.
 *
 * @param Group $group
 *   The group to join.
 * @param object $account
 *   The user that wants to join.
 */
function group_user_request_membership(Group $group, $account) {
  // @todo requested_on and requested_by?
  $values = array(
    'gid' => $group->gid,
    'uid' => $account->uid,
    'status' => 'requested',
  );

  // Create and save the membership.
  $group_membership = entity_create('group_membership', $values);
  $group_membership->save();

  drupal_set_message(t('You have requested to join the group.'));
}

/**
 * Check whether a user can leave a group.
 *
 * @param Group $group
 *   The group to check access for.
 * @param object $account
 *   The user to check access for.
 * @param GroupMembership|false $group_membership
 *   The group membership of the user that wants to leave.
 *
 * @return bool
 *   Whether the user can leave the group.
 */
function group_user_can_leave(Group $group, $account, $group_membership) {
  return !empty($group_membership) && group_access('leave group', $group, $account);
}

/**
 * Leave a group.
 *
 * @param Group $group
 *   The group to leave.
 * @param object $account
 *   The user that wants to leave.
 * @param GroupMembership $group_membership
 *   The group membership of the user that wants to leave.
 */
function group_user_leave(Group $group, $account, $group_membership) {
  $group_membership->delete();
  drupal_set_message(t('You have left the group.'));
}
