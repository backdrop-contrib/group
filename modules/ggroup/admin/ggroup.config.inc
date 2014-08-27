<?php
/**
 * @file
 * Subgroup configuration admin UI.
 */

/**
 * Add subgroup configuration to group type configuration.
 */
function ggroup_config_form($form, &$form_state, GroupType $group_type) {
  // Load the saved configuration.
  $defaults = isset($group_type->config['subgroup'])
    ? $group_type->config['subgroup']
    : array();

  $form['info'] = array(
    '#markup' => t('Below you will find a collapsible fieldset for every known group type where you can choose whether groups of that type can become subgroups of this type.<br />Furthermore, you can configure which group roles are to be inherited by members of the parent group.'),
  );

  // Set a value tree so we can easily copy over the submitted data later on.
  $form['subgroup']['#tree'] = TRUE;

  // Shows a fieldset for each group type, allowing the user to check whether
  // the edited group type can have subgroups of this type added.
  foreach (group_types() as $type => $sub_group_type) {
    $form['subgroup'][$type] = array(
      '#type' => 'fieldset',
      '#title' => $sub_group_type->label(),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $message = 'Enable %group_type as a subgroup of this group type';
    $replace = array('%group_type' => $sub_group_type->label());
    $default = isset($defaults[$type]);

    $form['subgroup'][$type]['enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t($message, $replace),
      '#default_value' => $default,
    );

    // Get the group roles of the subgroup type as checkbox options.
    $options = $sub_group_type->getRoles();
    foreach ($options as &$sub_group_role) {
      $sub_group_role = $sub_group_role->label();
    }

    // Anonymous or outsider cannot be inherited.
    unset($options['anonymous'], $options['outsider']);

    // Configuration for inheritance is shown on a per group role basis.
    foreach ($group_type->getRoles() as $role => $group_role) {
      // Anonymous or outsider cannot be inherited.
      if ($role == 'anonymous' || $role == 'outsider') {
        continue;
      }

      $message = 'Members who had the %group_role role automatically receive:';
      $replace = array('%group_role' => $group_role->label());
      $default = isset($defaults[$type][$role]) ? $defaults[$type][$role] : array();

      $form['subgroup'][$type][$role] = array(
        '#type' => 'checkboxes',
        '#title' => t($message, $replace),
        '#options' => $options,
        '#default_value' => $default,
      );
    }
  }

  $form['sync'] = array(
    '#type' => 'checkbox',
    '#title' => t('Re-synchronize inherited memberships'),
    '#description' => t('Check this to re-synchronize all inherited memberships upon saving the group type.<br />Useful if you previously granted inherited memberships too few or too many permissions.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for ggroup_config_form().
 */
function ggroup_config_form_submit($form, &$form_state) {
  $group_type = $form_state['build_info']['args'][0];

  // Unset the previous configuration so we can create a clean copy below.
  unset($group_type->config['subgroup']);

  // We only store the enabled subgroup types to have cleaner exports.
  foreach ($form_state['values']['subgroup'] as $type => $data) {
    if (empty($data['enabled'])) {
      continue;
    }

    // We don't need the enabled flag in exports either. The checkbox is mainly
    // there to allow us to use AJAX or other UX in the configuration form.
    unset($data['enabled']);

    // By setting data, we flag the subgroup type as enabled.
    $group_type->config['subgroup'][$type] = $data;
  }

  $group_type->save();

  if (!empty($form_state['values']['sync'])) {
    // @todo Resync the memberships.
  }
}
