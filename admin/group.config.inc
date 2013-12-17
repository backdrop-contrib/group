<?php
/**
 * @file
 * Group configuration admin UI.
 */

/**
 * General configuration form.
 *
 * Configure the default roles every group type should set.
 *
 * @see GroupController::save()
 */
function group_config_form($form, &$form_state) {
  $defaults = variable_get('group_default_roles');

  $form['group_default_roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default roles'),
    '#description' => t('Configure the roles that should be assigned to a user when they create a group.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  foreach (group_types() as $group_type) {
    // Fall back to the default of group_admin.
    if (!isset($defaults[$group_type->name])) {
      $defaults[$group_type->name] = array('group_admin' => 'group_admin');
    }

    // Populate the checkboxes for use below.
    $group_roles = $group_type->getRoles(FALSE);
    foreach ($group_roles as &$group_role) {
      $group_role = $group_role->label;
    }

    $form['group_default_roles'][$group_type->name] = array(
      '#type' => 'checkboxes',
      '#title' => t('Default roles for @group_type', array('@group_type' => $group_type->label)),
      '#options' => $group_roles,
      '#default_value' => $defaults[$group_type->name],
    );
  }

  return system_settings_form($form);
}
