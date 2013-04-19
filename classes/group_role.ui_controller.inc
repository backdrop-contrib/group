<?php
/**
 * @file
 * Defines the Entity API UI class for group roles.
 */

/**
 * UI class for group roles.
 */
class GroupRoleUIController extends EntityDefaultUIController {

  /**
   * Class constructor.
   */
  public function __construct($entity_type, $entity_info) {
    parent::__construct($entity_type, $entity_info);
  }

  /**
   * Provides definitions for implementing hook_menu().
   */
  public function hook_menu() {
    $items = parent::hook_menu();

    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = 0;

    // @todo Temporarily disabled until we figure out where we want this.
    /*
    $items["$this->path/manage/%entity_object/permissions"] = array(
      'title' => 'Permissions',
      'description' => 'Edit role permissions',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_permission_form', 4),
      'load arguments' => array($this->entityType),
      //@todo Check this.
      'access callback' => 'user_access',
      'access arguments' => array('administer group types'),
      'file' => 'admin/group_role.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 5,
    );
    */

    return $items;
  }

  //@todo Show a table of roles per group type.
}
