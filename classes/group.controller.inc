<?php
/**
 * @file
 * Defines the Entity API CRUD class for groups.
 */

/**
 * Controller for group entities.
 */
class GroupController extends EntityAPIController {

  /**
   * Delete a group.
   *
   * As part of the cleanup process, we delete all child entities. This will
   * in turn trigger group_entity_delete() and remove the deleted entities'
   * records as group entity for this group.
   *
   * We do this to keep the generic entity deletion logic in one place. This
   * could be a big performance hit for groups with a lot of content, so we
   * need to carefully monitor how this fares.
   *
   * @see group_entity_delete()
   * @see EntityAPIController::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $mids = array();

    foreach (group_load_multiple($ids) as $group) {
      foreach ($group->getEntities() as $entity_type => $bundles) {
        // It is safe to assume that 'group entity' is either 'single' or
        // 'multiple' because the entities could not belong to a group if
        // 'group entity' were set to FALSE.
        $entity_info = entity_get_info($entity_type);
        $single = $entity_info['group entity'] == 'single';

        foreach ($bundles as $bundle => $entities) {
          // Keep track of the entities we want to delete along with the group.
          $delete_ids = array();

          // If the entities can only belong to one group, we are safe to just
          // delete all of them along with the group.
          if ($single) {
            $delete_ids = array_keys($entities);
          }
          // If the entity can belong to multiple groups, we need to check
          // on a per-entity basis whether this is the last group that had
          // the entity as its child.
          else {
            foreach ($entities as $entity_id => $entity) {
              if (count($entity->group) == 1) {
                $delete_ids[] = $entity_id;
              }
            }
          }

          entity_delete_multiple($entity_type, $delete_ids);
        }
      }

      // Gather the membership ids to delete.
      $memberships = group_membership_load_by_group($group->gid);
      $mids = array_merge($mids, array_keys($memberships));
    }

    // Delete group memberships.
    group_membership_delete_multiple($mids);

    parent::delete($ids, $transaction);
  }

  /**
   * Save a group.
   *
   * Add the user who creates a group as the first member.
   *
   * @see EntityAPIController::save()
   */
  public function save($group, DatabaseTransaction $transaction = NULL) {
    // Only act upon saving a new group.
    if (empty($group->is_new)) {
      return parent::save($group, $transaction);
    }

    // Hold off on returning the parent return value.
    $return = parent::save($group, $transaction);

    // Retrieve the default roles for this group type.
    $defaults = variable_get('group_default_roles');
    $roles = isset($defaults[$group->type])
      ? array_filter($defaults[$group->type])
      : array();

    // Add the current user as the first member.
    global $user;
    $group->addMember($user->uid, array('roles' => $roles));

    return $return;
  }

  /**
   * Create a group.
   *
   * We first set up the values that are specific to the group schema
   * but then also run the EntityAPIController counterpart.
   *
   * @param $values
   *   An array of values to set, keyed by property name.
   *
   * @return Group
   *   A new Group instance.
   */
  public function create(array $values = array()) {
    // Provide defaults that are needed in group_form().
    $values += array(
      'type' => '',
      'title' => '',
    );

    return parent::create($values);
  }
}
