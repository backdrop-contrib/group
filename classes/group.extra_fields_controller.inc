<?php
/**
 * @file
 * Defines the Entity API extra fields class for groups.
 */

/**
 * Controller for generating extra fields for groups and group entities.
 */
class GroupExtraFieldsController extends EntityDefaultExtraFieldsController {

  /**
   * Implements EntityExtraFieldsControllerInterface::fieldExtraFields().
   */
  public function fieldExtraFields() {
    foreach (group_types() as $type => $group_type) {
      // Add the 'title' field to every group type.
      // Display shouldn't be necessary: https://drupal.org/node/2123923.
      // @todo Follow-up on above comment.
      foreach (array('form', 'display') as $context) {
        $extra['group'][$type][$context]['title'] = array(
          'label' => t('Group title'),
          'description' => t('The name of the group'),
          'weight' => -5,
        );
      }

      // Add the membership action form.
      $extra['group'][$type]['display']['membership_actions'] = array(
        'label' => t('Membership action form'),
        'description' => t('The form containing various membership actions such as: join, leave, request membership, ...'),
        'weight' => -4,
      );
    }

    return $extra;
  }
}
