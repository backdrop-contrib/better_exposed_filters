<?php
/**
 * @file
 * bef_test_content.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function bef_test_content_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'bef_test_view';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'BEF Test View';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'BEF Test View';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'better_exposed_filters';
  $handler->display->display_options['exposed_form']['options']['bef'] = array(
    'general' => array(
      'allow_secondary' => 0,
      'secondary_label' => 'Advanced options',
    ),
    'sort' => array(
      'bef_format' => 'bef_toggle_links',
      'advanced' => array(
        'collapsible' => 0,
        'collapsible_label' => 'Sort options',
        'combine' => 1,
        'combine_rewrite' => '',
        'reset' => 0,
        'reset_label' => '',
        'is_secondary' => 0,
      ),
    ),
    'pager' => array(
      'bef_format' => 'bef_links',
      'is_secondary' => 0,
    ),
    'field_integer_value' => array(
      'bef_format' => 'default',
      'slider_options' => array(
        'bef_slider_min' => '0',
        'bef_slider_max' => '99999',
        'bef_slider_step' => '1',
        'bef_slider_animate' => '',
        'bef_slider_orientation' => 'horizontal',
      ),
      'more_options' => array(
        'bef_select_all_none' => FALSE,
        'bef_collapsible' => 0,
        'is_secondary' => 1,
        'any_label' => '',
        'bef_filter_description' => '',
        'tokens' => array(
          'available' => array(
            0 => 'global_types',
          ),
        ),
        'rewrite' => array(
          'filter_rewrite_values' => '',
        ),
      ),
    ),
  );
  $handler->display->display_options['exposed_form']['options']['input_required'] = 1;
  $handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'filtered_html';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['pager']['options']['expose']['items_per_page'] = TRUE;
  $handler->display->display_options['pager']['options']['expose']['items_per_page_label'] = 'Yo! Show me';
  $handler->display->display_options['pager']['options']['expose']['items_per_page_options_all'] = TRUE;
  $handler->display->display_options['pager']['options']['expose']['items_per_page_options_all_label'] = 'All dem things!';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'field_letters' => 'field_letters',
    'field_numbers' => 'field_numbers',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_letters' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_numbers' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Letters */
  $handler->display->display_options['fields']['field_letters']['id'] = 'field_letters';
  $handler->display->display_options['fields']['field_letters']['table'] = 'field_data_field_letters';
  $handler->display->display_options['fields']['field_letters']['field'] = 'field_letters';
  $handler->display->display_options['fields']['field_letters']['delta_offset'] = '0';
  /* Field: Content: Numbers */
  $handler->display->display_options['fields']['field_numbers']['id'] = 'field_numbers';
  $handler->display->display_options['fields']['field_numbers']['table'] = 'field_data_field_numbers';
  $handler->display->display_options['fields']['field_numbers']['field'] = 'field_numbers';
  /* Field: Content: Integer */
  $handler->display->display_options['fields']['field_integer']['id'] = 'field_integer';
  $handler->display->display_options['fields']['field_integer']['table'] = 'field_data_field_integer';
  $handler->display->display_options['fields']['field_integer']['field'] = 'field_integer';
  $handler->display->display_options['fields']['field_integer']['settings'] = array(
    'thousand_separator' => ',',
    'prefix_suffix' => 1,
  );
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['date_format'] = 'short';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  $handler->display->display_options['sorts']['created']['exposed'] = TRUE;
  $handler->display->display_options['sorts']['created']['expose']['label'] = 'Post date';
  /* Filter criterion: Content: Integer (field_integer) */
  $handler->display->display_options['filters']['field_integer_value']['id'] = 'field_integer_value';
  $handler->display->display_options['filters']['field_integer_value']['table'] = 'field_data_field_integer';
  $handler->display->display_options['filters']['field_integer_value']['field'] = 'field_integer_value';
  $handler->display->display_options['filters']['field_integer_value']['operator'] = '>=';
  $handler->display->display_options['filters']['field_integer_value']['group'] = 1;
  $handler->display->display_options['filters']['field_integer_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_integer_value']['expose']['operator_id'] = 'field_integer_value_op';
  $handler->display->display_options['filters']['field_integer_value']['expose']['label'] = 'Integer (field_integer)';
  $handler->display->display_options['filters']['field_integer_value']['expose']['operator'] = 'field_integer_value_op';
  $handler->display->display_options['filters']['field_integer_value']['expose']['identifier'] = 'field_integer_value';
  $handler->display->display_options['filters']['field_integer_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  $handler->display->display_options['filters']['field_integer_value']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['field_integer_value']['group_info']['label'] = 'Integer (field_integer)';
  $handler->display->display_options['filters']['field_integer_value']['group_info']['identifier'] = 'field_integer_value';
  $handler->display->display_options['filters']['field_integer_value']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Small',
      'operator' => '<=',
      'value' => array(
        'value' => '500',
        'min' => '',
        'max' => '',
      ),
    ),
    2 => array(
      'title' => 'Medium',
      'operator' => 'between',
      'value' => array(
        'value' => '',
        'min' => '500',
        'max' => '1000',
      ),
    ),
    3 => array(
      'title' => 'Large',
      'operator' => '>=',
      'value' => array(
        'value' => '1000',
        'min' => '',
        'max' => '',
      ),
    ),
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'test';
  $export['bef_test_view'] = $view;

  return $export;
}
