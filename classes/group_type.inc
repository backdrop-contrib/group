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
   * Returns whether a group type is locked.
   */
  public function isLocked() {
    return isset($this->status)
      && empty($this->is_new)
      && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }

  /**
   * Retrieve all possible roles for a group type.
   *
   * @return array
   *   A list of GroupRole entities.
   *
   * @see group_roles()
   */
  public function getRoles() {
    $query = db_select('group_role', 'gr');
    $query->condition('type', $this->name);
    $query->addField('gr', 'name');

    return group_roles($query->execute()->fetchCol());
  }
}
