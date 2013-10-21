<?php
/**
 * @file
 * Contains all Entity API functions for the Group module.
 */

/**
 * Implements hook_entity_info().
 *
 * Defines the entities this module needs to function.
 */
function group_entity_info() {
  $info['group'] = array(
    'label' => t('Group'),
    'plural label' => t('Groups'),
    'description' => t('Groups people and content with roles and permissions.'),
    'entity class' => 'Group',
    'controller class' => 'GroupController',
    'base table' => 'groups',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'gid',
      'bundle' => 'type',
      'label' => 'title',
    ),
    'access callback' => 'group_entity_access',
    'label callback' => 'entity_class_label',
    'uri callback' => 'entity_class_uri',
    // Bundles are set in group_entity_info_alter().
    'bundles' => array(),
    'bundle keys' => array(
      'bundle' => 'name',
    ),
    'module' => 'group',
    'admin ui' => array(
      'path' => 'admin/group',
      'file' => 'forms/group.inc',
      'controller class' => 'GroupUIController',
    ),
    'group entity' => 'single',
  );

  $info['group_type'] = array(
    'label' => t('Group type'),
    'plural label' => t('Group types'),
    'description' => t('Define different group types.'),
    'entity class' => 'GroupType',
    'controller class' => 'GroupTypeController',
    'base table' => 'group_type',
    'fieldable' => FALSE,
    'bundle of' => 'group',
    'exportable' => TRUE,
    'entity keys' => array(
      'id' => 'tid',
      'name' => 'name',
      'label' => 'label',
    ),
    'access callback' => 'group_type_access',
    'module' => 'group',
    'admin ui' => array(
      'path' => 'admin/group/type',
      'file' => 'admin/group_type.inc',
      'controller class' => 'GroupTypeUIController',
    ),
    'features controller class' => 'GroupTypeFeaturesController',
  );

  $info['group_role'] = array(
    'label' => t('Group role'),
    'plural label' => t('Group roles'),
    'description' => t('Define different group roles.'),
    'entity class' => 'GroupRole',
    'controller class' => 'GroupRoleController',
    'base table' => 'group_role',
    'fieldable' => FALSE,
    'exportable' => TRUE,
    'entity keys' => array(
      'id' => 'rid',
      'name' => 'name',
      'label' => 'label',
    ),
    // Roles are bound to group types.
    'access callback' => 'group_type_access',
    'module' => 'group',
    'admin ui' => array(
      'path' => 'admin/group/role',
      'file' => 'admin/group_role.inc',
      'controller class' => 'GroupRoleUIController',
    ),
    'features controller class' => 'GroupRoleFeaturesController',
  );

  $info['group_membership'] = array(
    'label' => t('Group membership'),
    'plural label' => t('Group memberships'),
    'description' => t('Provides functionality to link Group to User entities.'),
    'entity class' => 'GroupMembership',
    'controller class' => 'GroupMembershipController',
    'base table' => 'group_membership',
    'entity keys' => array(
      'id' => 'mid',
    ),
    'module' => 'group',
  );

  return $info;
}

/**
 * Implements hook_entity_info_alter().
 */
function group_entity_info_alter(&$entity_info) {
  // Informs Drupal that group types are bundles for groups.
  foreach (group_types() as $name => $group_type) {
    $entity_info['group']['bundles'][$name] = array(
      'label' => $group_type->label,
      'admin' => array(
        'path' => 'admin/group/type/manage/%group_type',
        'real path' => 'admin/group/type/manage/' . $name,
        'bundle argument' => 4,
        'access arguments' => array('administer group types'),
      ),
    );
  }

  // Entities default to not being group entities.
  foreach ($entity_info as $entity => $data) {
    $entity_info[$entity] += array(
      'group entity' => FALSE,
    );
  }
}

/**
 * Entity API access callback for groups.
 *
 * @param string $group_type
 *   (optional) The group type to create. Only needs to be specified
 *   when $op is 'create'.
 */
