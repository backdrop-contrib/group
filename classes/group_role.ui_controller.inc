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

    $items["$this->path/manage/%entity_object/permissions"] = array(
      'title' => 'Permissions',
      'description' => 'Edit role permissions',
      // @todo
      'access callback' => TRUE,
      //'page callback' => 'entity_ui_get_form',
      //'page arguments' => array($this->entityType, $entity, 'add'),
      //'access callback' => 'entity_access',
      //'access arguments' => array('create', $this->entityType),
      //'file' => 'forms/group.inc',
      //'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 0,
    );

    return $items;
  }

  /**
   * Returns the operation count for calculating colspans.
   */
  protected function operationCount() {
    return parent::operationCount() + 1;
  }

  /**
   * Generates the row for the passed entity and may be overridden in order to
   * customize the rows.
   *
   * @param $additional_cols
   * Additional columns to be added after the entity label column.
   */
  protected function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {
    $row = parent::overviewTableRow($conditions, $id, $entity, $additional_cols);
    $new = array(
      l(t('permissions'), $this->path . '/manage/' . $id . '/permissions')
    );

    array_splice($row, 3, 0, $new);

    return $row;
  }
}
