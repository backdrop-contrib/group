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

  /**
   * Implements GroupSubscriptionControllerInterface.
   */
  public function subscriptionFormSubmit($form, &$form_state) {
    $values = $form_state['values'];

    // Act upon clicking 'Request membership'.
    if (isset($values['request']) && $values['op'] == $values['request']) {
      // @todo Implement this.
      drupal_set_message(t('You have requested to join the group.'));
      drupal_set_message(t('Not implemented yet'), 'warning');
    }

    // Delegate the submission to the parent class.
    else {
      parent::subscriptionFormSubmit($form, $form_state);
    }
  }

}