function group_entity_access($op, $group, $account = NULL, $entity_type = NULL, $group_type = NULL) {
  switch ($op) {
    case 'create':
      return !empty($group_type) && user_access("create $group_type group", $account);
    case 'view':
      return group_access('administer group', $group, $account)
        || group_access('view group', $group, $account);
    case 'update':
    case 'delete':
      return group_access('administer group', $group, $account);
  }
}

/**
 * Entity API access callback for group types and roles.
 */
function group_type_access($op, $group_type_or_role, $account = NULL, $entity_type = NULL) {
  return user_access('administer group types', $account);
}

/**
 * Implements hook_entity_property_info().
 */
function group_entity_property_info() {
  $info = array();

  // Properties for Group entities.
  $group = &$info['group']['properties'];
  $group['gid'] = array(
    'label' => t('Group ID'),
    'description' => t('The unique group ID.'),
    'type' => 'integer',
    'schema_field' => 'gid',
  );

  $group['type'] = array(
    'label' => t('Group type name'),
    'description' => t('The name of the group type.'),
    'type' => 'token',
    'setter callback' => 'entity_property_verbatim_set',
    'clear' => array('group_type'),
    'schema_field' => 'type',
  );

  $group['group_type'] = array(
    'label' => t('Group type'),
    'description' => t('The group type.'),
    'type' => 'group_type',
    'getter callback' => 'group_get_properties',
    'setter callback' => 'group_set_properties',
    'computed' => TRUE,
    'clear' => array('type'),
  );

  $group['title'] = array(
    'label' => t('Title'),
    'description' => t('The title of the group.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'schema_field' => 'title',
  );

  // Properties for GroupType entities.
  $group_type = &$info['group_type']['properties'];
  $group_type['tid'] = array(
    'label' => t('Group type ID'),
    'description' => t('The unique group type ID.'),
    'type' => 'integer',
    'schema_field' => 'tid',
  );

  $group_type['name'] = array(
    'label' => t('Machine name'),
    'description' => t('The machine readable name for the type.'),
    'type' => 'token',
    'setter callback' => 'entity_property_verbatim_set',
    'schema_field' => 'name',
  );

  $group_type['label'] = array(
    'label' => t('Label'),
    'description' => t('The human readable name for the type.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'schema_field' => 'label',
  );

  $group_type['outsider_permissions'] = array(
    'label' => t('Outsider permissions'),
    'description' => t('The outsider permissions for the group type.'),
    'type' => 'list<text>',
    'setter callback' => 'entity_property_verbatim_set',
    //'options list' => '',
  );

  $group_type['member_permissions'] = array(
    'label' => t('Member permissions'),
    'description' => t('The member permissions for the group type.'),
    'type' => 'list<text>',
    'setter callback' => 'entity_property_verbatim_set',
    //'options list' => '',
  );

  $group_type['weight'] = array(
    'label' => t('Weight'),
    'description' => t('The weight of the type compared to others.'),
    'type' => 'integer',
    'setter callback' => 'entity_property_verbatim_set',
    'schema_field' => 'weight',
  );

  $group_type['status'] = array(
    'label' => t('Entity status'),
    'description' => t('The entity status of the type.'),
    'type' => 'integer',
    'schema_field' => 'status',
  );

  $group_type['module'] = array(
    'label' => t('Providing module'),
    'description' => t('The module providing the entity.'),
    'type' => 'text',
    'schema_field' => 'module',
  );

  // Properties for GroupRole entities.
  $group_role = &$info['group_role']['properties'];
  $group_role['rid'] = array(
    'label' => t('Group role ID'),
    'description' => t('The unique group role ID.'),
    'type' => 'integer',
    'schema_field' => 'rid',
  );

  $group_role['type'] = array(
    'label' => t('Group type name'),
    'description' => t('The name of the group type the role is coupled to.'),
    'type' => 'token',
    'setter callback' => 'entity_property_verbatim_set',
    'clear' => array('group_type'),
    'schema_field' => 'type',
  );

  $group_role['group_type'] = array(
    'label' => t('Group type'),
    'description' => t('The group type the role is coupled to.'),
    'type' => 'group_type',
    'getter callback' => 'group_get_properties',
    'setter callback' => 'group_set_properties',
    'computed' => TRUE,
    'clear' => array('type'),
  );

  $group_role['name'] = array(
    'label' => t('Machine name'),
    'description' => t('The machine readable name for the role.'),
    'type' => 'token',
    'setter callback' => 'entity_property_verbatim_set',
    'schema_field' => 'name',
  );

  $group_role['label'] = array(
    'label' => t('Label'),
    'description' => t('The human readable name for the role.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'schema_field' => 'label',
  );

  $group_role['global'] = array(
    'label' => t('Global status'),
    'description' => t('Whether the role is global or not.'),
    'type' => 'boolean',
    'setter callback' => 'entity_property_verbatim_set',
    'schema_field' => 'global',
  );

  $group_role['permissions'] = array(
    'label' => t('Permissions'),
    'description' => t('The permissions for the role.'),
    'type' => 'list<text>',
    'setter callback' => 'entity_property_verbatim_set',
    //'options list' => '',
  );

  $group_role['weight'] = array(
    'label' => t('Weight'),
    'description' => t('The weight of the role compared to others.'),
    'type' => 'integer',
    'setter callback' => 'entity_property_verbatim_set',
    'schema_field' => 'weight',
  );

  $group_role['status'] = array(
    'label' => t('Entity status'),
    'description' => t('The entity status of the role.'),
    'type' => 'integer',
    'schema_field' => 'status',
  );

  $group_role['module'] = array(
    'label' => t('Providing module'),
    'description' => t('The module providing the entity.'),
    'type' => 'text',
    'schema_field' => 'module',
  );

  // Properties for GroupMembership entities.
  $group_membership = &$info['group_membership']['properties'];
  $group_membership['mid'] = array(
    'label' => t('Group membership ID'),
    'description' => t('The unique group membership ID.'),
    'type' => 'integer',
    'schema_field' => 'mid',
  );

  $group_membership['gid'] = array(
    'label' => t('Group ID'),
    'description' => t('The ID of the group the membership belongs to.'),
    'type' => 'integer',
    'setter callback' => 'entity_property_verbatim_set',
    'clear' => array('group'),
    'schema field' => 'gid',
  );

  $group_membership['group'] = array(
    'label' => t('Group'),
    'description' => t('The group the membership belongs to.'),
    'type' => 'group',
    'getter callback' => 'group_get_properties',
    'setter callback' => 'group_set_properties',
    'computed' => TRUE,
    'clear' => array('gid'),
  );

  $group_membership['uid'] = array(
    'label' => t('User ID'),
    'description' => t('The ID of the user the membership belongs to.'),
    'type' => 'integer',
    'setter callback' => 'entity_property_verbatim_set',
    'clear' => array('user'),
    'schema_field' => 'uid',
  );

  $group_membership['user'] = array(
    'label' => t('User'),
    'description' => t('The user the membership belongs to.'),
    'type' => 'user',
    'getter callback' => 'group_get_properties',
    'setter callback' => 'group_set_properties',
    'computed' => TRUE,
    'clear' => array('uid'),
  );

  $group_membership['roles'] = array(
    'label' => t('Roles'),
    'description' => t('The group roles for the membership.'),
    'type' => 'list<group_role>',
    //'options list' => '',
  );

  return $info;
}

/**
 * Implements hook_entity_property_info_alter().
 */
function group_entity_property_info_alter(&$info) {
  // Add the 'group' property to every group entity.
  foreach (entity_get_info() as $type => $e_info) {
    if (!empty($e_info['group entity'])) {
      $properties = &$info[$type]['properties'];

      $properties['group'] = array(
        'label' => t('Group'),
        'description' => t('The group(s) the entity belongs to.'),
        'type' => $e_info['group entity'] == 'single' ? 'group' : 'list<group>',
        'getter callback' => 'entity_property_verbatim_get',
        'setter callback' => 'entity_property_verbatim_set',
        'computed' => TRUE,
      );
    }
  }
}

/**
 * Callback for getting only-in-code group properties.
 */
function group_get_properties($data, $options, $name, $type, $info) {
  switch ($name) {
    case 'group':
      return $data->gid;
    case 'group_type':
      return $data->type;
    case 'user':
      return $data->uid;
  }
}

/**
 * Callback for setting only-in-code group properties.
 */
function group_set_properties(&$data, $name, $value, $langcode, $type, $info) {
  switch ($name) {
    case 'group':
      $data->gid = $value;
      break;
    case 'group_type':
      $data->type = $value;
      break;
    case 'user':
      $data->uid = $value;
      break;
  }
}

/**
 * Implements hook_entity_load().
 *
 * Add the 'group' key to entity objects that can belong to a group.
 */
function group_entity_load($entities, $type) {
  $entity_info = entity_get_info($type);

  if (!empty($entity_info['group entity'])) {
    $single = $entity_info['group entity'] == 'single';

    // Load group ids outside the loop for performance reasons.
    $gids = group_get_entities_group_ids($type, array_keys($entities));

    foreach ($entities as $entity_id => $entity) {
      if (!empty($gids[$entity_id])) {
        $entity->group = $single ? reset($gids[$entity_id]) : $gids[$entity_id];
      }
    }
  }
}

/**
 * Implements hook_entity_insert().
 */
function group_entity_insert($entity, $type) {
  $entity_info = entity_get_info($type);

  if (!empty($entity_info['group entity'])) {
    group_entity_save($entity, $type);
  }
}

/**
 * Implements hook_entity_update().
 */
function group_entity_update($entity, $type) {
  $entity_info = entity_get_info($type);

  if (!empty($entity_info['group entity'])) {
    // Retrieve the entity id and bundle.
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($type, $entity);

    // Load the original entity to detect changes.
    $original = entity_load_unchanged($type, $entity_id);

    // Gather the original and updated group ids as arrays.
    $original_gids = (isset($original->group)) ? (array) $original->group : array();
    $updated_gids = (isset($entity->group)) ? (array) $entity->group : array();

    // Remove the entity from groups it no longer belongs to.
    if ($gids = array_diff($original_gids, $updated_gids)) {
      foreach (group_load_multiple($gids) as $group) {
        $group->removeEntity($entity_id, $type);
      };
    }

    // Saving takes care of creating new links.
    group_entity_save($entity, $type);
  }
}

/**
 * Helper for hook_entity_insert() and hook_entity_update().
 */
function group_entity_save($entity, $type) {
  if (!empty($entity->group)) {
    $entity_info = entity_get_info($type);

    // Retrieve the entity id and bundle.
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($type, $entity);

    // Load the original entity to detect changes.
    $original = entity_load_unchanged($type, $entity_id);

    // Gather the original and updated group ids as arrays.
    $original_gids = (isset($original->group)) ? (array) $original->group : array();
    $updated_gids = (isset($entity->group)) ? (array) $entity->group : array();

    // Add the entity to groups that it didn't belong to earlier.
    if ($gids = array_diff($updated_gids, $original_gids)) {
      foreach (group_load_multiple($gids) as $group) {
        $group->addEntity($entity_id, $type, $bundle);
      }
    }
  }
}

/**
 * Implements hook_entity_delete().
 */
function group_entity_delete($entity, $type) {
  $entity_info = entity_get_info($type);

  // Retrieve the entity id and bundle.
  list($entity_id, $revision_id, $bundle) = entity_extract_ids($type, $entity);

  // Delete group memberships.
  if ($type === 'user') {
    $mids = array_keys(group_membership_load_by_user($entity_id));
    group_membership_delete_multiple($mids);
  }

  // Delete group entity mappings.
  if (!empty($entity_info['group entity']) && !empty($entity->group)) {
    foreach (group_load_multiple((array) $entity->group) as $group) {
      $group->removeEntity($entity_id, $type);
    }
  }
}
