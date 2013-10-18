(function ($) {

Drupal.behaviors.groupFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.node-form-group-information', context).drupalSetSummary(function (context) {
      var input = $('#edit-group-settings-group', context).val();

      if (input != '') {
        return $.trim(input);
      }

      return Drupal.t('Not a group node');
    });
  }
};

})(jQuery);
