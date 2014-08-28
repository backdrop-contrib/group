<?php
/**
 * @file
 * Contains all Entity API functions for the Subgroup module.
 */

/**
 * Implements hook_entity_info_alter().
 */
function ggroup_entity_info_alter(&$entity_info) {
  // Groups can only have one group parent.
  $entity_info['group']['group entity'] = 'single';
}

/**
 * Implements hook_entity_property_info_alter().
 *
 * Informs Drupal of the changes we made to {group_membership}.
 */
function ggroup_entity_property_info_alter(&$info) {
  $group_membership = &$info['group_membership']['properties'];
  $group_membership['parent_membership'] = array(
    'label' => t('Parent membership'),
    'description' => t('The membership from which this one was inherited.'),
    'type' => 'group_membership',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'parent_mid',
  );
}

/**
 * Implements hook_entity_insert().
 */
function ggroup_entity_insert($entity, $type) {
  // Check and run inheritance when a subgroup is created.
  if ($type == 'group' && !empty($entity->group)) {
    // Load the parent group.
    $parent = group_load($entity->group);

    // Run inheritance for all of the parent group's memberships.
    foreach ($parent->getMembers() as $group_membership) {
      ggroup_run_member_inheritance($group_membership, array($entity->gid));
    }
  }

  // Check and run inheritance when a membership is created.
  elseif ($type == 'group_membership') {
    // Run inheritance for the membership.
    ggroup_run_member_inheritance($entity);
  }
}

/**
 * Implements hook_entity_update().
 */
function ggroup_entity_update($entity, $type) {
  if ($type == 'group_membership') {
    // If an inherited membership is updated, check if it broke inheritance.
    if ($entity->status == 'inherited') {
      // Retrieve the roles the membership should have gotten.
      $inherited_roles = ggroup_get_inherited_roles($entity);

      if ($inherited_roles !== FALSE) {
        // Flag the inherited membership as overridden if anything has changed.
        if (array_diff($entity->roles, $inherited_roles) || array_diff($inherited_roles, $entity->roles)) {
          $entity->status = 'inherited-overridden';
          $entity->save();
        }
      }
    }

    // Rerun inheritance for any updated membership.
    ggroup_run_member_inheritance($entity);
  }
}

/**
 * Implements hook_entity_delete().
 *
 * @todo Parent is deleted before children. Causes fatal EMW errors.
 * @see https://drupal.org/node/1789494
 */
function ggroup_entity_delete($entity, $type) {
  if ($type == 'group_membership') {
    // Check for any memberships that had this one as its parent.
    $children = group_memberships(array('parent_mid' => $entity->mid));

    foreach ($children as $group_membership) {
      // If the inherited membership was 'clean', delete it as well.
      if ($group_membership->status == 'inherited') {
        $group_membership->delete();
      }

      // If it was 'dirty', only retain additions.
      elseif ($group_membership->status == 'inherited-overridden') {
        // Retrieve the roles the membership should have gotten.
        $inherited_roles = ggroup_get_inherited_roles($group_membership);

        if ($inherited_roles !== FALSE) {
          // Compare the roles with what they should have been.
          $additions = array_diff($group_membership->roles, $inherited_roles);

          // Save the roles that were added and break the inheritance.
          if (!empty($additions)) {
            $group_membership->roles = $additions;
            $group_membership->status = 'active';
            $group_membership->parent_mid = NULL;
            $group_membership->save();
          }
        }
      }
    }
  }
}
