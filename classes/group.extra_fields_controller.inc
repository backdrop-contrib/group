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
    // Add the 'title' field to every group type.
    foreach (group_types() as $type => $group_type) {
      foreach (array('form', 'display') as $context) {
        $extra['group'][$type][$context]['title'] = array(
          'label' => t('Group title'),
          'description' => t('The name of the group'),
          'weight' => -5,
        );
      }
    }

    return $extra;
  }
}
