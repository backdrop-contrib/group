<?php
/**
 * @file
 * Page functions for groups.
 */

/**
 * Retrieves autocomplete suggestions for existing groups.
 *
 * This function will search for all existing groups the user
 * has view access to, regardless of group type.
 *
 * @param string $search
 *   The partial title of the group.
 *
 * @return string
 *   A JSONified array of matches.
 */
function group_autocomplete($search = '') {
  return group_autocomplete_by_type(NULL, $search);
}

/**
 * Retrieves autocomplete suggestions for existing groups.
 *
 * This function will search for all existing groups the user
 * has view access to, optionally filtered by group type.
 *
 * @param GroupType $type
 *   The group type to retrieve suggestions for. The search will
 *   traverse all known group types when no type is provided.
 * @param string $search
 *   The partial title of the group.
 *
 * @return string
 *   A JSONified array of matches.
 *
 * @todo Individual group access check.
 */
function group_autocomplete_by_type($type, $search = '') {
  $matches = array();

  if ($search) {
    $query = db_select('groups', 'g');
    $query->addField('g', 'title');
    $query->condition('title', db_like($search) . '%', 'LIKE');
    $query->range(0, 10);

    if (!empty($type)) {
      $query->condition('type', $type->name);
    }

    foreach ($query->execute()->fetchCol() as $title) {
      $match = check_plain($title);
      $matches[$match] = $match;
    }
  }

  drupal_json_output($matches);
}

/**
 * Displays add group links for available group types.
 *
 * Redirects to group/add/[type] if only one group type is available.
 */
function group_add_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }

  if ($content) {
    $output =  theme('admin_block_content', array('content' => $content));
  }
  else {
    $output = '<p>' . t('You have not created any group types yet. Go to the <a href="@create-group">group type creation page</a> to add a new group type.', array('@create-group' => url('admin/structure/group_type/add'))) . '</p>';
  }

  return $output;
}
