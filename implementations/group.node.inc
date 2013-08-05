<?php
/**
 * @file
 * Contains Group's implementation of the Node module hooks and forms.
 *
 * Please note that this is an additional layer on top of the Entity
 * implementation. For instance: we do not need hook_node_delete() because
 * our implementation of hook_entity_delete() covers everything.
 *
 * Furthermore, we attach a 'group' property onto nodes that stores the group
 * id of the group it belongs to. If a node is not group content, the property
 * will remain at its default state of 0.
 */

/**
 * We use the module author's year of birth as the grant id.
 */
define('GROUP_BYPASS_GRANT_ID', 1986);

/**
 * Implements hook_group_permission().
 *
 * Polyfills permissions for the Node module.
 */
function node_group_permission() {
  $permissions = array();

  foreach (node_type_get_types() as $node_type) {
    $replace = array('%node_type' => $node_type->name);

    $permissions["view $node_type->type"] = array(
      'title' => t('%node_type: View content', $replace),
    );
    $permissions["create $node_type->type"] = array(
      'title' => t('%node_type: Create new content', $replace),
    );
    $permissions["edit own $node_type->type content"] = array(
      'title' => t('%node_type: Edit own content', $replace),
    );
    $permissions["edit any $node_type->type content"] = array(
      'title' => t('%node_type: Edit any content', $replace),
    );
    $permissions["delete own $node_type->type content"] = array(
      'title' => t('%node_type: Delete own content', $replace),
    );
    $permissions["delete any $node_type->type content"] = array(
      'title' => t('%node_type: Delete any content', $replace),
    );
  }

  return $permissions;
}

/**
 * Implements hook_node_access().
 *
 * We rely on the grant system to allow access to a node. Only node creation
 * cannot be handled by the grant system and is therefore taken care of here.
 *
 * @see group_node_grants().
 * @see group_node_access_records().
 */
function group_node_access($node, $op, $account) {
  if (is_string($node)) {
    if ($op == 'create') {
      // If the user can bypass group access, he is allowed access.
      if (user_access('bypass group access', $account)) {
        return NODE_ACCESS_ALLOW;
      }

      // Check for all group types if an outsider can create nodes of the given
      // node type in them.
      foreach (group_types() as $type => $group_type) {
        $has_access = in_array('administer group', $group_type->outsider_permissions);
        $has_access = $has_access || in_array("create $node", $group_type->outsider_permissions);

        // If the group type allows access to outsiders and the provided user
        // has as at least one group of this type he is an outsider to, we
        // allow access.
        if ($has_access && group_outsider_gids($account->uid, $type)) {
          return NODE_ACCESS_ALLOW;
        }
      }

      // Check the user's groups for creation rights.
      foreach (group_load_by_member($account->uid) as $group) {
        $has_access = group_access('administer group', $group, $account);
        $has_access = $has_access || group_access("create $node", $group, $account);

        if ($has_access) {
          return NODE_ACCESS_ALLOW;
        }
      }
    }
  }

  return NODE_ACCESS_IGNORE;
}

/**
 * Implements hook_node_grants().
 *
 * @see group_node_access_records().
 */
