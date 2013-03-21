<?php
/**
 * @file
 * Group related helper functions.
 */

/**
 * Get the parent groups for an entity.
 *
 * @param string $entity_type
 *   The type of entity to retrieve the parents for.
 * @param int $entity_id
 *   The id of the entity to retrieve the parents for.
 *
 * @return array
 *   A list of all groups that the entity belongs to.
 */
function group_get_entity_parents($entity_type, $entity_id) {
  $query = db_select('group_entity', 'ge');
  $query->condition('ge.entity_type', $entity_type);
  $query->condition('ge.entity_id', $entity_id);
  $query->addfield('ge', 'gid');

  return group_load_multiple($query->execute()->fetchCol());
}
