<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_group_permission().
 */
function ginvite_group_permission() {
  $permissions = array(
    'invite members by user' => array(
      'title' => t('Invite members by user'),
      'description' => t('Invite members by their username on the website'),
      'warning' => t('You only get the autocomplete if you have the sitewide "View user profiles" permission.'),
    ),
    'invite members by mail' => array(
      'title' => t('Invite members by mail'),
      'description' => t('Invite members by their e-mail address'),
      'warning' => t('Registered e-mail addresses will get a user invite instead.'),
    ),
  );

  return $permissions;
}

/**
 * Implements hook_group_membership_status_info().
 */
function ginvite_group_membership_status_info() {
  $info['invited'] = array(
    'title' => t('Invited'),
    'active' => FALSE,
  );

  return $info;
}

/**
 * Implements hook_group_membership_action_info().
 */
function ginvite_group_membership_action_info() {
  $info['ginvite_accept'] = array(
    'label' => t('Accept'),
    'description' => t('You have been invited to join this group'),
    'access callback' => 'ginvite_user_is_invited',
    'action callback' => 'ginvite_user_accept',
  );

  $info['ginvite_decline'] = array(
    'label' => t('Decline'),
    'description' => t('You have been invited to join this group'),
    'access callback' => 'ginvite_user_is_invited',
    'action callback' => 'ginvite_user_decline',
  );

  return $info;
}

/**
 * Check whether a user has a pending group invitation.
 *
 * @param Group $group
 *   The group the invitation is for.
 * @param object $account
 *   The user the invitation is for.
 * @param GroupMembership $group_membership
 *   The group membership object to check for the invitation.
 *
 * @return bool
 *   Whether the user has an invitation.
 */
function ginvite_user_is_invited($group, $account, $group_membership) {
  return !empty($group_membership) && $group_membership->status == 'invited';
}

/**
 * Accept a pending group invitation.
 *
 * @param Group $group
 *   The group the invitation is for.
 * @param object $account
 *   The user the invitation is for.
 * @param GroupMembership $group_membership
 *   The group membership object for the invitation.
 */
function ginvite_user_accept($group, $account, $group_membership) {
  $group_membership->status = 'active';
  $group_membership->save();
  drupal_set_message(t('You have accepted the group invitation.'));
}

/**
 * Accept a pending group invitation.
 *
 * @param Group $group
 *   The group the invitation is for.
 * @param object $account
 *   The user the invitation is for.
 * @param GroupMembership $group_membership
 *   The group membership object for the invitation.
 */
function ginvite_user_decline($group, $account, $group_membership) {
  $group_membership->delete();
  drupal_set_message(t('You have declined the group invitation.'));
}
