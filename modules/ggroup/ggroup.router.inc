<?php
/**
 * @file
 * Contains all router functions for the Subgroup module.
 */

/**
 * Implements hook_menu().
 */
function ggroup_menu() {
  $items['group/%group/subgroup/add/%'] = array(
    'title' => 'Add subgroup',
    'title callback' => 'ggroup_add_subgroup_title',
    'title arguments' => array(4),
    'page callback' => 'ggroup_add_subgroup_form',
    'page arguments' => array(1, 4),
    'access callback' => 'ggroup_add_subgroup_access',
    'access arguments' => array(1, 4),
    'file' => 'forms/ggroup.inc',
  );

  $items['admin/group/config/subgroup'] = array(
    'title' => 'Subgroups',
    'description' => 'Configuration regarding subgroups.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ggroup_config_form'),
    'access callback' => 'user_access',
    'access arguments' => array('configure group module'),
    'file' => 'admin/ggroup.config.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => -5,
  );

  return $items;
}

/**
 * Implements hook_menu_local_tasks_alter().
 *
 * Adds subgroup creation local tasks to group/%group.
 */
function ggroup_menu_local_tasks_alter(&$data, $router_item, $root_path) {
  // Add action links for subgroup creation on group/%group.
  if ($root_path == 'group/%') {
    $gid = $router_item['original_map'][1];

    // Retrieve the group types that can be subgroups.
    $subgroups = array_filter(variable_get('ggroup_enabled_subgroups'));

    // Set local actions for the enabled subgroup types.
    foreach (group_types() as $group_type => $info) {
      if (isset($subgroups[$group_type])) {
        $item = menu_get_item("group/$gid/subgroup/add/$group_type");

        if ($item['access']) {
          $data['actions']['output'][] = array(
            '#theme' => 'menu_local_action',
            '#link' => $item,
          );
        }
      }
    }
  }
}

/**
 * Title callback for group/%group/subgroup/add/%.
 */
function ggroup_add_subgroup_title($group_type) {
  $types = group_types();
  return t('Add @group', array('@group' => $types[$group_type]->label));
}

/**
 * Access callback for group/%group/subgroup/add/%.
 */
function ggroup_add_subgroup_access($group, $group_type) {
  return group_access("create $group_type", $group);
}
