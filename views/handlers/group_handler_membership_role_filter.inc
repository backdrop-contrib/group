<?php
/**
 * @file
 * Definition of group_handler_membership_role_filter.
 */

/**
 * Handler to filter memberships on group roles.
 *
 * @ingroup views_filter_handlers
 */
class group_handler_membership_role_filter extends views_handler_filter_in_operator {

  /**
   * Join {group_membership} with {group_membership_role} if necessary.
   */
  public function join_with_roles() {
    $this->ensure_my_table();

    // Check if the {group_membership_role} table has been joined already. That
    // table should only be joined by this module and as such can be retrieved
    // because a filter should not care about the type of join.
    foreach ($this->query->table_queue as $table) {
      if ($table['table'] == 'group_membership_role') {
        $this->gmr_alias = $table['alias'];
        return;
      }
    }

    // Otherwise we join {group_membership} with {group_membership_role}.
    $join_definition = array(
      'left_table' => $this->table_alias,
      'left_field' => 'mid',
      'table' => 'group_membership_role',
      'field' => 'mid',
    );

    $join = new views_join();
    $join->definition = $join_definition;
    $join->construct();
    $join->adjusted = TRUE;

    $this->gmr_alias = $this->query->add_table('group_membership_role', $this->relationship, $join);
  }

  public function op_simple() {
    if (empty($this->value)) {
      return;
    }
    $this->join_with_roles();

    // We use array_values() because the checkboxes keep keys and that can cause
    // array addition problems.
    $this->query->add_where($this->options['group'], "$this->gmr_alias.role", array_values($this->value), $this->operator);
  }

  public function op_empty() {
    $this->join_with_roles();
    if ($this->operator == 'empty') {
      $operator = "IS NULL";
    }
    else {
      $operator = "IS NOT NULL";
    }

    $this->query->add_where($this->options['group'], "$this->gmr_alias.role", NULL, $operator);
  }
}
