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
  $items['admin/config/people/group'] = array(
    'title' => 'Group settings',
    'description' => 'Configure the Group module.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('group_config_form'),
    'access callback' => 'user_access',
    'access arguments' => array('configure group module'),
    'file' => 'admin/group.config.inc',
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
    // but the user does have the permission to administer the content types, so
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
function group_type_add_role_access($group_type) {
  return user_access('administer group types') && !entity_has_status('group_type', $group_type, ENTITY_FIXED);
}

/**
 * Title callback: Returns the group title.
 *
 * @see GroupUiController::hook_menu()
 */
function group_page_title($group) {
  return check_plain($group->title);
}
