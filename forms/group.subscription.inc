<?php
/**
 * @file
 * Group subscription form generator.
 */

/**
 * Generate the subscription form for a group.
 */
function group_subscription_form($form, &$form_state, Group $group, $account = NULL) {
  // Make sure this file is available throughout the entire form lifetime.
  form_load_include($form_state, 'inc', 'group', 'forms/group.subscription');

  $subscription_type = group_type_load($group->type)->subscription_type;
  $subscription_controller = group_subscription_controller($subscription_type);

  return $subscription_controller->subscriptionForm($form, $form_state, $group, $account);
}

/**
 * Validate handler for group_subscription_form().
 */
function group_subscription_form_validate($form, &$form_state) {
  $group = $form_state['build_info']['args'][0];

  $subscription_type = group_type_load($group->type)->subscription_type;
  $subscription_controller = group_subscription_controller($subscription_type);

  $subscription_controller->subscriptionFormValidate($form, $form_state);
}

/**
 * Submit handler for group_subscription_form().
 */
function group_subscription_form_submit($form, &$form_state) {
  $group = $form_state['build_info']['args'][0];

  $subscription_type = group_type_load($group->type)->subscription_type;
  $subscription_controller = group_subscription_controller($subscription_type);

  $subscription_controller->subscriptionFormSubmit($form, $form_state);
}
