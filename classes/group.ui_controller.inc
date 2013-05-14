<?php
/**
 * @file
 * Defines the Entity API UI class for groups.
 */

/**
 * UI class for groups.
 */
class GroupUIController extends EntityDefaultUIController {

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
    // Make this an entry in the Management menu.
    $items[$this->path] = array(
      'title' => 'Groups',
      'description' => 'Find and manage groups.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_overview_form', 'group'),
      'access arguments' => array('access group overview'),
      'weight' => -9,
    );

    // Make this an entry in the Management menu.
    $items["$this->path/groups"] = array(
      'title' => 'Groups',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );

    $items['group/autocomplete'] = array(
      'title' => 'Group autocomplete',
      'page callback' => 'group_autocomplete',
      // TRUE because of individual 'view' access checks on every group.
      'access callback' => TRUE,
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    $items['group/autocomplete/type/%group_type'] = array(
      'title' => 'Group type specific autocomplete',
      'title callback' => 'check_plain',
      'page callback' => 'group_autocomplete_by_type',
      'page arguments' => array(3),
      // TRUE because of individual 'view' access checks on every group.
      'access callback' => TRUE,
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    $items['group/add'] = array(
      'title' => 'Add group',
      'page callback' => 'group_add_page',
      'access callback' => '_group_add_access',
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
    );

    foreach (group_types() as $name => $type) {
      $group = entity_create('group', array('type' => $name));

      $items["group/add/$name"] = array(
        'title' => $type->label,
        'title callback' => 'check_plain',
        'description' => "Create $type->label",
        'page callback' => 'entity_ui_get_form',
        'page arguments' => array('group', $group, 'add'),
        'access callback' => 'group_entity_access',
        'access arguments' => array('create', NULL, NULL, 'group', $name),
        'file' => 'forms/group.inc',
        'file path' => drupal_get_path('module', 'group'),
      );
    }

    $items['group/%group/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_operation_form', 'group', 1, 'delete'),
      'access callback' => 'group_entity_access',
      'access arguments' => array('delete', 1),
      'weight' => 1,
      'type' => MENU_LOCAL_TASK,
      //'context' => MENU_CONTEXT_INLINE,
    );

    return $items;
  }

  /**
   * Builds the group overview form.
   */
  public function overviewForm($form, &$form_state) {
    ctools_include('group', 'group', 'admin');

    if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
      $groups = array_filter($form_state['values']['groups']);
      return group_multiple_delete_confirm($form, $form_state, $groups);
    }

    $form['filter'] = group_filter_form();
    $form['admin'] = group_admin_groups();

    return $form;
  }

  /**
   * Overview form submit callback.
   */
  public function overviewFormSubmit($form, &$form_state) {
    ctools_include('group', 'group', 'admin');
    group_filter_form_submit($form, $form_state);
  }

  /**
   * Builds the operation form.
   *
   * Overrides the default to only allow the deletion of groups.
   */
  public function operationForm($form, &$form_state, $group, $op) {
    if ($op == 'delete') {
      $confirm_question = t('Are you sure you want to delete %title?', array('%title' => $group->title));
      return confirm_form($form, $confirm_question, "group/$group->gid");
    }

    drupal_not_found();
    exit;
  }

  /**
   * Applies an operation to the given entity.
   *
   * Overrides the default to only allow the deletion of groups.
   */
  public function applyOperation($op, $group) {
    if ($op == 'delete') {
      entity_delete('group', $id);
      watchdog('group', 'Deleted %title.', array('%title' => $group->title));

      return t('Deleted %title.', array('%title' => $group->title));
    }

    return FALSE;
  }
}
