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
      'access callback' => 'user_access',
      'access arguments' => array('view groups'),
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    $items['group/autocomplete/type/%entity_object'] = array(
      'title' => 'Group type specific autocomplete',
      'title callback' => 'check_plain',
      'page callback' => 'group_autocomplete_by_type',
      'page arguments' => array(3),
      'load arguments' => array('group_type'),
      'access callback' => 'user_access',
      'access arguments' => array('view groups'),
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    $items['group/add'] = array(
      'title' => 'Add group',
      'page callback' => 'group_add_page',
      'access callback' => '_node_add_access',
      'access callback' => TRUE,
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
    );

    foreach (group_types() as $name => $type) {
      $entity = entity_create($this->entityType, array());
      $entity->type = $name;

      $items["group/add/$name"] = array(
        'title' => $type->label,
        'title callback' => 'check_plain',
        'description' => "Create $type->label",
        'page callback' => 'entity_ui_get_form',
        'page arguments' => array($this->entityType, $entity, 'add'),
        'access callback' => 'entity_access',
        'access arguments' => array('create', $this->entityType),
        'file' => 'forms/group.inc',
        'file path' => drupal_get_path('module', 'group'),
      );
    }

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
}
