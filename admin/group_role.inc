<?php
/**
 * @file
 * Group role editing UI.
 */

/**
 * Generates the group role editing form.
 */
function group_role_form($form, &$form_state, $group_role, $op = 'edit') {
  if ($op == 'clone') {
    $group_role->label .= ' (cloned)';
  }

  $group_types = array();
  foreach (group_types() as $name => $group_type) {
    $group_types[$name] = $group_type->label;
  }

  if (empty($group_role->global) && !empty($group_role->type)) {
    $form['type'] = array(
      '#type' => 'value',
      '#value' => $group_role->type,
    );
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $group_role->label,
    '#description' => t('The human-readable name of this group role.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $group_role->name,
    '#maxlength' => 32,
    '#disabled' => $group_role->isLocked() || $op == 'edit',
    '#machine_name' => array(
      'exists' => 'group_role_load',
      'source' => array('label'),
      'label' => t('Machine name'),
      'replace_pattern' => '[^a-z0-9_]+',
      'replace' => '_',
    ),
    '#element_validate' => array('form_validate_machine_name', 'entity_ui_validate_machine_name'),
    '#description' => t('A unique machine-readable name for this group role. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save group role'),
    '#weight' => 40,
  );

  if (!$group_role->isLocked() && $op == 'edit') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete group role'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('group_role_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function group_role_form_submit(&$form, &$form_state) {
  $group_role = entity_ui_form_submit_build_entity($form, $form_state);
  $group_role->save();

  $form_state['redirect'] = 'admin/group/role';
}

/**
 * Form API submit callback for the delete button.
 */
function group_role_form_submit_delete(&$form, &$form_state) {
  $group_role = $form_state['group_role']->name;

  $form_state['redirect'] = "admin/group/role/manage/$group_role/delete";
}
