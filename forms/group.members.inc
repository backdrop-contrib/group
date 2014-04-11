<?php
/**
 * @file
 * Group member overview admin UI.
 */

/**
 * Builds the group member overview form.
 *
 * @todo Dynamic pager amount.
 */
function group_member_overview_form($form, &$form_state, Group $group) {
  // If the form was rebuilt from an operation, we show that form instead.
  if ($form_state['rebuild']) {
    $args = array_merge(array($form, &$form_state), $form_state['rebuild_info']['args']);
    $function = $form_state['rebuild_info']['callback'];

    return call_user_func_array($form_state['rebuild_info']['callback'], $args);
  }

  // The primary submit handler for this form is the filter.
  $form['#submit'] = array('group_member_filters_form_submit');

  // Build the overview form with filters and bulk operations.
  $form['filters'] = group_member_filters_form($group);
  $form['options'] = group_member_options_form($group);
  $form['members'] = group_members_form($group, 20);
  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}

/**
 * Builds the group member overview filters.
 */
function group_member_filters_form() {
  $filters = module_invoke_all('group_member_filters');

  $session = isset($_SESSION['group_member_filters'])
    ? $_SESSION['group_member_filters']
    : array();

  $i = 0;
  $fs_filters = array(
    '#type' => 'fieldset',
    '#title' => t('Show only members where'),
    '#theme' => 'exposed_filters__group_members',
  );

  // Loop through session saved filters.
  foreach ($session as $type => $value) {
    $value = $filters[$type]['options'][$value];

    $t_args = array(
      '%property' => $filters[$type]['title'],
      '%value' => $value,
    );

    $fs_filters['current'][] = ($i++)
      ? array('#markup' => t('and where %property is %value', $t_args))
      : array('#markup' => t('where %property is %value', $t_args));

    // Remove the option if it is already being filtered on.
    unset($filters[$type]);
  }

  if (count($filters)) {
    $fs_filters['status'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('clearfix')),
      '#prefix' => ($i ? '<div class="additional-filters">' . t('and where') . '</div>' : ''),
    );
  }

  $fs_filters['status']['filters'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('filters')),
  );

  foreach ($filters as $key => $filter) {
    $fs_filters['status']['filters'][$key] = array(
      '#type' => 'select',
      '#options' => $filter['options'],
      '#title' => $filter['title'],
      '#default_value' => '[any]',
    );
  }

  $fs_filters['status']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );

  if (count($filters)) {
    $fs_filters['status']['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => count($session) ? t('Refine') : t('Filter'),
    );
  }

  if (count($session)) {
    $fs_filters['status']['actions']['undo'] = array(
      '#type' => 'submit',
      '#value' => t('Undo'),
    );

    $fs_filters['status']['actions']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
    );
  }

  return $fs_filters;
}

/**
 * Filter form submit callback.
 */
function group_member_filters_form_submit($form, &$form_state) {
  $filters = module_invoke_all('group_member_filters');

  switch ($form_state['values']['op']) {
    case t('Filter'):
    case t('Refine'):
      // Apply every filter that has a choice selected other than 'any'.
      foreach ($filters as $filter => $options) {
        if (isset($form_state['values'][$filter]) && $form_state['values'][$filter] != '[any]') {
          // Flatten the options array to accommodate hierarchical/nested options.
          $flat_options = form_options_flatten($filters[$filter]['options']);

          // Only accept valid selections offered on the dropdown, block bad input.
          if (isset($flat_options[$form_state['values'][$filter]])) {
            $_SESSION['group_member_filters'][$filter] = $form_state['values'][$filter];
          }
        }
      }
      break;
    case t('Undo'):
      array_pop($_SESSION['group_member_filters']);
      break;
    case t('Reset'):
      $_SESSION['group_member_filters'] = array();
      break;
  }
}

/**
 * Builds the group member administration options.
 */
function group_member_options_form(Group $group) {
  $fs_options = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );

  if (!group_access('administer members', $group)) {
    $fs_options['#access'] = FALSE;
  }
  else {
    $options = array();
    foreach (module_invoke_all('group_member_operations', $group) as $operation => $array) {
      if (!empty($array['optgroup'])) {
        $options[$array['optgroup']][$operation] = $array['label'];
      }
      else {
        $options[$operation] = $array['label'];
      }
    }

    $fs_options['operation'] = array(
      '#type' => 'select',
      '#title' => t('Operation'),
      '#title_display' => 'invisible',
      '#options' => $options,
      '#default_value' => 'approve',
    );

    $fs_options['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#validate' => array('group_member_options_form_validate'),
      '#submit' => array('group_member_options_form_submit'),
    );
  }

  return $fs_options;
}

