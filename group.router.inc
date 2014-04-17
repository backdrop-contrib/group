<?php
/**
 * @file
 * Contains all router functions for the Group module.
 */

/**
 * Implements hook_menu().
 *
 * @see GroupUiController::hook_menu()
 * @see GroupRoleUiController::hook_menu()
 * @see GroupTypeUiController::hook_menu()
 */
function group_menu() {
  $items['user/%user/group'] = array(
    'title' => 'Group',
    'description' => 'Group info for a given user.',
    'page callback' => 'group_user_page',
    'page arguments' => array(1),
    'access callback' => 'user_edit_access',
    'access arguments' => array(1),
    'file' => 'pages/group.user.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['user/%user/group/list'] = array(
    'title' => 'My groups',
    'title callback' => 'group_user_page_title',
    'title arguments' => array(1),
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0,
  );

  $items['admin/group/config'] = array(
    'title' => 'Configuration',
    'description' => 'Configure the Group modules.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('group_config_form'),
    'access callback' => 'user_access',
    'access arguments' => array('configure group module'),
    'file' => 'admin/group.config.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['admin/group/config/group'] = array(
    'title' => 'Groups',
    'description' => 'Configuration regarding groups.',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -10,
  );

  return $items;
}

/**
 * Implements hook_menu_local_tasks_alter().
 *
 * Adds a local task to admin/group.
 */
function group_menu_local_tasks_alter(&$data, $router_item, $root_path) {
  // Add action link to 'group/add' on 'admin/group' page.
  if ($root_path == 'admin/group') {
    $item = menu_get_item('group/add');

    if ($item['access']) {
      $data['actions']['output'][] = array(
        '#theme' => 'menu_local_action',
        '#link' => $item,
      );
    }
  }
}

/**
 * Access callback: Checks whether the user has permission to add a group.
 *
 * @see GroupUiController::hook_menu()
 */
function _group_add_access() {
  foreach (group_types() as $group_type) {
    if (group_entity_access('create', NULL, NULL, 'group', $group_type->name)) {
      return TRUE;
    }
  }

  if (user_access('administer group types')) {
    // There are no group types defined that the user has permission to create,
    // but the user does have the permission to administer the group types, so
    // grant them access to the page anyway.
    return TRUE;
  }

  return FALSE;
}

/**
 * Access callback: Checks if a role can be added to a group type.
 *
 * @see GroupTypeUiController::hook_menu()
 */
function group_type_add_role_access(GroupType $group_type) {
  return user_access('administer group types') && !entity_has_status('group_type', $group_type, ENTITY_FIXED);
}

/**
 * Access callback: Checks if a user has access to their own membership.
 *
 * This is most commonly used for tasks that a user can perform on their own
 * membership such as cancelling their membership.
 *
 * This should not be used for administrative tasks over a membership such as
 * setting group roles or the blocking or unblocking of someone's membership.
 * The group permission 'administer members' should be used for that.
 *
 * @see GroupUiController::hook_menu()
 */
function group_membership_access(Group $group, $account) {
  global $user;

  // Short-circuit if the current user is the subject of the membership.
  // Otherwise, check if the current user can administer all members or
  // bypass group access.
  return $user->uid == $account->uid || group_access('administer members', $group, $user);
}

/**
 * Title callback: Returns the group title.
 *
 * @see GroupUiController::hook_menu()
 */
function group_page_title(Group $group) {
  return $group->title;
}

/**
 * Title callback for user/%user/group/list.
 */
function group_user_page_title($account) {
  global $user;

  if ($user->uid == $account->uid) {
    return t('My groups');
  }

  return t("@user's groups", array('@user' => $account->name));
}
