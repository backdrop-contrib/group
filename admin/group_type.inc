<?php
/**
 * @file
 * Group type editing UI.
 */

/**
 * Builds the role permission page.
 *
 * @param GroupType $type
 *   The group type to show the permissions page for.
 * @param GroupRole $role
 *   (optional) A single group role to show the page for.
 *
 * @todo Implement theme_group_admin_permissions().
 * @todo Implement $role menu callback.
 * @todo Implement anonymous / registered / admin.
 */
function group_permission_form($form, &$form_state, $type, $role = NULL) {
  $form['#theme'] = 'user_admin_permissions';

  // Retrieve role names for columns.
  $roles = isset($role) ? array($role) : $type->getRoles();

  // Fetch permissions for all roles or the one selected role.
  $role_permissions = array();
  foreach ($roles as $role) {
    $role_permissions[$role->name] = $role->getPermissions();
  }

  // Store $roles for use when saving the data.
  // @todo rename to roles when we wrote theme_...
  $form['role_names'] = array(
    '#type' => 'value',
    '#value' => $roles,
  );

  // Render role/permission overview:
  $options = array();
  $module_info = system_get_info('module');
  $hide_descriptions = system_admin_compact_mode();

  // Get a list of all the modules implementing a hook_group_permission() and
  // sort by display name.
  $modules = array();
  foreach (module_implements('group_permission') as $module) {
    $modules[$module] = $module_info[$module]['name'];
  }
  asort($modules);

  foreach ($modules as $module => $display_name) {
    if ($permissions = module_invoke($module, 'group_permission')) {
      $form['permission'][] = array(
        '#markup' => $module_info[$module]['name'],
        '#id' => $module,
      );

      foreach ($permissions as $perm => $perm_item) {
        // Fill in default values for the permission.
        $perm_item += array(
          'description' => '',
          'restrict access' => FALSE,
          'warning' => !empty($perm_item['restrict access']) ? t('Warning: Give to trusted roles only; this permission has security implications.') : '',
        );

        $options[$perm] = '';
        $form['permission'][$perm] = array(
          '#type' => 'item',
          '#markup' => $perm_item['title'],
          '#description' => theme('user_permission_description', array('permission_item' => $perm_item, 'hide' => $hide_descriptions)),
        );

        foreach ($roles as $name => $role) {
          // Builds arrays for checked boxes for each role
          if (in_array($perm, $role_permissions[$name])) {
            $status[$name][] = $perm;
          }
        }
      }
    }
  }

  // Have to build checkboxes here after checkbox arrays are built
  foreach ($roles as $name => $role) {
    $form['checkboxes'][$name] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => isset($status[$name]) ? $status[$name] : array(),
      '#attributes' => array('class' => array('rid-' . $name)),
    );

    $form['role_names'][$name] = array(
      '#markup' => check_plain($role->label),
      '#tree' => TRUE,
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save permissions'),
  );

  $form['#attached']['js'][] = drupal_get_path('module', 'user') . '/user.permissions.js';

  return $form;
}

/**
 * Generates the model type editing form.
 */
function group_type_form($form, &$form_state, $group_type, $op = 'edit') {
  if ($op == 'clone') {
    $group_type->label .= ' (cloned)';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $group_type->label,
    '#description' => t('The human-readable name of this group type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $group_type->name,
    '#maxlength' => 32,
    '#disabled' => $group_type->isLocked() || $op == 'edit',
    '#machine_name' => array(
      'exists' => 'group_type_load',
      'source' => array('label'),
      'label' => t('Machine name'),
      'replace_pattern' => '[^a-z0-9_]+',
      'replace' => '_',
    ),
    '#element_validate' => array('form_validate_machine_name', 'entity_ui_validate_machine_name'),
    '#description' => t('A unique machine-readable name for this group type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save group type'),
    '#weight' => 40,
  );

  if (!$group_type->isLocked() && $op == 'edit') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete group type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('group_type_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function group_type_form_submit(&$form, &$form_state) {
  $group_type = entity_ui_form_submit_build_entity($form, $form_state);
  $group_type->save();

  $form_state['redirect'] = 'admin/group/type';
}

/**
 * Form API submit callback for the delete button.
 */
function group_type_form_submit_delete(&$form, &$form_state) {
  $group_type = $form_state['group_type']->name;

  $form_state['redirect'] = "admin/group/type/manage/$group_type/delete";
}
