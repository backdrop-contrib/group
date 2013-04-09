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

/**
 * Retrieves the group id from an autocomplete result.
 *
 * @param string $result
 *   The result returned by a group autocomplete field.
 *
 * @return int|false
 *   The group id (gid) of the result or FALSE on failure.
 *
 * @see group_autocomplete()
 * @see group_autocomplete_by_type()
 */
function group_autocomplete_result_gid($result) {
  if (preg_match('/(\d+)\)$/', $result, $matches)) {
    return $matches[1];
  }

  return FALSE;
}
