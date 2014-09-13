<?php
/**
 * @file
 * Group membership editing UI.
 */

/**
 * Generates the group membership editing form.
 */
function group_membership_form($form, &$form_state, GroupMembership $group_membership, $op = 'edit') {
  $form['#entity_builders'][] = 'group_membership_restore_roles_eb';

  $group = group_load($group_membership->gid);
  $account = user_load($group_membership->uid);

  $message = 'Edit membership for %user in %group';
  $replace = array(
    '%user' => $account->name,
    '%group' => $group->label(),
  );
  drupal_set_title(t($message, $replace), PASS_THROUGH);

  $roles = array();
  foreach ($group->getRoles(FALSE) as $group_role) {
    $roles[$group_role->name] = $group_role->label();
  }

  $form['roles'] = array(
    '#title' => t('Group roles'),
    '#type' => 'checkboxes',
    '#options' => $roles,
    '#default_value' => $group_membership->roles,
  );

  // Load fields based on group type.
  field_attach_form('group_membership', $group_membership, $form, $form_state, entity_language('group_membership', $group_membership));

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Edit membership'),
    '#weight' => 40,
  );

  return $form;
}

/**
 * Form API validate callback for the group membership form.
 */
function group_membership_form_validate($form, &$form_state) {
  entity_form_field_validate('group_membership', $form, $form_state);
}

/**
 * Group membership edit submit callback.
 */
function group_membership_form_submit($form, &$form_state) {
  $group_membership = entity_ui_form_submit_build_entity($form, $form_state);
  $group_membership->changeRoles($form_state['values']['roles']);

  drupal_set_message(t('The membership was successfully edited.'));
}

/**
 * Entity builder: Restore original roles to the GroupMembership.
 *
 * Because entity_form_submit_build_entity() sets all non-field values as
 * entity properties, GroupMembership::changeRoles() may get confused. To
 * rectify this, we reset the roles to the original values.
 *
 * @see group_membership_form_submit()
 * @see entity_form_submit_build_entity()
 */
function group_membership_restore_roles_eb($entity_type, GroupMembership $group_membership, $form, $form_state) {
  $original = entity_load_unchanged('group_membership', $group_membership->mid);
  $group_membership->roles = $original->roles;
}