if (Drupal.jsEnabled) {
  $(document).ready(function() {
    /*
     * Add Select all/none links to specified checkboxes
     */
    var selected = $('.form-checkboxes.bef-select-all-none');
    if (selected.length) {
      var selAll = Drupal.t('Select All');
      var selNone = Drupal.t('Select None');
      
      // Set up a prototype link and event handlers
      var link = $('<a class="bef-toggle" href="#">'+ selAll +'</a>')
      link.click(function() {
        if (selAll == $(this).text()) {
          // Select all the checkboxes
          $(this)
            .html(selNone)
            .siblings('.form-item').each(function() {
              $('input:checkbox', this).attr('checked', 'checked');
            });
        }
        else {
          // Unselect all the checkboxes
          $(this)
            .html(selAll)
            .siblings('.form-item').each(function() {
              $('input:checkbox', this).attr('checked', '');
            });
        }
      });

      // Add link to the page for each set of checkboxes.
      selected.each(function(index) {
        // Clone the link prototype and insert into the DOM
        var newLink = link.clone(true);
        newLink.insertBefore($('.form-item:first', this));
        
        // If all checkboxes are already checked by default then switch to Select None
        if ($('input:checkbox:checked', this).length == $('input:checkbox', this).length) {
          newLink.click();
        }
      });
    }
    
    /*
     * Turn the "Add select all/none" on and off based on other settings
     * 
     * @TODO:
     * When Drupal upgrades to jQuery 1.3, we can use this to provide addition UX support
     * (Uncomment the similar block in better_exposed_filters.module)
     * 
    $('#edit-options-expose-bef-format, #edit-options-expose-single').live('click', function() {
      if (('bef' == $('#edit-options-expose-bef-format').val()) 
          && ('checked' == $('#edit-options-expose-single').attr('checked'))) {
        $('#edit-options-expose-bef-select-all-none').attr('disabled', '');
      }
      else {
        $('#edit-options-expose-bef-select-all-none').attr('disabled', 'disabled');
      }
      return false;     // Stop event from propagating further
    });
    */
  });
}
