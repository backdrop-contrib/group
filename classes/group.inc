<?php
/**
 * @file
 * Defines the Entity API class for groups.
 */

/**
 * Main class for groups.
 */
class Group extends Entity {

  /**
   * The id of the group.
   *
   * @var integer
   */
  public $gid;

  /**
   * The type of the group.
   *
   * @var string
   */
  public $type;

  /**
   * The title of the group.
   *
   * @var string
   */
  public $title;

  /**
   * Class constructor.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, 'group');
  }

  /**
   * Specifies the default uri, which is picked up by uri() by default.
   */
  protected function defaultURI() {
    //@todo Add type to the path.
    return array('path' => 'group/' . $this->identifier());
  }

  /**
   * Retrieves all possible roles for this group.
   *
   * @return array
   *   A list of group_role entities.
   *
   * @see GroupType::getRoles()
   */
  public function getRoles() {
    return group_type_load($this->type)->getRoles();
  }

  /**
   * Get all members of a group.
   *
   * @return array
   *   An array of arrays for every possible role name, each containing
   *   the uids of the users having that role for this group.
   */
  public function getMembers() {
    $query = db_select('group_user', 'gu');
    $query->condition('gu.gid', $this->gid);
    $query->addField('gu', 'uid');
    $query->addField('gu', 'role');

    // Prepopulate $members with empty arrays for every role name.
    $members = array_fill_keys(array_keys($this->getRoles()), array());

    foreach ($query->execute()->fetchAllKeyed() as $uid => $role) {
      $members[$role][$uid] = $uid;
    }

    return $members;
  }

  /**
   * Get all members of a group, including subgroup members.
   *
   * @return array
   *   An array of arrays for every possible role name, each containing
   *   the uids of the users having that role for this group or any
   *   of its subgroups.
   */
  public function getMembersRecursive() {
    // Start of by gathering all members of this group.
    $members = $this->getMembers();

    // Then get a list of direct child groups.
    $subgroups = $this->getEntitiesOfType('group');

    // Gather all members of each child group and add them to
    // the members of this group.
    foreach ($subgroups as $group) {
      foreach ($group->getMembersRecursive() as $role => $uids) {
        // Skip empty rids.
        if (empty($uids)) {
          continue;
        }

        // Prevent PHP notices when this role wasn't set yet.
        if (!isset($members[$role])) {
          $members[$role] = array();
        }

        // Return only unique ids for each role.
        $members[$role] += array_combine($uids, $uids);
      }
    }

    return $members;
  }

  /**
   * Get all child entities.
   *
   * @param bool $flat
   *   Whether or not to flatten the result array.
   *
   * @return array
   *   A list of all child entities, keyed by type and bundle. When $flat
   *   is set to TRUE, the return value is one big array of entities.
   *
   * @todo Statically cache the result.
   */
  public function getEntities($flat = FALSE) {
    $entities = array(
      'flat' => array(),
      'full' => array(),
    );

    $query = db_select('group_entity', 'ge');
    $query->condition('ge.gid', $this->gid);
    $query->addField('ge', 'entity_type');
    $query->addField('ge', 'entity_id');
    $query->addField('ge', 'bundle');

    // Divide the retrieved entity ids by type and bundle.
    foreach ($query->execute() as $record) {
      $entities['full'][$record->entity_type][$record->bundle][] = $record->entity_id;
    }

    // Actually load the retrieved entities.
    foreach ($entities['full'] as $type => $bundles) {
      foreach ($bundles as $bundle => $ids) {
        // We run the entity_load() result through array_values() to prevent
        // people from making the false assumption that we always return the
        // entity ids as array keys.
        $entities['full'][$type][$bundle] = array_values(entity_load($type, $ids));
        $entities['flat'] = array_merge($entities['flat'], $entities['full'][$type][$bundle]);
      }
    }

    return $flat
      ? $entities['flat']
      : $entities['full'];
  }

  /**
   * Get all child entities of a certain type.
   *
   * @param string $entity_type
   *   The type of child entities.
   * @param string $bundle
   *   (optional) The bundle of the entity type.
   *
   * @return array
   *   A flat list of child entities of a certain
   *   type and optionally bundle.
   */
  public function getEntitiesOfType($entity_type, $bundle = NULL) {
    // We can reuse the cached result of getEntities().
    $entities = $this->getEntities();

    // Fetch the child entities of the requested type or
    // initialize an empty array when none were found.
    $entities = isset($entities[$entity_type])
      ? $entities[$entity_type]
      : array();

    if (isset($bundle)) {
      // Fetch the child entities of the requested bundle or
      // initialize an empty array when none were found.
      $entities = isset($entities[$bundle])
        ? $entities[$bundle]
        : array();
    }

    // To maintain a consistent return value, we flatten the
    // entity's bundle arrays if no bundle was specified.
    else {
      $flattened = array();

      foreach ($entities as $bundle => $ids) {
        $flattened = array_merge($flattened, $ids);
      }

      $entities = $flattened;
    }

    return $entities;
  }

  /**
   * Check if a given has a certain role for this group.
   *
   * @param int $uid
   *   The uid of the given user.
   * @param string $role
   *   The machine name of the group role.
   *
   * @return bool
   *   Whether the user has the specified role.
   */
  public function userHasRole($uid, $role) {
    $query = db_select('group_user', 'gu');
    $query->condition('gu.gid', $this->gid);
    $query->condition('gu.uid', $uid);
    $query->condition('gu.role', $role);

    return !!$query->countQuery()->execute()->fetchfield();
  }

  /**
   * Add a user to the group with a specific role.
   *
   * @param int $user
   *   The id of the user.
   * @param string $role
   *   The machine name of the group role.
   *
   * @todo: Potentially refresh caches.
   * @todo: Verify the role is valid for this group type.
   */
  public function addMember($uid, $role) {
    $query = db_insert('group_user');
    $query->fields(
      array(
        'gid' => $this->gid,
        'uid' => $uid,
        'role' => $role,
      )
    );

    $query->execute();
  }

  /**
   * Remove a user from a group.
   *
   * @param int $uid
   *   The id of the user.
   *
   * @todo: Potentially refresh caches.
   */
  public function removeMember($uid) {
    $query = db_delete('group_user');
    $query->condition('gid', $this->gid);
    $query->condition('uid', $uid);
    $query->execute();
  }

  /**
   * Add this group as a parent of an entity.
   *
   * @param int $entity_id
   *   The id of the entity.
   * @param string $entity_type
   *   The type of the entity.
   * @param string $bundle
   *   The bundle of the entity.
   */
  public function addAsParent($entity_id, $entity_type, $bundle) {
    $query = db_insert('group_entity');
    $query->fields(
      array(
        'gid' => $this->gid,
        'entity_id' => $entity_id,
        'entity_type' => $entity_type,
        'bundle' => $bundle,
      )
    );

    $query->execute();
  }
}
