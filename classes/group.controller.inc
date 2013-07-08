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
      // Delete child entities, including subgroups.
      foreach ($group->getEntities() as $entity_type => $bundles) {
        foreach ($bundles as $bundle => $entities) {
          entity_delete_multiple($entity_type, array_keys($entities));
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
   * Create a group.
   *
   * We first set up the values that are specific to the group schema
   * but then also run the EntityAPIController counterpart.
   *
   * @param $values
   *   An array of values to set, keyed by property name.
   *
   * @return
   *   A new instance of the group entity type.
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
