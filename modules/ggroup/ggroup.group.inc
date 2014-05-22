<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_group_permission().
 *
 * Adds permissions for creating subgroups.
 */
function ggroup_group_permission() {
  $permissions = array();

  // Set permissions for the enabled subgroup types.
  foreach (group_types() as $group_type) {
    $permissions["create $group_type->name"] = array(
      'title' => t('Create %group_type as a subgroup', array('%group_type' => $group_type->label())),
      'warning' => t("This only grants the permission to create subgroups of this type.<br />Which group types can actually become subgroups is configured under every group type's config link."),
    );
  }

  return $permissions;
}

/**
 * Implements hook_group_membership_status_info().
 */
function ggroup_group_membership_status_info() {
  $info['inherited'] = array(
    'title' => t('Inherited'),
    'active' => TRUE,
  );

  $info['inherited-overridden'] = array(
    'title' => t('Inherited (overridden)'),
    'active' => TRUE,
  );

  return $info;
}
