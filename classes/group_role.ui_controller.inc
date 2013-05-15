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

    // Show the appropriate description for the overview page.
    $help = !empty($this->group_type)
      ? t("<p>These are <strong>group type specific roles</strong>, meaning they only apply to this group type. You can create global group roles by visiting <a href='@url'>the 'Group roles' page</a>.</p>", array('@url' => url('admin/group/role')))
      : t("<p>These are <strong>global group roles</strong>, meaning they apply to every group of every group type. Use this if you don't want to create the same role over and over again for every group type. A good usage example would be a <em>group moderator</em> role that has limited admin rights.</p><p>You can create group type specific roles under 'Roles' on the 'Permissions' tab of a group type.</p>");

    // Either show the roles for a certain group type or all global roles.
    $conditions = !empty($this->group_type)
      ? array('type' => $this->group_type)
      : array('global' => 1);

    // Apply the variables set above.
    $form['help'] = array('#markup' => $help);
    $form['table'] = $this->overviewTable($conditions);
    $form['pager'] = array('#theme' => 'pager');

    return $form;
  }

  /**
   * Generates the table headers for the overview table.
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $additional_header[] = t('permissions');
    $header = parent::overviewTableHeaders($conditions, $rows, $additional_header);

    // Drop the Status column for local group roles.
    if (!empty($this->group_type)) {
      unset($header[1 + count($additional_header)]);
    }

    return $header;
  }

  /**
   * Returns the operation count for calculating colspans.
   */
  protected function operationCount() {
    // Drop the Export action for local group roles.
    if (!empty($this->group_type)) {
      return parent::operationCount() - 1;
    }

    return parent::operationCount();
  }

  /**
   * Generates the row for the passed entity and may be overridden in order to
   * customize the rows.
   *
   * @param $additional_cols
   * Additional columns to be added after the entity label column.
   */
  protected function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {
    // Check if the group role or parent group type is ENTITY_FIXED.
    $role_fixed = entity_has_status('group_role', $entity, ENTITY_FIXED);
    $type_fixed = !empty($this->group_type)
      ? entity_has_status('group_type', group_type_load($this->group_type), ENTITY_FIXED)
      : FALSE;

    $action = $role_fixed || $type_fixed ? t('view') : t('edit');
    $additional_cols = array(
      l($action, $this->path . '/manage/' . $id . '/permissions'),
    );

    $row = parent::overviewTableRow($conditions, $id, $entity, $additional_cols);

    // Drop the Status column and Export action for local group roles.
    if (!empty($this->group_type)) {
      // Get the amount of extra columns.
      $count = count($additional_cols);

      // Get the indexes of Status and Export.
      // We increase operationCount to counter the decrease in that function.
      // @todo Change 3 to 4 when http://drupal.org/node/1995470 is committed.
      $i_status = $count + 1;
      $i_export = $count + ($role_fixed ? 3 : 1 + $this->operationCount() + 1);

      unset($row[$i_status], $row[$i_export]);
    }

    return $row;
  }
}
