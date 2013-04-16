<?php
/**
 * @file
 * Group configuration admin UI.
 */

/**
 * General configuration form.
 */
function group_config_form($form, &$form_state) {
  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search settings'),
    '#collapsible' => TRUE,
  );

  // Populate the checkboxes for use below.
  $group_types = group_types();
  foreach ($group_types as &$group_type) {
    $group_type = $group_type->label;
  }

  $ac_default = array('general' => t('Type independent search form'));
  $ac_options = array_merge($ac_default, $group_types);

  $form['search']['group_autocomplete_suffix'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Show autocomplete search suffix'),
    '#description' => t('Select which group types to show the autocomplete search suffix for. Enable this for group types where unique names are not being enforced.<br />Developers can then use group_autocomplete_result_gid() on autocomplete results to find out which group was selected.<br />The group search blocks provided by this module will automatically take this setting into account.'),
    '#options' => $ac_options,
    '#default_value' => variable_get('group_autocomplete_suffix'),
  );

  return system_settings_form($form);
}
