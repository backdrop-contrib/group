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
    // Avoid notices for group types without saved configuration.
    if (!isset($defaults[$group_type->name])) {
      $defaults[$group_type->name] = array();
    }

    // Get the available group roles for this group type.
    $group_roles = $group_type->getRoles(FALSE);

    // Convert the roles into valid checkbox options.
    foreach ($group_roles as &$group_role) {
      $group_role = $group_role->label;
    }

    // Add a checkbox for each available group role.
    $form['group_default_roles'][$group_type->name] = array(
      '#type' => 'checkboxes',
      '#title' => t('Default roles for @group_type', array('@group_type' => $group_type->label)),
      '#options' => $group_roles,
      '#default_value' => $defaults[$group_type->name],
    );

    // Show some help text when no roles are available.
    if (empty($group_roles)) {
      $message = t('There are no roles available for this group type.');
      $form['group_default_roles'][$group_type->name]['#description'] = $message;
    }
  }

  return system_settings_form($form);
}
