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

  // Retrieve the group types that can be subgroups.
  $subgroups = array_filter(variable_get('ggroup_enabled_subgroups'));

  // Set permissions for the enabled subgroup types.
  foreach (group_types() as $group_type) {
    if (isset($subgroups[$group_type->name])) {
      $replace = array('%group_type' => $group_type->label);

      $permissions["create $group_type->name"] = array(
        'title' => t('Create %group_type as a subgroup', $replace),
      );
    }
  }

  return $permissions;
}
