<?php
// $Id$
class better_exposed_filters_exposed_form_plugin extends views_plugin_exposed_form_basic {

  function option_definition() {
    $options = parent::option_definition();
    
    // Add Better Exposed Filters options to those saved by Views
    $options['bef_format'] = array('default' => 'default');
    $options['bef_format_single'] = array('default' => FALSE);

    return $options;
  }
}