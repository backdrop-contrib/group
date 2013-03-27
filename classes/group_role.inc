<?php
/**
 * @file
 * Defines the Entity API class for group roles.
 */

/**
 * Main class for group role entities.
 */
class GroupRole extends Entity {

  /**
   * The internal id of the group role.
   *
   * @var int
   */
  public $rid;

  /**
   * The group type this role is attached to.
   *
   * @var string
   */
  public $type;

  /**
   * The machine-readable name of the group role.
   *
   * @var string
   */
  public $name;

  /**
   * The human-readable name of the group role.
   *
   * @var string
   */
  public $label;

  /**
   * The internal weight of the group role.
   *
   * @var int
   */
  public $weight;

  /**
   * The entity status of the group role.
   *
   * @var int
   */
  public $status;

  /**
   * The module that provided the group role.
   *
   * @var string
   */
  public $module;

  /**
   * Class constructor.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, 'group_role');
  }

  /**
   * Returns whether the group role is locked.
   */
  public function isLocked() {
    return isset($this->status)
      && empty($this->is_new)
      && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}
