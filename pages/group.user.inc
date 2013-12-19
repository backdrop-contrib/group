<?php
/**
 * @file
 * Page functions for user group info.
 */

/**
 * Generate the group info page for a given user.
 *
 * @param User $account
 *   The user to display data for.
 *
 * @return array
 *   The render array for this page.
 */
function group_user_page($account) {
  // Retrieve the destination url.
  $destination = drupal_get_destination();

  // Create the group invitation table if there are outstanding invites.
  if ($invites = GroupMembership::getUserInvites($account->uid)) {
    // Create the invitation table header.
    $header = array(
      'group' => t('Group name'),
      'roles' => t('Roles'),
      'operations' => t('Operations'),
    );

    $rows = array();
    foreach ($invites as $group_membership) {
      $wrapper = entity_metadata_wrapper('group_membership', $group_membership);

      // Extract the invitation roles.
      $group_roles = array();
      foreach ($wrapper->roles->getIterator() as $group_role_wrapper) {
        $group_roles[] = $group_role_wrapper->label();
      }

      // Populate the invitation operations.
      $operations['accept'] = array(
        'title' => t('accept'),
        'href' => "group/$group_membership->gid/invite/$account->uid/accept",
        'query' => $destination,
      );

      $operations['decline'] = array(
        'title' => t('decline'),
        'href' => "group/$group_membership->gid/invite/$account->uid/decline",
        'query' => $destination,
      );

      $operations['view group'] = array(
        'title' => t('view group'),
        'href' => "group/$group_membership->gid",
      );

      $rows[] = array(
        'group' => $wrapper->group->label(),
        'roles' => array(
          'data' => array(
            '#theme' => 'item_list__group_roles',
            '#items' => $group_roles,
          ),
        ),
        'operations' => array(
          'data' => array(
            '#theme' => 'links__group_invitation_operation_links',
            '#links' => $operations,
            '#attributes' => array('class' => array('links', 'inline')),
          ),
        ),
      );
    }

    $page['invites'] = array(
      '#theme' => 'table',
      '#title' => t('Pending invites'),
      '#header' => $header,
      '#rows' => $rows,
    );
  }

  return $page;
}
