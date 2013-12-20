<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_default_group_role().
 */
function group_default_group_role() {
  $group_roles = array();

  $group_admin = entity_create('group_role', array(
    'name' => 'group_admin',
    'label' => t('Administrator'),
    'weight' => 10,
    'permissions' => array_keys(module_invoke_all('group_permission')),
    'status' => ENTITY_FIXED,
  ));
  $group_roles['group_admin'] = group_role_import(entity_var_json_export($group_admin));

  return $group_roles;
}

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
function group_group_operation_links($group) {
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
    'options' => array('[any]' => t('any')) + group_membership_statuses(),
  );

  return $filters;
}

/**
 * Implements hook_group_member_operations().
 *
 * Callbacks are situated in forms/group.members.inc.
 */
function group_group_member_operations($group) {
  $operations['remove'] = array(
    'label' => t('Remove selected members'),
    'callback' => 'group_membership_multiple_delete_confirm',
    'form callback' => TRUE,
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
function group_group_member_operation_links($group_membership) {
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
