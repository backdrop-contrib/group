<?php

/**
 * @file
 * Hooks and functions used in safe mode.
 */

/**
 * Implements hook_query_TAG_alter().
 *
 * @see _gnode_node_access_info()
 */
function gnode_query_node_access_alter(QueryAlterableInterface $query) {
  global $user;

  if (_gnode_get_mode() !== GROUP_NODE_SAFE_MODE) {
    return NULL;
  }

  // Retrieve all query tables.
  $tables = $query->getTables();

  // Because this function requires the {node} table, search for its presence.
  foreach ($tables as $alias => $table_info) {
    if ($table_info['table'] == 'node') {
      $node_alias = $alias;
      break;
    }
  }

  // If we couldn't find the {node} table, we search for a table that has a
  // foreign key linking to it. If we find such a link, we store the table's
  // alias and field name to create a join with.
  if (empty($node_alias)) {
    // First we see if a base table is specified.
    $base_table = $query->getMetaData('base_table');

    // In case it is, we should look for a {node}.nid reference there.
    if (!empty($base_table)) {
      // Retrieve the alias from the list of tables.
      foreach ($tables as $alias => $table_info) {
        if ($table_info['table'] == $base_table) {
          $base_alias = $alias;
          break;
        }
      }

      // Store the {node}.nid reference in $base_field.
      if ($nid_field = _gnode_get_table_nid_reference($base_table)) {
        $base_field = $nid_field;
      }
    }
    // In case it isn't, we loop over all of the query's tables in search for
    // a table that we can link to the node table.
    else {
      // We go over all the tables in the query to find a base table.
      foreach ($tables as $alias => $table_info) {
        if (!($table_info instanceof SelectQueryInterface)) {
          if ($nid_field = _gnode_get_table_nid_reference($table_info['table'])) {
            $base_alias = $alias;
            $base_field = $nid_field;
            break;
          }
        }
      }
    }

    // If we still don't have a table and field to join {node} with, throw an
    // exception.
    if (empty($base_alias) || empty($base_field)) {
      throw new Exception(t('Query tagged for node access but there is no join possible to {node}. Add foreign keys to {node}.nid in schema to fix.'));
    }

    // Finally, join {node} with the query and store its alias.
    $node_alias = $query->join('node', 'n', "$base_alias.$base_field = n.nid");
  }

  // Read the account from the query, if provided.
  if (!$account = $query->getMetaData('account')) {
    $account = $user;
  }

  // Retrieve all node ids of group nodes along with their group id.
  $group_nodes = db_select('group_entity', 'ge');
  $group_nodes->addField('ge', 'gid');
  $group_nodes->addField('ge', 'entity_id');
  $group_nodes->condition('entity_type', 'node');

  // Store them by group id and as one big array of node ids.
  $group_nids = array();
  foreach ($group_nodes->execute() as $record) {
    $group_nids['all'][] = $record->entity_id;
    $group_nids[$record->gid][] = $record->entity_id;
  }

  // If there are no group nodes, there is nothing for us left to do.
  if (empty($group_nids['all'])) {
    return;
  }

  // Get a reference to the existing query conditions.
  $conditions = &$query->conditions();

  // Check for a node access query and store its condition index.
  $node_access_index = FALSE;
  foreach ($conditions as $index => $condition) {
    if ($index == '#conjunction') {
      continue;
    }

    if ($condition['value'] instanceof SelectQuery) {
      $tables = $condition['value']->getTables();

      if (isset($tables['na']['table']) && $tables['na']['table'] == 'node_access') {
        $node_access_index = $index;
        break;
      }
    }
  }

  // If the user can bypass group access, most of this function need not run.
  if (user_access('bypass group access', $account)) {
    // If there is no node access query and the user can bypass group access, we
    // do not need to alter the query. So exit the function early.
    if ($node_access_index === FALSE) {
      return;
    }

    // If there is a node access query, we only run it on non-group nodes,
    // meaning any group node passes the access checks.
    $node_access = db_or();

    // Store the original node access condition.
    $node_access_conditions = &$node_access->conditions();
    $node_access_conditions[0] = $conditions[$node_access_index];

    // Remove the node access condition from the top level conditions.
    unset($conditions[$node_access_index]);

    // Add our 'or is a group node' condition to node access checks.
    $node_access->condition("$node_alias.nid", $group_nids['all'], 'IN');

    // Finally, add our combined access check to the query.
    $query->condition($node_access);
  }

  // If the user can't bypass group access, we'll need to determine which
  // group nodes he has access to for the requested operation.
  else {
    // Read the operation from the query, if provided.
    if (!$op = $query->getMetaData('op')) {
      $op = 'view';
    }

    // Retrieve the group access condition info for this user and operation.
    $access_info = _gnode_node_access_info($account, $op);

    // This will be used to store conditions for group nodes.
    $group_access = db_or();

    // Retrieve all node ids of group nodes along with their group id.
    $group_nodes->addField('ge', 'gid');

    if (!empty($access_info['full access'])) {
      $query_nids = array();
      foreach (array_intersect_key($group_nids, $access_info['full access']) as $nids) {
        $query_nids = array_merge($query_nids, $nids);
      }

      if ($query_nids) {
        $group_access->condition("$node_alias.nid", $query_nids, 'IN');
      }
    }

    foreach (node_type_get_types() as $node_type => $type_info) {
      if (!empty($access_info['type access'][$node_type])) {
        $query_nids = array();
        foreach (array_intersect_key($group_nids, $access_info['type access'][$node_type]) as $nids) {
          $query_nids = array_merge($query_nids, $nids);
        }

        if ($query_nids) {
          $group_access->condition(db_and()
            ->condition("$node_alias.type", $node_type)
            ->condition("$node_alias.nid", $query_nids, 'IN')
          );
        }
      }

      if (!empty($access_info['user access'][$node_type])) {
        $query_nids = array();
        foreach (array_intersect_key($group_nids, $access_info['user access'][$node_type]) as $nids) {
          $query_nids = array_merge($query_nids, $nids);
        }

        if ($query_nids) {
          $group_access->condition(db_and()
            ->condition("$node_alias.uid", $account->uid)
            ->condition("$node_alias.type", $node_type)
            ->condition("$node_alias.nid", $query_nids, 'IN')
          );
        }
      }
    }

    // If there is no node access query and the user cannot bypass group
    // access, we need to add a condition checking whether the node is a
    // group node. In case it is, it needs to match group access conditions.
    if ($node_access_index === FALSE) {
      $group_access->condition("$node_alias.nid", $group_nids['all'], 'NOT IN');
      $query->condition($group_access);
    }

    // If there is a node access query, we need to add it to non-group nodes
    // only. Group nodes should still bypass node access and check only for
    // group access conditions.
    else {
      // We are about to put $group_access on the same level as $node_access.
      // However, in case $group_access is empty, this will cause a database
      // error to occur. To prevent this, we do nothing in said case. We take
      // the default '#conjunction' entry into account when checking.
      if (count($group_access->conditions()) < 2) {
        return;
      }

      $node_access = db_and();

      // Store the original node access condition.
      $node_access_conditions = &$node_access->conditions();
      $node_access_conditions[0] = $conditions[$node_access_index];

      // Remove the node access condition from the top level conditions.
      unset($conditions[$node_access_index]);

      // Add our 'not a group node' condition to node access checks.
      $node_access->condition("$node_alias.nid", $group_nids['all'], 'NOT IN');

      // Finally, add our combined access check to the query.
      $query->condition(db_or()
        ->condition($node_access)
        ->condition($group_access)
      );
    }
  }
}

