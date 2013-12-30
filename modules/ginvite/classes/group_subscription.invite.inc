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
   * Shows the buttons for accepting or declining a group invitation..
   */
  public function memberForm($form, &$form_state, GroupMembership $group_membership) {
    // If the membership is already active, show the leave button instead.
    if ($group_membership->status != GROUP_MEMBERSHIP_INVITED) {
      return parent::memberForm($form, $form_state, $group_membership);
    }

    $form['info'] = array(
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#markup' => t('You have been invited to join this group'),
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['accept'] = array(
      '#type' => 'submit',
      '#value' => t('Accept'),
    );

    $form['actions']['decline'] = array(
      '#type' => 'submit',
      '#value' => t('Decline'),
    );

    return $form;
  }

  /**
   * Overrides GroupPublicSubscriptionController::outsiderForm().
   *
   * Shows the invite only text.
   */
  public function outsiderForm($form, &$form_state, Group $group, $account) {
    $form['info'] = array(
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#markup' => t('This group is invite only'),
    );

    return $form;
  }

}
