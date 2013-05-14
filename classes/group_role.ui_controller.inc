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
   * The optional group type the UI is called for.
   *
   * @var string
   */
  protected $group_type;

  /**
   * Class constructor.
   */
  public function __construct($entity_type, $entity_info) {
    parent::__construct($entity_type, $entity_info);

    // By default we show the global UI.
    $this->group_type = '';
  }

  /**
   * Provides definitions for implementing hook_menu().
   */
  public function hook_menu() {
    $items = parent::hook_menu();

    $items[$this->path]['title'] = t('Group roles');
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = 0;

    $items["$this->path/manage/%group_role/permissions"] = array(
      'title' => 'Permissions',
      'description' => 'Edit permissions for this role',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_permission_form', NULL, 4),
      'access callback' => 'user_access',
      'access arguments' => array('administer group types'),
      'file' => 'admin/group_type.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 0,
    );

    return $items;
  }

  /**
   * Builds the entity overview form.
   */
  public function overviewForm($form, &$form_state) {
    if (!empty($form_state['build_info']['args'][1])) {
      $this->group_type = $form_state['build_info']['args'][1]->name;
    }

    // Either show the roles for a certain group type or all global roles.
    $conditions = !empty($this->group_type)
      ? array('type' => $this->group_type)
      : array('global' => 1);

    // Apply the conditions set above.
    $form['table'] = $this->overviewTable($conditions);
    $form['pager'] = array('#theme' => 'pager');

    return $form;
  }

  /**
   * Generates the table headers for the overview table.
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $header = parent::overviewTableHeaders($conditions, $rows, $additional_header);

    // Drop the Status column for local group roles.
    if (!empty($this->group_type)) {
      unset($header[1]);
    }

    return $header;
  }

  /**
   * Returns the operation count for calculating colspans.
   */
  protected function operationCount() {
    // Drop the Export action for local group roles.
    if (!empty($this->group_type)) {
      return parent::operationCount();
    }

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

    // Drop the Status column and Export action for local group roles.
    if (!empty($this->group_type)) {
      unset($row[1], $row[6]);
    }

    return $row;
  }
}
