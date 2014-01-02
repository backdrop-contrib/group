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
    // If the membership status is not 'invited', default to parent function.
    if ($group_membership->status != 'invited') {
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

  /**
   * Implements GroupSubscriptionControllerInterface.
   */
  public function subscriptionFormSubmit($form, &$form_state) {
    $values = $form_state['values'];
    $group = $form_state['build_info']['args'][0];
    $uid = $form_state['uid'];

    // Act upon clicking 'Accept'.
    if (isset($values['accept']) && $values['op'] == $values['accept']) {
      $group_membership = $group->getMember($uid);
      $group_membership->status = 'active';
      $group_membership->save();

      drupal_set_message(t('You have accepted the group invitation.'));
    }

    // Act upon clicking 'Decline'.
    elseif (isset($values['decline']) && $values['op'] == $values['decline']) {
      $group_membership = $group->getMember($uid);
      $group_membership->delete();

      drupal_set_message(t('You have declined the group invitation.'));
    }

    // Delegate the submission to the parent class.
    else {
      parent::subscriptionFormSubmit($form, $form_state);
    }
  }

}
