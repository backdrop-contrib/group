<?php

/**
 * @file
 * Contains all admin functions for the Group Node module.
 */

/**
 * Menu settings callback.
 *
 * @see gnode_menu().
 */
function gnode_settings($form, &$form_state) {
  $form['group_node_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Group node access mode'),
    '#default_value' => _gnode_get_mode(),
    '#description' => t('In safe mode, access to nodes within a Group is controlled by the Group module alone. In compliance mode, other Drupal modules may allow access to nodes within a Group which permissions within the Group may have not allowed. If this setting is changed then you will be prompted to rebuild node access permissions.'),
    '#options' => array(
      GROUP_NODE_SAFE_MODE => t('Safe mode'),
      GROUP_NODE_COMPLIANCE_MODE => t('Compliance mode'),
    ),
  );

  $form['#submit'][] = 'gnode_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Form submission handler.
 *
 * @see gnode_settings().
 */
function gnode_settings_form_submit(&$form, &$form_submit) {
  $selected_group_mode = $form_submit['values']['group_node_mode'];

  // If the access mode has changed then node access needs rebuilding.
  if ($selected_group_mode !== _gnode_get_mode()) {
    node_access_needs_rebuild(TRUE);
  }
}
