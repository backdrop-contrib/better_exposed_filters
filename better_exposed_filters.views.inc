<?php
// $Id$

/**
 * @file better_exposed_filters.views.inc
 * 
 * Adds Views3 support
 * 
 * Views3 adds the concept of exposed forms to the mix.  In addition, elements injected into a Views
 * dialog is no longer saved along with the Views form information (see the unpack_options() and
 * options_definition() methods of the views_object object). 
 *
 */

/**
 * Implementation of hook_views_plugins()
 */
function better_exposed_filters_views_plugins() {
  return array(
    'exposed_form' => array(
      'better_exposed_filters' => array(
        'title' => t('Better Exposed Filters'),
        'help' => t(
          'Changes the display of exposed filters from single- or multi-select boxes
           to radio buttons or checkboxes, respectively.'
         ),
         'handler' => 'better_exposed_filters_exposed_form_plugin',
         'path' => drupal_get_path('module', 'better_exposed_filters'),
         'uses options' => TRUE,
         'type' => 'normal',
         'parent' => 'basic',
      ),
    ),
  );
}