/**
 * Helper function for gnode_query_node_access_alter().
 *
 * Provides access information to format query conditions with.
 *
 * @param object $account
 *   The user account to retrieve access info for.
 * @param string $op
 *   The node operation to retrieve access info for.
 *
 * @return array
 *   An associative array containing group ids the account has access to,
 *   subdivided in three categories which  are keyed as follows:
 *   - full access: For these group ids, the user has access to every node.
 *   - type access: Keyed by node type. For these group ids, the user has
 *     access to every node of the node type indicated by the array key.
 *   - user access: Keyed by node type. For these group ids, the user has
 *     access to every node of the node type indicated by the array key of
 *     which he is also the author.
 *
 * @see gnode_query_node_access_alter()
 */
function _gnode_node_access_info($account, $op) {
  // Initialize the array that will hold group ids.
  $return = array();

  // Gather the machine names of all node types.
  $node_types = array_keys(node_type_get_types());

  // Determine whether the account could be 'anonymous' or 'outsider'.
  $account_exists = (bool) $account->uid;

  // Check access for groups the user is not a member of.
  foreach (group_types() as $type => $group_type) {
    if ($non_member_gids = group_non_member_gids($account->uid, $type)) {
      // Retrieve the permissions to check for rights.
      $permissions = $account_exists ? $group_type->outsider_permissions : $group_type->anonymous_permissions;

      // If you can administer the group, you have full access to every node
      // in that group. Add these group ids to 'full access'.
      if (in_array('administer group', $permissions)) {
        foreach ($non_member_gids as $gid) {
          $return['full access'][$gid] = $gid;
        }
        continue;
      }

      foreach ($node_types as $node_type) {
        // Use booleans for readability's sake.
        $type_access = FALSE;
        $user_access = FALSE;

        switch ($op) {
          case 'view':
            $type_access = in_array("view $node_type node", $permissions);
            break;

          case 'update':
          case 'delete':
            $type_access = in_array("$op any $node_type node", $permissions);
            $user_access = in_array("$op own $node_type node", $permissions);
            break;
        }

        // If the user has access to this node type, we add all nodes of that
        // node type (within the given group) to the list of accessible nodes.
        if ($type_access) {
          foreach ($non_member_gids as $gid) {
            $return['type access'][$node_type][$gid] = $gid;
          }
        }

        // Otherwise, if the user has author access to this node type, we add
        // all nodes of that node type (within the given group) where the user
        // is the author to the list of accessible nodes.
        elseif ($user_access) {
          foreach ($non_member_gids as $gid) {
            $return['user access'][$node_type][$gid] = $gid;
          }
        }
      }
    }
  }

  // Check access for groups the user is a member of.
  foreach (group_load_by_member($account->uid) as $group) {
    // Store gid to keep code below concise.
    $gid = $group->gid;

    // If you can administer the group, you have full access to every node
    // in that group. Add these group ids to 'full access'.
    if (group_access('administer group', $group, $account)) {
      $return['full access'][$gid] = $gid;
      continue;
    }

    foreach ($node_types as $node_type) {
      // Use booleans for readability's sake.
      $type_access = FALSE;
      $user_access = FALSE;

      switch ($op) {
        case 'view':
          $type_access = group_access("view $node_type node", $group, $account);
          break;

        case 'update':
        case 'delete':
          $type_access = group_access("$op any $node_type node", $group, $account);
          $user_access = group_access("$op own $node_type node", $group, $account);
          break;
      }

      // If the user has access to this node type, we add all nodes of that
      // node type (within the given group) to the list of accessible nodes.
      if ($type_access) {
        $return['type access'][$node_type][$gid] = $gid;
      }

      // If the user has author access to this node type, we add all nodes of
      // that node type (within the given group) where the user is the author
      // to the list of accessible nodes.
      if ($user_access) {
        $return['user access'][$node_type][$gid] = $gid;
      }
    }
  }

  return $return;
}

/**
 * Retrieves the name of the field that is used as a foreign key to {node}.nid.
 *
 * @param string $table
 *   The name of the table.
 *
 * @return string|false
 *   The name of the field or FALSE if it could not be found.
 */
function _gnode_get_table_nid_reference($table) {
  // Retrieve the schema info.
  $schema = drupal_get_schema($table);

  // Loop over the foreign key definitions in search for a field that
  // references {node}.nid. In case we find one, we return the field's name.
  if (isset($schema['foreign keys'])) {
    foreach ($schema['foreign keys'] as $relation) {
      if ($relation['table'] === 'node') {
        // Loop over the foreign key entries in search for a nid reference.
        foreach ($relation['columns'] as $local => $foreign) {
          if ($foreign === 'nid') {
            return $local;
          }
        }
      }
    }
  }

  return FALSE;
}