/**
 * Options form validate callback.
 */
function group_member_options_form_validate($form, &$form_state) {
  // Error if there are no members selected.
  if (!is_array($form_state['values']['members']) || !count(array_filter($form_state['values']['members']))) {
    form_set_error('', t('No members selected.'));
  }
}

/**
 * Options form submit callback.
 */
function group_member_options_form_submit($form, &$form_state) {
  $operations = module_invoke_all('group_member_operations', $form_state['build_info']['args'][0]);
  $operation = $operations[$form_state['values']['operation']];

  // Get all selected memberships.
  $mids = array_filter($form_state['values']['members']);

  // Build the arguments for the operation callback.
  $args = array($mids);
  if (isset($operation['callback arguments'])) {
    $args = array_merge($args, $operation['callback arguments']);
  }

  if (empty($operation['form callback'])) {
    call_user_func_array($operation['callback'], $args);
  }
  else {
    // Pass the operation form details.
    $form_state['rebuild_info']['callback'] = $operation['callback'];
    $form_state['rebuild_info']['args'] = $args;

    // We need to rebuild the form to be able to show the operation form.
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Builds the group member overview table.
 *
 * @param int $pager_limit
 *   The pager limit for the result page.
 */
function group_members_form(Group $group, $pager_limit) {
  // Retrieve the destination url for any 'edit' or 'delete' link only once.
  $destination = drupal_get_destination();

  $options = array();
  $header = array(
    'membername' => array(
      'data' => t('Member name'),
      'field' => 'u.name',
      'sort' => 'asc',
    ),
    'status' => array(
      'data' => t('Status'),
      'field' => 'gm.status',
    ),
    'roles' => t('Roles'),
    'operations' => t('Operations'),
  );

  // Retrieve members.
  $query = db_select('group_membership', 'gm')->extend('PagerDefault')->extend('TableSort');
  $query->join('users', 'u', 'gm.uid=u.uid');
  $query->addField('gm', 'mid');
  $query->condition('gm.gid', $group->gid);
  $query->condition('u.uid', 0, '<>');
  $query->orderByHeader($header);
  $query->limit($pager_limit);

  // Retrieve all active filters.
  $filters = isset($_SESSION['group_member_filters']) ? $_SESSION['group_member_filters'] : array();

  // Allow other modules to alter the query by passing on the filters.
  $query->addTag('group_member_overview');
  $query->addMetaData('filters', $filters);

  // Retrieve the membership status options.
  $statuses = group_membership_status_types();

  // Add the group memberships to the table.
  foreach (group_membership_load_multiple($query->execute()->fetchCol()) as $group_membership) {
    $wrapper = entity_metadata_wrapper('group_membership', $group_membership);

    $options[$group_membership->mid] = array(
      'membername' => l($wrapper->user->label(), "user/$group_membership->uid"),
      'status' => $statuses[$group_membership->status],
    );

    $member_roles = array();
    foreach ($wrapper->roles->getIterator() as $delta => $group_role_wrapper) {
      $member_roles[] = $group_role_wrapper->label();
    }

    $options[$group_membership->mid]['roles'] = array(
      'data' => array(
        '#theme' => 'item_list__group_roles',
        '#items' => $member_roles,
      ),
    );

    // Start with an empty operations array.
    $options[$group_membership->mid]['operations'] = array();

    // Gather all operation links.
    $operations = module_invoke_all('group_member_operation_links', $group_membership);

    if (count($operations) > 1) {
      // Render an unordered list of operations links.
      $options[$group_membership->mid]['operations'] = array(
        'data' => array(
          '#theme' => 'links__group_member_operation_links',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      );
    }
    elseif (!empty($operations)) {
      // Render the first and only operation as a link.
      $link = reset($operations);

      // Pass in $link as $options, they share the same keys.
      $options[$group_membership->mid]['operations'] = l($link['title'], $link['href'], $link);
    }
  }

  $form['members'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No members available'),
  );

  // Transform into a table if the user can't perform any operations.
  if (!group_access('administer members', $group)) {
    unset($form['members']['#type'], $form['members']['#options']);
    $form['members']['#theme'] = 'table';
    $form['members']['#rows'] = $options;
  }

  return $form;
}

/**
 * Multiple group membership deletion confirmation form.
 */
function group_membership_multiple_delete_confirm($form, &$form_state, $mids) {
  $form['#submit'][] = 'group_membership_multiple_delete_confirm_submit';

  $form['members'] = array(
    '#prefix' => '<ul>',
    '#suffix' => '</ul>',
    '#tree' => TRUE,
  );

  foreach (group_membership_load_multiple($mids) as $mid => $group_membership) {
    $wrapper = entity_metadata_wrapper('group_membership', $group_membership);

    if (!isset($gid)){
      $gid = $group_membership->gid;
    }

    $form['members'][$mid] = array(
      '#type' => 'hidden',
      '#value' => $mid,
      '#prefix' => '<li>',
      '#suffix' => $wrapper->user->label() . "</li>\n",
    );
  }

  $question = format_plural(count($mids),
    'Are you sure you want to remove this member?',
    'Are you sure you want to remove these members?'
  );

  return confirm_form($form, $question, "group/$gid/members", NULL, t('remove'));
}

/**
 * Multiple group deletion submit callback.
 */
function group_membership_multiple_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $mids = array_keys($form_state['values']['members']);
    $count = count($mids);

    group_membership_delete_multiple($mids);
    watchdog('group', 'Removed @count members.', array('@count' => $count));
    drupal_set_message(format_plural($count, 'Removed 1 member.', 'Removed @count members.'));
  }
}

/**
 * Group membership add role callback.
 */
function group_membership_add_role($mids, $role) {
  $roles = array($role);

  foreach (group_membership_load_multiple($mids) as $group_membership) {
    $group_membership->grantRoles($roles);
  }
}

/**
 * Group membership remove role callback.
 */
function group_membership_remove_role($mids, $role) {
  $roles = array($role);

  foreach (group_membership_load_multiple($mids) as $group_membership) {
    $group_membership->revokeRoles($roles);
  }
}

/**
 * Group membership block callback.
 *
 * @todo Save the previous status.
 */
function group_membership_block($mids) {
  foreach (group_membership_load_multiple($mids) as $group_membership) {
    $group_membership->status = 'blocked';
    $group_membership->save();
  }
}

/**
 * Group membership unblock callback.
 *
 * @todo Reinstate the previous status.
 */
function group_membership_unblock($mids) {
  foreach (group_membership_load_multiple($mids) as $group_membership) {
    $group_membership->status = 'active';
    $group_membership->save();
  }
}

/**
 * Group membership edit form.
 */
function group_membership_edit_form($form, &$form_state, Group $group, $account) {
  $group_membership = $group->getMember($account->uid);

  $message = 'Edit membership for %user in %group';
  $replace = array(
    '%user' => $account->name,
    '%group' => $group->title,
  );
  drupal_set_title(t($message, $replace), PASS_THROUGH);

  $roles = array();
  foreach ($group->getRoles(FALSE) as $group_role) {
    $roles[$group_role->name] = $group_role->label;
  }

  $form['roles'] = array(
    '#title' => t('Group roles'),
    '#type' => 'checkboxes',
    '#options' => $roles,
    '#default_value' => $group_membership->roles,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Edit membership'),
  );

  return $form;
}

/**
 * Group membership edit submit callback.
 */
function group_membership_edit_form_submit($form, &$form_state) {
  list($group, $account) = $form_state['build_info']['args'];

  $group_membership = $group->getMember($account->uid);
  $group_membership->changeRoles($form_state['values']['roles']);

  drupal_set_message(t('The membership was edited.'));
}

/**
 * Group membership cancel form.
 */
function group_membership_cancel_form($form, &$form_state, Group $group, $account) {
  global $user;

  $unsubscribe_self = $user->uid == $account->uid;

  $message = $unsubscribe_self
    ? 'Are you sure you want to unsubscribe from %group?'
    : 'Are you sure you want to unsubscribe %user from %group?';

  $confirm = $unsubscribe_self
    ? 'Yes, cancel my membership'
    : 'Yes, cancel the membership';

  $replace = array(
    '%user' => $account->name,
    '%group' => $group->title,
  );

  $redirect = $unsubscribe_self
    ? "group/$group->gid"
    : "group/$group->gid/member";

  return confirm_form($form, t($message, $replace), $redirect, NULL, t($confirm), t('No, take me back!'));
}

/**
 * Group membership cancel submit callback.
 */
function group_membership_cancel_form_submit($form, &$form_state) {
  list($group, $account) = $form_state['build_info']['args'];
  $group->removeMember($account->uid);

  drupal_set_message(t('The membership was cancelled.'));
}
