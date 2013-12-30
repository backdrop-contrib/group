<?php
/**
 * @file
 * Defines the public group subscription controller class.
 */

/**
 * Public group subscription controller.
 *
 * Classes extending this controller should probably their own memberForm().
 * If you only need a 'Leave group' button for members, there is no need to
 * overwrite outsiderForm().
 */
class GroupPublicSubscriptionController implements GroupSubscriptionControllerInterface {

  /**
   * Implements GroupSubscriptionControllerInterface.
   */
  public function subscriptionForm($form, &$form_state, Group $group, $account = NULL) {
    global $user;

    if (!isset($account)) {
      $account = $user;
    }

    if (!empty($account->uid)) {
      if ($group_membership = group_membership_load($group->gid, $account->uid)) {
        return $this->memberForm($form, $form_state, $group_membership);
      }
    }

    return $this->outsiderForm($form, $form_state, $group, $account);
  }

  /**
   * The form for users who are already a member of the group.
   *
   * Shows a 'Leave group' button.
   *
   * @param GroupMembership $group_membership
   *   The membership to display the form for.
   *
   * @return array
   *   A render array to generate the form with.
   */
  public function memberForm($form, &$form_state, GroupMembership $group_membership) {
    $form['leave'] = array(
      '#type' => 'submit',
      '#value' => t('Leave group'),
    );

    return $form;
  }

  /**
   * The form for users who are not a member of the group.
   *
   * Shows a 'Join group' button.
   *
   * @param Group $group
   *   The group to render the form for.
   * @param stdObject $account
   *   The account to render the form for.
   *
   * @return array
   *   A render array to generate the form with.
   */
  public function outsiderForm($form, &$form_state, Group $group, $account) {
    $form['join'] = array(
      '#type' => 'submit',
      '#value' => t('Join group'),
    );

    return $form;
  }

  /**
   * Implements GroupSubscriptionControllerInterface.
   */
  public function subscriptionFormValidate($form, &$form_state) {
  }

  /**
   * Implements GroupSubscriptionControllerInterface.
   */
  public function subscriptionFormSubmit($form, &$form_state) {
    dpm($form_state['values']);
  }

}