function group_node_grants($account, $op) {
  $grants = array();

  // If the user can bypass group access, he only needs the master grant.
  if (user_access('bypass group access', $account)) {
    return array('group:bypass' => array(GROUP_BYPASS_GRANT_ID));
  }

  // Gather the machine names of all node types.
  $node_types = array_keys(node_type_get_types());

  // Provide grants for groups the user is an outsider to.
  foreach (group_types() as $type => $group_type) {
    if ($outsider_gids = group_outsider_gids($account->uid, $type)) {
      // If the user is an admin, he requires no further specific grants.
      if (in_array('administer group', $group_type->outsider_permissions)) {
        foreach ($outsider_gids as $gid) {
          $grants['group:administer'][] = $gid;
        }
        continue;
      }

      foreach ($node_types as $node_type) {
        // Shorten variable name for readability's sake.
        $permissions = $group_type->outsider_permissions;

        // Allow the user to view content of this type.
        if ($op == 'view' && in_array("view $node_type", $permissions)) {
          foreach ($outsider_gids as $gid) {
            $grants["group:$node_type:view"][] = $gid;
          }
        }
        // Allow the user to edit any content of this type.
        elseif ($op == 'update' && in_array("edit any $node_type", $permissions)) {
          foreach ($outsider_gids as $gid) {
            $grants["group:$node_type:update"][] = $gid;
          }
        }
        // Allow the user to delete any content of this type.
        elseif ($op == 'delete' && in_array("delete any $node_type", $permissions)) {
          foreach ($outsider_gids as $gid) {
            $grants["group:$node_type:delete"][] = $gid;
          }
        }
        // Allow the user to edit own content of this type.
        elseif ($op == 'update' && in_array("edit own $node_type", $permissions)) {
          foreach ($outsider_gids as $gid) {
            $grants["group:$gid:$node_type:update"][] = $account->uid;
          }
        }
        // Allow the user to delete own content of this type.
        elseif ($op == 'delete' && in_array("delete own $node_type", $permissions)) {
          foreach ($outsider_gids as $gid) {
            $grants["group:$gid:$node_type:delete"][] = $account->uid;
          }
        }
      }
    }
  }

  // Provide grants for groups the user is a member of.
  foreach (group_load_by_member($account->uid) as $group) {
    // If the user is an admin, he requires no further specific grants.
    if (group_access('administer group', $group, $account)) {
      $grants['group:administer'][] = $group->gid;
      continue;
    }

    foreach ($node_types as $node_type) {
      // Allow the user to view content of this type.
      if ($op == 'view' && group_access("view $node_type", $group, $account)) {
        $grants["group:$node_type:view"][] = $group->gid;
      }
      // Allow the user to edit any content of this type.
      elseif ($op == 'update' && group_access("edit any $node_type", $group, $account)) {
        $grants["group:$node_type:update"][] = $group->gid;
      }
      // Allow the user to delete any content of this type.
      elseif ($op == 'delete' && group_access("delete any $node_type", $group, $account)) {
        $grants["group:$node_type:delete"][] = $group->gid;
      }
      // Allow the user to edit own content of this type.
      elseif ($op == 'update' && group_access("edit own $node_type", $group, $account)) {
        $grants["group:$group->gid:$node_type:update"][] = $account->uid;
      }
      // Allow the user to delete own content of this type.
      elseif ($op == 'delete' && group_access("delete own $node_type", $group, $account)) {
        $grants["group:$group->gid:$node_type:delete"][] = $account->uid;
      }
    }
  }

  return $grants;
}

/**
 * Implements hook_node_access_records().
 *
 * We define the following realms:
 * - 'group:bypass': Grants full access to any user having the
 *   'bypass group access' permission. Because a realm id isn't really
 *   needed here, we specify the module author's date of birth: 1986.
 * - 'group:administer': Grants full access to any user having the
 *   'administer group' permission for the group the node belongs to. Because
 *   this realm is defined for every group, we use the group id as realm id.
 * - 'group:NODE_TYPE:OP': Grants access for the specified operation to any
 *    user having one of the following group permissions:
 *    - view NODE_TYPE
 *    - update any NODE_TYPE
 *    - delete any NODE_TYPE
 *    Because these realms are defined for every group, we use the group id as
 *    realm id.
 * - 'group:GROUP_ID:NODE_TYPE:OP': Grants access for the specified operation
 *   to any user having one of the following group permissions:
 *    - update own NODE_TYPE
 *    - delete own NODE_TYPE
 *    Because these realms are defined for specific group-user combinations, we
 *    should use the user id as the grant id. However, we still need to know
 *    what group the grant is for, so we incorporate the group id into the
 *    realm name. We chose to incorporate the group id instead of the user id
 *    because there will almost always be more users than groups.
 *
 * @see group_node_grants().
 */
function group_node_access_records($node) {
  $grants = array();

  // Add realms specific to this group.
  if (!empty($node->group)) {
    // Add the realm corresponding to the 'bypass group access' permission.
    $grants[] = array(
      'realm' => 'group:bypass',
      'gid' => GROUP_BYPASS_GRANT_ID,
      'grant_view' => 1,
      'grant_update' => 1,
      'grant_delete' => 1,
      'priority' => 0,
    );

    // Add the realm corresponding to the 'administer group' permission.
    $grants[] = array(
      'realm' => 'group:administer',
      'gid' => $node->group,
      'grant_view' => 1,
      'grant_update' => 1,
      'grant_delete' => 1,
      'priority' => 0,
    );

    // Add realms for published nodes.
    if ($node->status) {
      $defaults_any = array(
        'gid' => $node->group,
        'grant_view' => 0,
        'grant_update' => 0,
        'grant_delete' => 0,
        'priority' => 0,
      );

      $defaults_own = array(
        'gid' => $node->uid,
      ) + $defaults_any;

      // View any content of this type.
      $grants[] = array (
        'realm' => "group:$node->type:view",
        'grant_view' => 1,
      ) + $defaults_any;

      // Edit any content of this type.
      $grants[] = array (
        'realm' => "group:$node->type:update",
        'grant_update' => 1,
      ) + $defaults_any;

      // Delete any content of this type.
      $grants[] = array (
        'realm' => "group:$node->type:delete",
        'grant_delete' => 1,
      ) + $defaults_any;

      // Edit own content of this type.
      $grants[] = array (
        'realm' => "group:$node->group:$node->type:update",
        'grant_update' => 1,
      ) + $defaults_own;

      // Delete own content of this type.
      $grants[] = array (
        'realm' => "group:$node->group:$node->type:delete",
        'grant_delete' => 1,
      ) + $defaults_own;
    }
  }

  return $grants;
}

