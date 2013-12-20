<?php
/**
 * @file
 * Group member invite UI.
 *
 * This file contains hook_form_alter() variants even though it isn't always
 * included from the .module file. The reason for this is that the forms they
 * alter reside in this file and are loaded by hook_menu() anyway.
 */

/**
 * Base form for inviting users.
 *
 * @see group_forms()
 */
function ginvite_form($form, &$form_state, Group $group) {
  $group_roles = group_type_load($group->type)->getRoles(FALSE);
  foreach ($group_roles as &$group_role) {
    $group_role = $group_role->label;
  }

  $form['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => ('Roles'),
    '#description' => ('Select the roles the member will receive should they accept the invite.'),
    '#options' => $group_roles,
    '#default_value' => array(),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send invitation'),
  );

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Build the group invite by user form.
 *
 * @see ginvite_forms()
 */
function group_form_ginvite_by_user_form_alter(&$form, &$form_state) {
  $form['user'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('Enter the username of the person you want to invite.'),
    '#autocomplete_path' => 'user/autocomplete',
    '#required' => TRUE,
    '#weight' => -1,
  );
}

/**
 * Validate handler for ginvite_by_user_form().
 *
 * @see group_form_ginvite_by_user_form_alter()
 */
function ginvite_by_user_form_validate($form, &$form_state) {
  $username = $form_state['values']['user'];

  // Check if the username exists.
  if (! ($account = user_load_by_name($username)) ) {
    form_set_error('user', t('User %user not found', array('%user' => $username)));
  }

  else {
    // Save the uid for reusability's sake.
    $form_state['values']['uid'] = $account->uid;

    // Check if the membership already exists.
    if ($form_state['build_info']['args'][0]->getMember($account->uid)) {
      form_set_error('user', t('User %user is already a member of this group', array('%user' => $username)));
    }
  }
}

/**
 * Submit handler for ginvite_by_user_form().
 *
 * @see group_form_ginvite_by_user_form_alter()
 */
function ginvite_by_user_form_submit($form, &$form_state) {
  $values = array(
    'gid' => $form_state['build_info']['args'][0]->gid,
    'uid' => $form_state['values']['uid'],
    'status' => GROUP_MEMBERSHIP_INVITED,
  );

  // Create the membership.
  $group_membership = entity_create('group_membership', $values);

  // Granting roles saves the membership.
  $group_membership->grantRoles(array_filter($form_state['values']['roles']));

  // Show a confirmation message.
  drupal_set_message(t('Invite sent to %user', array('%user' => $form_state['values']['user'])));
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Build the group invite by mail form.
 *
 * @see ginvite_forms()
 */
function group_form_ginvite_by_mail_form_alter(&$form, &$form_state) {
  $form['mail'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail address'),
    '#description' => t('Enter the e-mail address of the person you want to invite.'),
    '#required' => TRUE,
    '#weight' => -1,
  );
}

/**
 * Validate handler for ginvite_by_mail_form().
 *
 * @see group_form_ginvite_by_mail_form_alter()
 */
function ginvite_by_mail_form_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['mail'])) {
    form_set_error('mail', t('Invalid e-mail address.'));
  }
}

/**
 * Submit handler for ginvite_by_mail_form().
 *
 * @see group_form_ginvite_by_mail_form_alter()
 */
function ginvite_by_mail_form_submit($form, &$form_state) {
}
