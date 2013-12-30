<?php
/**
 * @file
 * Defines the invitation group subscription controller class.
 */

/**
 * Invitation group subscription controller.
 */
class GroupInviteSubscriptionController extends GroupPublicSubscriptionController {

  /**
   * Overrides GroupPublicSubscriptionController::outsiderForm().
   *
   * Shows the invite management form.
   */
  public function outsiderForm($form, &$form_state, Group $group, $account) {
    $form['info'] = array(
      '#markup' => 'This group is invite only',
    );

    return $form;
  }

}
