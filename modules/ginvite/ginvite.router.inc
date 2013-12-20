<?php
/**
 * @file
 * Contains all router functions for the Group invite module.
 */

/**
 * Implements hook_menu().
 */
function ginvite_menu() {
  $items['group/%group/invite'] = array(
    'title' => 'Invites',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ginvite_by_user_form', 1),
    'access callback' => 'group_access',
    'access arguments' => array('invite members by user', 1),
    'file' => 'forms/ginvite.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['group/%group/invite/user'] = array(
    'title' => 'By user',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0,
  );

  $items['group/%group/invite/mail'] = array(
    'title' => 'By mail',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ginvite_by_mail_form', 1),
    'access callback' => 'group_access',
    'access arguments' => array('invite members by mail', 1),
    'file' => 'forms/ginvite.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['user/%user/group/invite'] = array(
    'title' => 'Invites',
    'description' => 'Group invitations for a given user.',
    'page callback' => 'ginvite_user_page',
    'page arguments' => array(1),
    'access callback' => 'user_edit_access',
    'access arguments' => array(1),
    'file' => 'pages/ginvite.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['user/%user/group/invite/%group/accept'] = array(
    'page callback' => 'ginvite_accept_callback',
    'page arguments' => array(1, 4),
    'access callback' => 'user_edit_access',
    'access arguments' => array(1),
    'file' => 'pages/ginvite.inc',
    'type' => MENU_CALLBACK,
  );

  $items['user/%user/group/invite/%group/decline'] = array(
    'page callback' => 'ginvite_decline_callback',
    'page arguments' => array(1, 4),
    'access callback' => 'user_edit_access',
    'access arguments' => array(1),
    'file' => 'pages/ginvite.inc',
    'type' => MENU_CALLBACK,
  );

  return $items;
}
