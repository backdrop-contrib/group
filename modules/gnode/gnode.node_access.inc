<?php

/**
 * @file
 * Hooks and functions used in compliance mode.
 */

// We need a grant ID for match-all access records.
define('GNODE_MASTER_GRANT_ID', 1986);

/**
 * Implements hook_node_grants().
 *
 * @see gnode_node_access_records().
 */
function gnode_node_grants($account, $op) {
  // Provide the master grant for users who can bypass group access.
  if (user_access('bypass group access')) {
    return array('gnode_bypass' => array(GNODE_MASTER_GRANT_ID));
  }

  // Initialize a grant array for members and one for anonymous/outsider users.
  $grants_m = $grants_ao = array();

  // Gather the machine names of all node types.
  $node_types = array_keys(node_type_get_types());

  // If the user could not bypass group access, we need to check their access
  // for every single group. Because loading every group would incur a massive
  // performance hit, we only load those groups the user is a member of.
  foreach (GroupMembership::getByActiveStatus($account->uid) as $group_membership) {
    // Add the groups the user is a member of to use later on.
    $member_gids[] = $gid = $group_membership->gid;

    // See whether the user has all permissions for this group.
    $group_administrator = $group_membership->hasPermission('administer group');

    foreach ($node_types as $node_type) {
      switch ($op) {
        case 'view':
          // Allow the user to view published content of this type.
          if ($group_administrator || $group_membership->hasPermission("view $node_type node")) {
            $grants_m["gnode:$node_type"][] = $gid;
          }
          // Allow the user to view unpublished content of this type.
          if ($group_administrator || $group_membership->hasPermission("view unpublished $node_type node")) {
            $grants_m["gnode_unpublished:$node_type"][] = $gid;
          }
          break;

        case 'update':
        case 'delete':
          // If you can act on any node, there's no need for the author grant.
          if ($group_administrator || $group_membership->hasPermission("$op any $node_type node")) {
            $grants_m["gnode:$node_type"][] = $gid;
          }
          elseif ($group_membership->hasPermission("$op own $node_type node")) {
            $grants_m["gnode_author:{$account->uid}:$node_type"][] = $gid;
          }
          break;
      }
    }
  }

  // All other groups have the benefit of sharing the same permission set among
  // all anonymous or authenticated users per group type. We can therefore know
  // the user's permissions for all groups of the same type they aren't part of.
  $query = db_select('groups', 'g')->fields('g', array('gid', 'type'));
  if (!empty($member_gids)) {
    $query->condition('gid', $member_gids, 'NOT IN');
  }
  foreach ($query->execute() as $record) {
    $gids_per_group_type[$record->type][] = $record->gid;
  }

  foreach (group_types() as $name => $group_type) {
    // If we could not retrieve any group IDs, skip to the next group type.
    if (empty($gids_per_group_type[$name])) {
      continue;
    }

    // Get the IDs of all the groups the user is not part of for the group type.
    $gids = $gids_per_group_type[$name];

    // Grab the anonymous or outsider permissions for the group type depending
    // on the user's account status (anonymous or authenticated).
    $permissions = $account->uid == 0
      ? $group_type->anonymous_permissions
      : $group_type->outsider_permissions;

    // See whether the user has all permissions for this group type.
    $group_administrator = in_array('administer group', $permissions, TRUE);

    foreach ($node_types as $node_type) {
      switch ($op) {
        case 'view':
          // Allow the user to view published content of this type.
          if ($group_administrator || in_array("view $node_type node", $permissions, TRUE)) {
            $grants_ao["gnode:$node_type"][] = $gids;
          }
          // Allow the user to view unpublished content of this type.
          if ($group_administrator || in_array("view unpublished $node_type node", $permissions, TRUE)) {
            $grants_ao["gnode_unpublished:$node_type"][] = $gids;
          }
          break;

        case 'update':
        case 'delete':
          // If you can act on any node, there's no need for the author grant.
          if ($group_administrator || in_array("$op any $node_type node", $permissions, TRUE)) {
            $grants_ao["gnode:$node_type"][] = $gids;
          }
          elseif (in_array("$op own $node_type node", $permissions, TRUE)) {
            $grants_ao["gnode_author:{$account->uid}:$node_type"][] = $gids;
          }
          break;
      }
    }
  }

  // The code above populated the anonymous/outsider grants by adding the group
  // IDs per group type. We need to combine this into one big list of group IDs
  // per entry in the $grants_ao array.
  foreach ($grants_ao as $key => $grants_per_group_type) {
    $grants_ao[$key] = array_reduce($grants_per_group_type, 'array_merge', array());
  }

  // Recursively merge the member grants with the anonymous/outsider grants.
  return array_merge_recursive($grants_m, $grants_ao);
}

