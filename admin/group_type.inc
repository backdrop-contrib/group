<?php
/**
 * @file
 * Group type editing UI.
 */

/**
 * Generates the group type editing form.
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

/**
 * Builds the role permission page.
 *
 * @param GroupType $type
 *   The group type to show the permissions page for.
 * @param GroupRole $role
 *   (optional) A single group role to show the page for.
 *
 * @todo Implement $role menu callback.
 * @todo Implement anonymous / registered / admin.
 */
function group_permission_form($form, &$form_state, $type, $role = NULL) {
  // Store if the permission descriptions should be hidden.
  $hide = system_admin_compact_mode();

  // Retrieve role names for columns.
  $roles = isset($role) ? array($role) : $type->getRoles();

  // Fetch permissions for all roles or the one selected role.
  $role_permissions = array();
  foreach ($roles as $role) {
    $role_permissions[$role->name] = $role->getPermissions();
  }

  // Store $roles for use in the theme function and submit handler.
  $form['roles'] = array(
    '#type' => 'value',
    '#value' => $roles,
  );

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

/**
 * Returns HTML for an individual group permission description.
 */
function theme_group_permission_description($variables) {
  if (!$variables['hide']) {
    $description = array();
    $permission = $variables['permission'];

    if (!empty($permission['description'])) {
      $description[] = $permission['description'];
    }

    if (!empty($permission['warning'])) {
      $description[] = '<em class="permission-warning">' . $permission['warning'] . '</em>';
    }

    if (!empty($description)) {
      return implode('<br />', $description);
    }
  }
}

/**
 * Returns HTML for the administer permissions page.
 */
function theme_group_permission_form($variables) {
  $form = $variables['form'];
  $roles = $form['roles']['#value'];

  // Theme the rows of the permission table.
  foreach (element_children($form['permissions']) as $pname) {
    $row = array();

    // Theme a module name row.
    if (is_numeric($pname)) {
      $row[] = array(
        'data' => drupal_render($form['permissions'][$pname]),
        'class' => array('module'),
        'id' => 'module-' . $form['permissions'][$pname]['#id'],
        'colspan' => count($form['roles']['#value']) + 1,
      );
    }

    // Theme a permission row.
    else {
      $row[] = array(
        'data' => drupal_render($form['permissions'][$pname]),
        'class' => array('permissions'),
      );

      foreach (element_children($form['checkboxes']) as $rname) {
        // Title content for both the label as the checkbox itself.
        $title = $roles[$rname]->label . ': ' . $form['permissions'][$pname]['#markup'];

        $form['checkboxes'][$rname][$pname]['#title'] = $title;
        $form['checkboxes'][$rname][$pname]['#title_display'] = 'invisible';
        $form['checkboxes'][$rname][$pname]['#attributes']['title'] = $title;

        $row[] = array(
          'data' => drupal_render($form['checkboxes'][$rname][$pname]),
          'class' => array('checkbox'),
        );
      }
    }

    $rows[] = $row;
  }

  // Compose the table header.
  $header = array(t('Permission'));
  foreach (element_children($form['roles']) as $rname) {
    $header[] = array(
      'data' => drupal_render($form['roles'][$rname]),
      'class' => array('checkbox'),
    );
  }

  // Generate and return the actual output.
  $output = theme('system_compact_link');
  $output .= theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'permissions'),
  ));
  $output .= drupal_render_children($form);

  return $output;
}
