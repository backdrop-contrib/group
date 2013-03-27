<?php
/**
 * @file
 * Defines the Entity API class for group types.
 */

/**
 * Main class for group type entities.
 */
class GroupType extends Entity {

  /**
   * The internal id of the group type.
   *
   * @var int
   */
  public $tid;

  /**
   * The machine-readable name of the group type.
   *
   * @var string
   */
  public $name;

  /**
   * The human-readable name of the group type.
   *
   * @var string
   */
  public $label;

  /**
   * The internal weight of the group type.
   *
   * @var int
   */
  public $weight;

  /**
   * The entity status of the group type.
   *
   * @var int
   */
  public $status;

  /**
   * The module that provided the group type.
   *
   * @var string
   */
  public $module;

  /**
   * Class constructor.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, 'group_type');
  }

  /**
   * Returns whether the group type is locked.
   */
  public function isLocked() {
    return isset($this->status)
      && empty($this->is_new)
      && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }

  /**
   * Retrieves all possible roles for this group type.
   *
   * @return array
   *   A list of group_role entities, keyed by their machine name.
   */
  public function getRoles() {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'group_role');
    $query->entityCondition('bundle', 'group_role');
    $query->propertyCondition('type', $this->name);
    $result = $query->execute();

    $roles = array();
    if (!empty($result['group_role'])) {
      foreach ($result['group_role'] as $group_role) {
        $roles[$group_role->name] = $group_role;
      }
    }

    return $roles;
  }
}
