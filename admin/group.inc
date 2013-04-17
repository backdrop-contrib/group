<?php
/**
 * @file
 * Group overview admin UI.
 *
 * @see Group::overviewForm()
 * @see Group::overviewFormSubmit()
 */

/**
 * Returns the filter options for the overview page.
 */
function group_filters() {
  // Get a list of all group types.
  $group_types = array();
  foreach (group_types() as $name => $group_type) {
    $group_types[$name] = $group_type->label;
  }

  // Build a group type filter.
  $filters['type'] = array(
    'title' => t('Group type'),
    'options' => array(
      '[any]' => t('any'),
    ) + $group_types,
  );

  return $filters;
}

/**
 * Builds the group administration filter.
 */
function group_filter_form() {
  $filters = group_filters();

  $session = isset($_SESSION['group_overview_filter'])
    ? $_SESSION['group_overview_filter']
    : array();

  $i = 0;
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only groups where'),
    '#theme' => 'exposed_filters__group',
  );

  // Loop through session saved filters.
  foreach ($session as $type => $value) {
    $value = $filters[$type]['options'][$value];

    $t_args = array(
      '%property' => $filters[$type]['title'],
      '%value' => $value,
    );

    $form['filters']['current'][] = ($i++)
      ? array('#markup' => t('and where %property is %value', $t_args))
      : array('#markup' => t('where %property is %value', $t_args));

    // Remove the option if it is already being filtered on.
    unset($filters[$type]);
  }

  if (count($filters)) {
    $form['filters']['status'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('clearfix')),
      '#prefix' => ($i ? '<div class="additional-filters">' . t('and where') . '</div>' : ''),
    );
  }

  $form['filters']['status']['filters'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('filters')),
  );

  foreach ($filters as $key => $filter) {
    $form['filters']['status']['filters'][$key] = array(
      '#type' => 'select',
      '#options' => $filter['options'],
      '#title' => $filter['title'],
      '#default_value' => '[any]',
    );
  }

  $form['filters']['status']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );

  if (count($filters)) {
    $form['filters']['status']['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => count($session) ? t('Refine') : t('Filter'),
    );
  }

  if (count($session)) {
    $form['filters']['status']['actions']['undo'] = array(
      '#type' => 'submit',
      '#value' => t('Undo'),
    );

    $form['filters']['status']['actions']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
    );
  }

  return $form;
}

/**
 * Filter form submit callback.
 */
function group_filter_form_submit($form, &$form_state) {
  $filters = group_filters();

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
            $_SESSION['group_overview_filter'][$filter] = $form_state['values'][$filter];
          }
        }
      }
      break;
    case t('Undo'):
      array_pop($_SESSION['group_overview_filter']);
      break;
    case t('Reset'):
      $_SESSION['group_overview_filter'] = array();
      break;
  }
}

/**
 * Multiple group deletion confirmation form.
 */
function group_multiple_delete_confirm($form, &$form_state, $groups) {
  $form['groups'] = array(
    '#prefix' => '<ul>',
    '#suffix' => '</ul>',
    '#tree' => TRUE,
  );

  foreach ($groups as $gid => $value) {
    $title = db_query('SELECT title FROM {groups} WHERE gid = :gid', array(':gid' => $gid))->fetchField();
    $form['groups'][$gid] = array(
      '#type' => 'hidden',
      '#value' => $gid,
      '#prefix' => '<li>',
      '#suffix' => check_plain($title) . "</li>\n",
    );
  }

  $form['operation'] = array(
    '#type' => 'hidden',
    '#value' => 'delete',
  );

  $form['#submit'][] = 'group_multiple_delete_confirm_submit';

  $question = count($groups)
    ? t('Are you sure you want to delete this group?')
    : t('Are you sure you want to delete these groups?');

  return confirm_form($form, $question, 'admin/group', NULL, t('Delete'));
}

/**
 * Multiple group deletion submit callback.
 */
function group_multiple_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $groups = $form_state['values']['groups'];

    group_delete_multiple(array_keys($groups));

    $count = count($groups);
    watchdog('groups', 'Deleted @count groups.', array('@count' => $count));
    drupal_set_message(format_plural($count, 'Deleted 1 group.', 'Deleted @count groups.'));
  }

  $form_state['redirect'] = 'admin/group';
}

/**
 * Builds the group administration overview.
 *
 * @todo Copy node_admin_nodes().
 */
function group_admin_groups() {
  $session = isset($_SESSION['group_overview_filter'])
    ? $_SESSION['group_overview_filter']
    : array();

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'group');

  // Add all the filters to the query.
  foreach ($session as $property => $value) {
    $query->propertyCondition($property, $value);
  }

  // @todo work this out.
  /*
  if ($this->overviewPagerLimit) {
    $query->pager($this->overviewPagerLimit);
  }
  */

  $results = $query->execute();

  $gids = isset($results['group'])
    ? array_keys($results['group'])
    : array();

  $groups = ($gids)
    ? group_load_multiple($gids)
    : array();

  ksort($groups);

  $header = array(
    'title' => t('Group name'),
  );

  $options = array();
  foreach ($groups as $group) {
    $options[] = array(
      'title' => $group->title,
    );
  }

  $render = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No groups available.'),
  );

  return $render;
}
