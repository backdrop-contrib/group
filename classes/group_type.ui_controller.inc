<?php
/**
 * @file
 * Defines the Entity API UI class for group types.
 */

/**
 * UI class for group types.
 */
class GroupTypeUIController extends EntityDefaultUIController {

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
    $items[$this->path]['weight'] = -5;

    return $items;
  }
}