/**
 * Implements hook_node_access_records().
 *
 * In compliance mode defines the following realms:
 * - 'gnode:NODE_TYPE': Grants view and update or delete any access to nodes.
 * - 'gnode_unpublished:NODE_TYPE': Grants view access to unpublished nodes.
 * - 'gnode_author:UID:NODE_TYPE': Grants update or delete access to authors.
 * - 'gnode_bypass': Given to anyone with the 'bypass group access' permission.
 *
 * In safe mode there is a single realm:
 * - 'gnode:safemode': Grants view access, however this is never given to any
 *   user as we hand access control over to gnode_query_node_access_alter().
 *
 * @see gnode_node_grants().
 */
function gnode_node_access_records($node) {
  $grants = array();

  // If the node isn't part of a group, we do not set any access records for it.
  // This allows the node module to take over the responsibility for access by
  // setting the all access record in node_access_acquire_grants().
  if (empty($node->group)) {
    return $grants;
  }

  // We can use the same grant-all base because we will only hand out the grants
  // based on the $op parameter in hook_node_grants().
  $base = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
    'priority' => 0,
  );

  // Add the non-author record for viewing nodes.
  $prefix = $node->status ? 'gnode' : 'gnode_unpublished';
  $grants[] = array(
    'gid' => $node->group,
    'realm' => "$prefix:{$node->type}",
  ) + $base;

  // Add the author record for updating or deleting nodes.
  $grants[] = array(
    'gid' => $node->group,
    'realm' => "gnode_author:{$node->uid}:{$node->type}",
  ) + $base;

  // Add the general access bypass record.
  $grants[] = array(
    'gid' => GNODE_MASTER_GRANT_ID,
    'realm' => 'gnode_bypass',
  ) + $base;

  return $grants;
}

/**
 * Implements hook_node_access().
 */
function gnode_node_access($node, $op, $account) {
  if (is_string($node) && $op == 'create') {
    if (gnode_group_node_create_access($node, $account)) {
      return NODE_ACCESS_ALLOW;
    }
  }

  // Make sure we are dealing with a group node.
  elseif (!empty($node->group)) {
    // If the user can bypass group access, he is allowed access.
    if (user_access('bypass group access', $account)) {
      return NODE_ACCESS_ALLOW;
    }

    $group = group_load($node->group);

    // If the user has admin rights for the given group, he is allowed access.
    if (group_access('administer group', $group, $account)) {
      return NODE_ACCESS_ALLOW;
    }

    switch ($op) {
      case "view":
        if ($node->status && group_access("view $node->type node", $group, $account)) {
          return NODE_ACCESS_ALLOW;
        }
        elseif (!$node->status && group_access("view unpublished $node->type node", $group, $account)) {
          return NODE_ACCESS_ALLOW;
        }
        break;

      case "update":
      case "delete":
        if (group_access("$op any $node->type node", $group, $account)) {
          return NODE_ACCESS_ALLOW;
        }
        elseif ($account->uid == $node->uid && group_access("$op own $node->type node", $group, $account)) {
          return NODE_ACCESS_ALLOW;
        }
        break;
    }

    // If none of the above allowed access, we deny access.
    return NODE_ACCESS_DENY;
  }

  // Ignore access for ungrouped nodes.
  return NODE_ACCESS_IGNORE;
}
