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
      '#markup' => 'Request membership',
    );

    return $form;
  }

}
