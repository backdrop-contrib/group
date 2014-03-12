<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_group_permission().
 */
function gadd_group_permission() {
  $permissions = array(
    'add member' => array(
      'title' => t('Add members'),
      'description' => t('Forcibly add members by their username on the website'),
      'warning' => t('You only get the autocomplete if you have the sitewide "View user profiles" permission.'),
    ),
  );

  return $permissions;
}