/**
 * Implements hook_node_prepare().
 */
function group_node_prepare($node) {
  if (empty($node->group)) {
    $groups = isset($node->nid) ? group_get_entity_groups('node', $node->nid) : array();

    // Nodes can only have one group.
    $node->group = !empty($groups) ? key($groups) : 0;
  }
}

/**
 * Implements hook_node_insert().
 */
function group_node_insert($node) {
  group_node_save($node);
}

/**
 * Implements hook_node_update().
 */
function group_node_update($node) {
  // Delete all existing links.
  $query = db_delete('group_entity');
  $query->condition('entity_id', $node->nid);
  $query->condition('entity_type', 'node');
  $query->execute();

  // Saving takes care of creating new links.
  group_node_save($node);
}

/**
 * Helper for hook_node_insert() and hook_node_update().
 */
function group_node_save($node) {
  if (!empty($node->group)) {
    group_load($node->group)->addEntity($node->nid, 'node', $node->type);
  }
}

/**
 * Implements hook_form_BASE_FORM_ID_alter().
 *
 * Adds a Group vertical tab to the node form.
 *
 * You can only select those groups that you can create nodes of this type in.
 * It would not make sense if someone could move nodes to a group where he does
 * not have creation rights.
 *
 * @see group_node_submit()
 */
function group_form_node_form_alter(&$form, $form_state) {
  global $user;
  $node_type = $form['#node']->type;

  // Gather all group ids an outsider can create nodes of the given type in.
  $outsider_gids = array();
  foreach (group_types() as $type => $group_type) {
    $has_access = $user->uid == 1;
    $has_access = $has_access || in_array('administer group', $group_type->outsider_permissions);
    $has_access = $has_access || in_array("create $node_type", $group_type->outsider_permissions);

    if ($has_access) {
      $outsider_gids = array_merge($outsider_gids, group_outsider_gids($user->uid, $type));
    }
  }

  // Allow access to all outsider groups of a group type with access.
  $user_groups = array();
  foreach (group_load_multiple($outsider_gids) as $gid => $group) {
    $user_groups[$gid] = check_plain($group->title);
  }

  // Check the user's groups for creation rights and add them to $user_groups.
  foreach (group_load_by_member($user->uid) as $group) {
    $has_access = $group->userHasPermission($user->uid, "create $node_type");
    $has_access = $has_access || $group->userHasPermission($user->uid, 'administer group');

    if ($has_access) {
      $user_groups[$group->gid] = check_plain($group->title);
    }
  }

  $form['group_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Group settings'),
    '#access' => !empty($user_groups),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#group' => 'additional_settings',
    '#attributes' => array(
      'class' => array('node-form-group-information'),
    ),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'group') . '/misc/group.js'),
    ),
    '#tree' => TRUE,
    '#weight' => -50,
  );

  $form['group_settings']['group'] = array(
    '#type' => 'select',
    '#title' => t('Select a group to attach this node to.'),
    '#description' => t("By selecting a group, the node will inherit the group's access control."),
    '#options' => $user_groups,
    '#empty_option' => t('- No group -'),
    '#empty_value' => 0,
    '#default_value' => $form['#node']->group,
    // If the user can't create this node outside of a group, he is not allowed
    // to move it to the sitewide scope either.
    '#required' => !user_access("create $node_type"),
  );
}

/**
 * Implements hook_node_submit().
 *
 * @see group_form_node_form_alter()
 */
function group_node_submit($node, $form, $form_state) {
  // Decompose the selected menu parent option into 'menu_name' and 'plid', if
  // the form used the default parent selection widget.
  if (!empty($form_state['values']['group_settings']['group'])) {
    $node->group = $form_state['values']['group_settings']['group'];
  }
}
