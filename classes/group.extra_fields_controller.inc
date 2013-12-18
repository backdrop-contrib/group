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
      foreach (array('form', 'display') as $context) {
        $extra['group'][$type][$context]['title'] = array(
          'label' => t('Group title'),
          'description' => t('The name of the group'),
          'weight' => -5,
        );
      }

      // Add the membership form to public and private group types.
      switch ($group_type->subscription_type) {
        case 'public':
          $extra['group'][$type]['display']['join'] = array(
            'label' => t('Join group form'),
            'description' => t('The form to allow outsiders to join the group with.'),
            'weight' => -4,
          );
          break;
        case 'private':
          $extra['group'][$type]['display']['apply'] = array(
            'label' => t('Request membership form'),
            'description' => t('The form to allow outsiders to apply for group membership with.'),
            'weight' => -4,
          );
          break;
      }
    }

    return $extra;
  }
}
