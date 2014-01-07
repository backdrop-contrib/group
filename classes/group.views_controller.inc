<?php
/**
 * @file
 * Defines the Entity API Views controller class for groups.
 */

/**
 * Controller for generating Views data for Group.
 */
class GroupViewsController extends EntityDefaultViewsController {

  /**
   * Add group entity relationships to groups.
   */
  public function views_data() {
    $data = parent::views_data();

    // Add an implicit join to {group_entity} for group views.
    $data['group_entity']['table']['join']['groups'] = array(
      'left_field' => 'gid',
      'field' => 'gid',
    );

    // Then add a relationship to all sorts of group entities.
    foreach (entity_get_info() as $type => $e_info) {
      if (!empty($e_info['group entity'])) {
        $data['group_entity'][$e_info['base table']] = array(
          'title' => $e_info['label'],
          'help' => t('@label entities associated with this group.', array('@label' => $e_info['label'])),
          'relationship' => array(
            'group' => t('Group entity'),
            'label' => t('Group @label', array('@label' => drupal_strtolower($e_info['label']))),
            'base' => $e_info['base table'],
            'relationship field' => 'entity_id',
            'extra' => array(
              array('table' => 'group_entity', 'field' => 'entity_type', 'value' => $type),
            ),
          ),
        );
      }
    }

    return $data;
  }

}
