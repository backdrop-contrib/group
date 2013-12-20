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
      'description' => t('Invite members by their user name on the website'),
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
