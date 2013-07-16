(function ($) {

Drupal.behaviors.groupFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.node-form-group-information', context).drupalSetSummary(function (context) {
      var option = $('.form-item-group-settings-group option:selected', context);

      if (option.val() != '0') {
        return $.trim(option.text());
      }

      return Drupal.t('Not a group node');
    });
  }
};

})(jQuery);
