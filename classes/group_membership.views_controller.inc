<?php
/**
 * @file
 * Defines the Entity API Views controller class for group memberships.
 */

/**
 * Controller for generating Views data for Group memberships.
 */
class GroupMembershipViewsController extends EntityDefaultViewsController {

  /**
   * Add group entity relationships to groups.
   */
  public function views_data() {
    $data = parent::views_data();

    $data['group_membership']['roles'] = array(
      'title' => t('Group role'),
      'help' => t('The granted group roles for a membership.'),
      'relationship' => array(
        'handler' => 'group_handler_membership_role_relationship',
        'label' => t('Membership role'),
        'base' => 'group_role',
      ),
    );

    return $data;
  }

}
