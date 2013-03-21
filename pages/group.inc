<?php
/**
 * @file
 * Page functions for groups.
 */

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
