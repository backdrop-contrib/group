<?php

/**
 * @file
 * Hooks and functions used in compliance mode.
 */

/**
 * Implements hook_node_grants().
 *
 * @see gnode_node_access_records().
 */
function gnode_node_grants($account, $op) {
  if (_gnode_get_mode() !== GROUP_NODE_COMPLIANCE_MODE) {
    return array();
  }

  $grants = array();

  // Gather the machine names of all node types.
  $node_types = array_keys(node_type_get_types());

  // Provider grants for users with bypass group access.
  if (user_access('bypass group access', $account)) {
    if ($all_gids = group_get_all_group_ids()) {
      foreach ($node_types as $node_type) {
        foreach ($all_gids as $gid) {
          $grants["gnode:{$node_type}"][] = $gid;
        }
      }
    }
    return $grants;
  }

  // Provide grants for anonymous user.
  if (empty($account->uid)) {
    foreach (group_types() as $type => $group_type) {
      if ($all_gids = group_get_all_group_ids($type)) {
        foreach ($node_types as $node_type) {
          // Shorten variable name for readability's sake.
          $permissions = $group_type->anonymous_permissions;
          $group_administrator = in_array("administer group", $permissions);

          // Allow the user to view content of this type.
          if ($op === 'view' && ($group_administrator || in_array("view $node_type node", $permissions))) {
            foreach ($all_gids as $gid) {
              $grants["gnode:$node_type"][] = $gid;
            }
          }

          // Allow the user to update content of this type.
          elseif ($op === 'update' && ($group_administrator || in_array("update any $node_type node", $permissions))) {
            foreach ($all_gids as $gid) {
              $grants["gnode:$node_type"][] = $gid;
            }
          }

          // Allow the user to delete content of this type.
          elseif ($op === 'delete' && ($group_administrator || in_array("delete any $node_type node", $permissions))) {
            foreach ($all_gids as $gid) {
              $grants["gnode:$node_type"][] = $gid;
            }
          }

          if (!$group_administrator) {
            // Allow the user to update their own content of this type.
            if ($op === 'update' && in_array("update own $node_type node", $permissions)) {
              foreach ($all_gids as $gid) {
                $grants["gnode_author:$gid:$node_type"][] = $account->uid;
              }
            }

            // Allow the user to delete their own content of this type.
            elseif ($op === 'delete' && in_array("delete own $node_type node", $permissions)) {
              foreach ($all_gids as $gid) {
                $grants["gnode_author:$gid:$node_type"][] = $account->uid;
              }
            }
          }
        }
      }
    }

    return $grants;
  }

  // Provide grants for groups the user is an outsider to.
  foreach (group_types() as $type => $group_type) {
     if ($outsider_gids = group_non_member_gids($account->uid, $type)) {
      foreach ($node_types as $node_type) {
        // Shorten variable name for readability's sake.
        $permissions = $group_type->outsider_permissions;
        $group_administrator = in_array("administer group", $permissions);

        // Allow the user to view content of this type.
        if ($op === 'view' && ($group_administrator || in_array("view $node_type node", $permissions))) {
          foreach ($outsider_gids as $gid) {
            $grants["gnode:$node_type"][] = $gid;
          }
        }

        // Allow the user to update content of this type.
        elseif ($op === 'update' && ($group_administrator || in_array("update any $node_type node", $permissions))) {
          foreach ($outsider_gids as $gid) {
            $grants["gnode:$node_type"][] = $gid;
          }
        }

        // Allow the user to delete content of this type.
        elseif ($op === 'delete' && ($group_administrator || in_array("delete any $node_type node", $permissions))) {
          foreach ($outsider_gids as $gid) {
            $grants["gnode:$node_type"][] = $gid;
          }
        }

        if (!$group_administrator) {
          // Allow the user to update their own content of this type.
          if ($op === 'update' && in_array("update own $node_type node", $permissions)) {
            foreach ($outsider_gids as $gid) {
              $grants["gnode_author:$gid:$node_type"][] = $account->uid;
            }
          }

          // Allow the user to delete their own content of this type.
          elseif ($op === 'delete' && in_array("delete own $node_type node", $permissions)) {
            foreach ($outsider_gids as $gid) {
              $grants["gnode_author:$gid:$node_type"][] = $account->uid;
            }
          }
        }
      }
    }
  }

  // Provide grants for groups the user is a member of.
  foreach (group_load_by_member($account->uid) as $group) {
    $group_administrator = group_access("administer group", $group, $account);

    foreach ($node_types as $node_type) {
      // Allow the user to view content of this type.
      if ($op === 'view' && ($group_administrator || group_access("view $node_type node", $group, $account))) {
        $grants["gnode:$node_type"][] = $group->gid;
      }

      // Allow the user to update content of this type.
      elseif ($op === 'update' && ($group_administrator || group_access("update any $node_type node", $group, $account))) {
        $grants["gnode:$node_type"][] = $group->gid;
      }

      // Allow the user to delete content of this type.
      elseif ($op === 'delete' && ($group_administrator || group_access("delete any $node_type node", $group, $account))) {
        $grants["gnode:$node_type"][] = $group->gid;
      }

      if (!$group_administrator) {
        // Allow the user to update their own content of this type.
        if ($op === 'update' && group_access("update own $node_type node", $group, $account)) {
          $grants["gnode_author:{$group->gid}:{$node_type}"][] = $account->uid;
        }

        // Allow the user to delete their own content of this type.
        elseif ($op === 'delete' && group_access("delete own $node_type node", $group, $account)) {
          $grants["gnode_author:{$group->gid}:{$node_type}"][] = $account->uid;
        }
      }
    }
  }

  return $grants;
}

/**
 * Implements hook_node_access_records().
 *
 * In compliance mode defines the following realms:
 * - 'gnode:NODE_TYPE': Grants access for this node type.
 * - 'gnode_author:GID:NODE_TYPE': Grants access to node author for this node
 *   type.
 *
 * In safe mode there is a single realm:
 * - 'gnode:safemode': Grants view access, however this is never given to any
 *   user as we hand access control over to gnode_query_node_access_alter().
 *
 * @see gnode_node_grants().
 */
function gnode_node_access_records($node) {
  $grants = array();

  // If the node isn't part of a group or is unpublished, we do not set any
  // access records for it. This leads to the node module taking over the
  // responsibility for both cases.
  if (empty($node->group) || !$node->status) {
    return $grants;
  }
  
  if (_gnode_get_mode() !== GROUP_NODE_COMPLIANCE_MODE) {
    // Add a bogus grant to make sure the node module doesn't add a default
    // access record that will allow anyone to view the node as long as it's
    // published.
    $grants[] = array(
      'gid' => '9000',
      'grant_view' => 1,
      'grant_update' => 0,
      'grant_delete' => 0,
      'priority' => 0,
      'realm' => "gnode:safemode",
    );
  }
  else {
    // Add a grant for the view/update/delete any permissions.
    $grants[] = array(
      'gid' => $node->group,
      'grant_view' => 1,
      'grant_update' => 1,
      'grant_delete' => 1,
      'priority' => 0,
      'realm' => "gnode:{$node->type}",
    );

    // Add a grant for the update/delete own permissions.
    $grants[] = array(
      'gid' => $node->uid,
      'grant_view' => 1,
      'grant_update' => 1,
      'grant_delete' => 1,
      'priority' => 0,
      'realm' => "gnode_author:{$node->group}:{$node->type}",
    );
  }

  return $grants;
}
