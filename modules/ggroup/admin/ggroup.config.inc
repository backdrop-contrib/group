<?php
/**
 * @file
 * Subgroup configuration admin UI.
 */

/**
 * General configuration form.
 */
function ggroup_config_form($form, &$form_state) {
  // Populate the checkboxes for use below.
  $group_types = group_types();
  foreach ($group_types as &$group_type) {
    $group_type = $group_type->label;
  }

  $form['ggroup_enabled_subgroups'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select which group types can become subgroups of other group types'),
    '#description' => t('You can use this setting to keep the permission overview clean or to prevent "Create subgroup" links from showing for user 1.'),
    '#options' => $group_types,
    '#default_value' => variable_get('ggroup_enabled_subgroups'),
  );

  return system_settings_form($form);
}
