<?php
/**
 * @file
 * Defines the GroupSubscriptionControllerInterface.
 */

/**
 * Interface for group subscription controllers.
 */
interface GroupSubscriptionControllerInterface {

  /**
   * Render the subscription form.
   *
   * This method is called whenever a user is presented with the membership
   * widget for a group. Typically, this could show a 'join group' or
   * 'leave group' button depending on the user's membership status.
   *
   * @param Group $group
   *   The group to render the form for.
   * @param stdObject $account
   *   (optional) The account to check the membership status for.
   *
   * @return array
   *   A render array to generate the form with.
   */
  public function subscriptionForm($form, &$form_state, Group $group, $account = NULL);

  /**
   * Validate the subscription form.
   */
  public function subscriptionFormValidate($form, &$form_state);

  /**
   * Submit the subscription form.
   */
  public function subscriptionFormSubmit($form, &$form_state);

}
