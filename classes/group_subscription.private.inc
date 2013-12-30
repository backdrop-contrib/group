<?php
/**
 * @file
 * Defines the private group subscription controller class.
 */

/**
 * Private group subscription controller.
 */
class GroupPrivateSubscriptionController extends GroupPublicSubscriptionController {

  /**
   * Overrides GroupPublicSubscriptionController::outsiderForm().
   *
   * Shows a 'Request membership' button.
   */
  public function outsiderForm($form, &$form_state, Group $group, $account) {
    $form['info'] = array(
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#markup' => t('This group requires admin approval to join'),
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['request'] = array(
      '#type' => 'submit',
      '#value' => t('Request membership'),
    );

    return $form;
  }

}
