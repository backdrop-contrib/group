<?php
/**
 * @file
 * Group role permissions admin UI.
 */

/**
 * Builds the role permission page.
 *
 * @param GroupType $type
 *   The group type to show the permissions page for.
 * @param GroupRole $role
 *   (optional) A single group role to show the page for.
 *   When $role is provided, $type is ignored.
 *
 * @todo Implement anonymous / registered / admin.
 */
function group_permission_form($form, &$form_state, $type, $role = NULL) {
  // Store if the permission descriptions should be hidden.
  $hide = system_admin_compact_mode();

  // Retrieve role names for columns.
  $roles = isset($role) ? array($role->name => $role) : $type->getRoles();

  // Store $roles for use in the theme function and submit handler.
  $form['roles'] = array(
    '#type' => 'value',
    '#value' => $roles,
  );

  // Shortcut to the theme function if no roles are available.
  if (empty($roles)) {
    return $form;
  }

  // Fetch permissions for all roles or the one selected role.
  $role_permissions = array();
  foreach ($roles as $role) {
    $role_permissions[$role->name] = $role->getPermissions();
  }

  // Get a list of all the modules implementing a hook_group_permission()
  // and sort by display name.
  $modules = array();
  $module_info = system_get_info('module');

  foreach (module_implements('group_permission') as $module) {
    $modules[$module] = $module_info[$module]['name'];
  }
  asort($modules);

  // Populate the checkbox options and rows for every possible permission.
  $options = array();
  foreach ($modules as $module => $display_name) {
    if ($permissions = module_invoke($module, 'group_permission')) {
      // Add the module name first so we can show a separating row.
      $form['permissions'][] = array(
        '#markup' => $module_info[$module]['name'],
        '#id' => $module,
      );

      // Then add a row for every permission defined by the module.
      foreach ($permissions as $pname => $permission) {
        // Add the permission name to the checkbox options.
        $options[$pname] = '';

        // Set the warning message, if any.
        $warning = !empty($permission['restrict access'])
          ? t('Warning: Give to trusted roles only; this permission has security implications.')
          : '';

        // Fill in default values for the permission.
        $permission += array(
          'description' => '',
          'restrict access' => FALSE,
          'warning' => $warning,
        );

        // Theme the permission name and description.
        $form['permissions'][$pname] = array(
          '#type' => 'item',
          '#markup' => $permission['title'],
          '#description' => theme('group_permission_description', array(
            'permission' => $permission,
            'hide' => $hide,
          )),
        );

        foreach ($roles as $rname => $role) {
          // Save which checkboxes should already be checked.
          if (in_array($pname, $role_permissions[$rname])) {
            $status[$rname][] = $pname;
          }
        }
      }
    }
  }

  foreach ($roles as $rname => $role) {
    // Build the checkboxes for every role.
    $form['checkboxes'][$rname] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => isset($status[$rname]) ? $status[$rname] : array(),
      '#attributes' => array('class' => array('rid-' . $rname)),
    );

    // Add the role names to the table.
    $form['roles'][$rname] = array(
      '#markup' => check_plain($role->label),
      '#tree' => TRUE,
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save permissions'),
  );

  // @todo.
  $form['#attached']['js'][] = drupal_get_path('module', 'user') . '/user.permissions.js';

  return $form;
}

/**
 * Submit handler for group_permission_form().
 *
 * Saves the (de)selected permissions for a group type.
 */
function group_permission_form_submit($form, &$form_state) {
  foreach ($form_state['values']['roles'] as $rname => $role) {
    $role->changePermissions($form_state['values'][$rname]);
  }

  drupal_set_message(t('The changes have been saved.'));
}
