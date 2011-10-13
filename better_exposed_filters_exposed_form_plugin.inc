<?php
/**
 * @file
 * Provides an Better Exposed Filters exposed form plugin for View 3.x.
 */
class better_exposed_filters_exposed_form_plugin extends views_plugin_exposed_form_basic {

  function summary_title() {
    return t('BEF Settings');
  }

  function option_definition() {
    $options = parent::option_definition();

    // Add Better Exposed Filters options to those saved by Views
    $options['bef'] = array('default' => array());

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $bef_options = array();

    /*
     * Add options for exposed sorts
     */
    $exposed = FALSE;
    foreach ($this->display->handler->get_handlers('sort') as $label => $sort) {
      if ($sort->options['exposed']) {
        $exposed = TRUE;
        break;
      }
    }
    if ($exposed) {
      $bef_options['sort']['bef_format'] = array(
        '#type' => 'select',
        '#title' => t('Display exposed sort options as'),
        '#default_value' => $this->options['bef']['sort']['bef_format'],
        '#options' => array(
          'default' => t('Default select list'),
          'bef' => t('Radio Buttons'),
          'bef_links' => t('Links'),
        ),
        '#description' => t('Select a format for the exposed sort options.'),
      );
      $bef_options['sort']['combine'] = array(
        '#type' => 'checkbox',
        '#title' => t('Combine sort order with sort by'),
        '#default_value' => $this->options['bef']['sort']['combine'],
        '#description' => t(
          'Combines the sort by options and order (ascending or decending) into a single list.  Use this to
          display "Option1 (ascending)", "Option1 (descending)", "Option2 (ascending)", "Option2 (descending)"
          in a single form element.'
        ),
      );
      $bef_options['sort']['reset'] = array(
        '#type' => 'checkbox',
        '#title' => t('Include a "Reset sort" option'),
        '#default_value' => $this->options['bef']['sort']['reset'],
        '#description' => t('Adds a "Reset sort" link; Views will use the default sort order.'),
      );
      $bef_options['sort']['reset_label'] = array(
        '#type' => 'textfield',
        '#title' => t('"Reset sort" label'),
        '#default_value' => $this->options['bef']['sort']['reset_label'],
        '#description' => t('Text to use if the above option is checked'),
      );
    }

    /*
     * Add options for exposed pager
     */
    if (isset($this->display->display_options['pager']) && $this->display->display_options['pager']['options']['expose']['items_per_page']) {
      $bef_options['pager']['bef_format'] = array(
        '#type' => 'select',
        '#title' => t('Display exposed pager options as'),
        '#default_value' => $this->options['bef']['pager']['bef_format'],
        '#options' => array(
          'default' => t('Default select list'),
          'bef' => t('Radio Buttons'),
          'bef_links' => t('Links'),
        ),
        '#description' => t('Select a format for the exposed pager options.'),
      );
    }

    // Only add the description text once -- it was getting a little long to be added to
    // each filter
    $bef_filter_intro = FALSE;

    // Go through each filter and add BEF options
    foreach ($this->display->handler->get_handlers('filter') as $label => $filter) {
      if (!$filter->options['exposed']) {
        continue;
      }

      // If we're adding BEF filter options, add an intro to explain what's going on
      if (!$bef_filter_intro) {
        $bef_options['bef_intro'] = array(
          '#markup' => '<h3>' . t('Exposed Filter Settings') . '</h3><p>'
            . t('Select a format and additional options for each exposed filter.')
            . '</p><p>'
            . t('The "Nested" option places filter options is an unordered list.
                Hierarchical taxonomy filters will be rendered as nested,
                unordered lists. "Links" will render filter options as links,
                but is only available if "Allow multiple selections"
                is NOT checked in the "Configure filter criterion" form. The "Hidden"
                option is generally used for multi-step filters.  Note: if "Allow multiple
                selections" is checked in the "Configure filter criterion" form,
                checkboxes will be used, otherwise radio buttons.')
            . '</p><p>'
            . t('You may also add an optional description for any exposed filter
                in the "More options" section.')
            . '</p>',
        );
        $bef_filter_intro = TRUE;
      }

      // Is this a type of field we can tweak? If so, we'll display additional options.
      $valid = array('in', 'or', 'and');
      $bef_specific = FALSE;

      // We can work with any of these operators
      if (in_array($filter->operator, $valid)) {
        $bef_specific = TRUE;
      }
      // ... or any of these specific filters
      else if (isset($filter->value['type']) && 'date' == $filter->value['type']) {
        // Offer a datepicker option for date fields
        $bef_options[$label]['bef_format'] = array(
          '#type' => 'select',
          '#title' => t('Display "@label" exposed filter as', array('@label' => $filter->options['expose']['label'])),
          '#default_value' => $this->options['bef'][$label]['bef_format'],
          '#options' => array(
            'default' => t('Default text field'),
            'bef_datepicker' => t('jQuery UI Datepicker'),
          ),
        );
      }
      else if ('yes-no' == $filter->definition['type']) {
        // others?
        $bef_specific = TRUE;
      }
      else {
        // dsm($filter->definition['type']);
      }

      // Main BEF option: default/checkboxes/hidden/etc.
      if ($bef_specific) {
        $bef_options[$label]['bef_format'] = array(
          '#type' => 'select',
          '#title' => t('Display "@label" exposed filter as', array('@label' => $filter->options['expose']['label'])),
          '#default_value' => $this->options['bef'][$label]['bef_format'],
          '#options' => array(
            'default' => t('Default select list'),
            'bef' => t('Checkboxes/Radio Buttons'),
            'bef_ul' => t('Nested Checkboxes/Radio Buttons'),
            'bef_links' => t('Links'),
            'bef_hidden' => t('Hidden'),
          ),
        );
      }

      // Fieldset to keep the UI from getting out of hand
      $bef_options[$label]['more_options'] = array(
        '#type' => 'fieldset',
        '#title' => t('More options for "@label"', array('@label' => $filter->options['expose']['label'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      // Select all checkbox
      if ($bef_specific) {
        $bef_options[$label]['more_options']['bef_select_all_none'] = array(
          '#type' => 'checkbox',
          '#title' => t('Add select all/none links'),
          '#default_value' => $this->options['bef'][$label]['more_options']['bef_select_all_none'],
          '#disabled' => !$filter->options['expose']['multiple'],
          '#description' => t(
            'Add a "Select All/None" link when rendering the exposed filter using
              checkboxes. If this option is disabled, edit the filter and check the
              "Allow multiple selections".'
          ),
        );

        // Put filter in collapsible fieldset option
        // TODO: expand to all exposed filters
        $bef_options[$label]['more_options']['bef_collapsible'] = array(
          '#type' => 'checkbox',
          '#title' => t('Make this filter collapsible'),
          '#default_value' => $this->options['bef'][$label]['more_options']['bef_collapsible'],
          '#description' => t(
            'Puts this filter in a collapsible fieldset'
          ),
        );
      }

      // Build a description option form element -- available to all exposed filters
      $bef_options[$label]['more_options']['bef_filter_description'] = array(
        '#type' => 'textarea',
        '#title' => t('Description'),
        '#default_value' => $this->options['bef'][$label]['more_options']['bef_filter_description'],
        '#description' => t('Adds descriptive text to the exposed filter.  This is usually
                              rendered in smaller print under the label or the options.'),
      );
    } // foreach ($filters as $filter) {

    // Add BEF form elements to the exposed form options form
    $form['bef'] = $bef_options;
  }

  /*
   * Tweak the exposed filter form to show Better Exposed Filter options.
   */
  function exposed_form_alter(&$form, &$form_state) {
    parent::exposed_form_alter($form, $form_state);

    // If we have no visible elements, we don't show the Apply button.
    $show_apply = FALSE;

    // We only want to add the datepicker code once.
    // TODO: Does this need to be moved to a behavior.attach()?
    $add_datepicker = FALSE;
    $datepicker_format = '';

    /*
     * Handle exposed sort elements
     */
    if (isset($this->options['bef']['sort'])) {
      $show_apply = TRUE;

      // Check for combined sort_by and sort_order
      if ($this->options['bef']['sort']['combine']) {
        // Combine sort_by and sort_order into a single element
        $form['sort_bef_combine'] = array(
          '#type' => 'radios',
          '#title' => $form['sort_by']['#title'],   // Already sanitized by Views
        );
        $options = array();

        // Add reset sort option at the top of the list
        if ($this->options['bef']['sort']['reset']) {
          $options[' '] = t($this->options['bef']['sort']['reset_label']);
        }
        else {
          $form['sort_bef_combine']['#default_value'] = '';
        }

        $selected = '';
        foreach ($form['sort_by']['#options'] as $by_key => $by_val) {
          foreach ($form['sort_order']['#options'] as $order_key => $order_val) {
            // Use a space to separate the two keys, we'll unpack them in our submit handler
            $options["$by_key $order_key"] = "$by_val $order_val";
          }
        }

        $form['sort_bef_combine'] = array(
          '#type' => 'radios',
          '#options' => $options,
          '#title' => $form['sort_by']['#title'],   // Already sanitized by Views
        );

        // Handle display-specific details
        switch ($this->options['bef']['sort']['bef_format']) {
          case 'bef':
            $form['sort_bef_combine']['#prefix'] = '<div class="bef-sort-combined bef-select-as-radios">';
            $form['sort_bef_combine']['#suffix'] = '</div>';
            break;

          case 'bef_links':
            $form['sort_bef_combine']['#theme'] = 'select_as_links';
            break;

          case 'default':
            $form['sort_bef_combine']['#type'] = 'select';
            break;
        }

        // Add our submit routine to process
        $form['#submit'][] = 'bef_sort_combine_submit';

        // Remove the existing sort_by and sort_order elements
        unset($form['sort_by']);
        unset($form['sort_order']);
      } // if ($this->options['bef']['sort']['combine']) {
      else {
        // Leave sort_by and sort_order as separate elements
        if ('bef' == $this->options['bef']['sort']['bef_format']) {
          $form['sort_by']['#type'] = 'radios';
          if (empty($form['sort_by']['#process'])) {
            $form['sort_by']['#process'] = array();
          }
          array_unshift($form['sort_by']['#process'], 'form_process_radios');
          $form['sort_by']['#prefix'] = '<div class="bef-sortby bef-select-as-radios">';
          $form['sort_by']['#suffix'] = '</div>';

          $form['sort_order']['#type'] = 'radios';
          if (empty($form['sort_order']['#process'])) {
            $form['sort_order']['#process'] = array();
          }
          array_unshift($form['sort_order']['#process'], 'form_process_radios');
          $form['sort_order']['#prefix'] = '<div class="bef-sortorder bef-select-as-radios">';
          $form['sort_order']['#suffix'] = '</div>';
        }
        elseif ('bef_links' == $this->options['bef']['sort']['bef_format']) {
          $form['sort_by']['#theme'] = 'select_as_links';
          $form['sort_order']['#theme'] = 'select_as_links';
        }

        // Add reset sort option if selected
        if ($this->options['bef']['sort']['reset']) {
          array_unshift($form['sort_by']['#options'], $this->options['bef']['sort']['reset_label']);
        }
      } // if ($this->options['bef']['sort']['combine']) { ... } else {
    }   // if (isset($this->options['bef']['sort'])) {

    /*
     * Handle exposed pager elements
     */
    if (isset($this->options['bef']['pager'])) {
      $show_apply = TRUE;

      switch ($this->options['bef']['pager']['bef_format']) {
        case 'bef':
          $form['items_per_page']['#type'] = 'radios';
          if (empty($form['items_per_page']['#process'])) {
            $form['items_per_page']['#process'] = array();
          }
          array_unshift($form['items_per_page']['#process'], 'form_process_radios');
          $form['items_per_page']['#prefix'] = '<div class="bef-sortby bef-select-as-radios">';
          $form['items_per_page']['#suffix'] = '</div>';
          break;

        case 'bef_links':
          if (count($form['items_per_page']['#options']) > 1) {
            $form['items_per_page']['#theme'] = 'select_as_links';
            $form['items_per_page']['#items_per_page'] = max($form['items_per_page']['#default_value'], key($form['items_per_page']['#options']));
          }
          break;
      }
    }

    // Shorthand for all filters in this view
    $filters = $form_state['view']->display_handler->handlers['filter'];

    // Go through each saved option looking for Better Exposed Filter settings
    foreach ($this->options['bef'] as $label => $options) {

      // Sanity check: Ensure this filter is an exposed filter
      if (empty($filters[$label]) || !$filters[$label]->options['exposed']) {
        continue;
      }

      // Form element is designated by the element ID which is user-configurable
      $field_id = $form['#info']["filter-$label"]['value'];

      if (!isset($options['bef_format'])) {
        $options['bef_format'] = '';
      }
      switch ($options['bef_format']) {
        case 'bef_datepicker':
          $show_apply = TRUE;
          $add_datepicker = TRUE;

          if ((
            // Single Date API-based input element
            isset($form[$field_id]['value']['#type'])
              && 'date_text' == $form[$field_id]['value']['#type']
          )
          || (    // Or ...
            // Double Date API-based input elements such as "in-between"
            isset($form[$field_id]['min']) && isset($form[$field_id]['max'])
            && 'date_text' == $form[$field_id]['min']['#type']
            && 'date_text' == $form[$field_id]['max']['#type']
          )) {
            // Date module date field

            /*
             * Convert Date API formatting to jQuery formatDate formatting.
             *
             * @TODO: To be honest, I'm not sure this is needed.  Can you
             * set a Date API field to accept anything other than Y-m-d?
             * Well, better safe than sorry...
             *
             * @see
             *  http://us3.php.net/manual/en/function.date.php
             * and
             *  http://docs.jquery.com/UI/Datepicker/formatDate
             *
             * Array format: PHP date format => jQuery formatDate format
             * (comments are for the PHP format, lines that are commented out
             * do not have a jQuery formatDate equivalent, but maybe someday
             * they will...)
             */
            $convert = array(
              // Day
              'd' => 'dd',    // Day of the month, 2 digits with leading zeros 01 to 31
              'D' => 'D',     // A textual representation of a day, three letters  Mon through Sun
              'j' => 'd',     // Day of the month without leading zeros  1 to 31
              'l' => 'DD',    // (lowercase 'L') A full textual representation of the day of the week  Sunday through Saturday
              // 'N' => ' ',     // ISO-8601 numeric representation of the day of the week (added in PHP 5.1.0) 1 (for Monday) through 7 (for Sunday)
              // 'S' => ' ',     // English ordinal suffix for the day of the month, 2 characters st, nd, rd or th. Works well with j
              // 'w' => ' ',     // Numeric representation of the day of the week 0 (for Sunday) through 6 (for Saturday)
              'z' => 'o',     // The day of the year (starting from 0) 0 through 365

              // Week
              // 'W' => ' ',     // ISO-8601 week number of year, weeks starting on Monday (added in PHP 4.1.0) Example: 42 (the 42nd week in the year)

              // Month
              'F' => 'MM',    // A full textual representation of a month, such as January or March  January through December
              'm' => 'mm',    // Numeric representation of a month, with leading zeros 01 through 12
              'M' => 'M',     // A short textual representation of a month, three letters  Jan through Dec
              'n' => 'm',     // Numeric representation of a month, without leading zeros  1 through 12
              // 't' => ' ',     // Number of days in the given month 28 through 31

              // Year
              // 'L' => ' ',     // Whether it's a leap year  1 if it is a leap year, 0 otherwise.
              // 'o' => ' ',     // ISO-8601 year number. This has the same value as Y, except that if the ISO week number (W) belongs to the previous or next year, that year is used instead. (added in PHP 5.1.0)  Examples: 1999 or 2003
              'Y' => 'yy',    // A full numeric representation of a year, 4 digits Examples: 1999 or 2003
              'y' => 'y',     // A two digit representation of a year  Examples: 99 or 03

              // Time
              // 'a' => ' ',     // Lowercase Ante meridiem and Post meridiem am or pm
              // 'A' => ' ',     // Uppercase Ante meridiem and Post meridiem AM or PM
              // 'B' => ' ',     // Swatch Internet time  000 through 999
              // 'g' => ' ',     // 12-hour format of an hour without leading zeros 1 through 12
              // 'G' => ' ',     // 24-hour format of an hour without leading zeros 0 through 23
              // 'h' => ' ',     // 12-hour format of an hour with leading zeros  01 through 12
              // 'H' => ' ',     // 24-hour format of an hour with leading zeros  00 through 23
              // 'i' => ' ',     // Minutes with leading zeros  00 to 59
              // 's' => ' ',     // Seconds, with leading zeros 00 through 59
              // 'u' => ' ',     // Microseconds (added in PHP 5.2.2) Example: 654321
            );

            $format = '';
            if (isset($form[$field_id]['value'])) {
              $format = $form[$field_id]['value']['#date_format'];
              $form[$field_id]['value']['#attributes']['class'][] = 'bef-datepicker';
            }
            else {
              // Both min and max share the same format
              $format = $form[$field_id]['min']['#date_format'];
              $form[$field_id]['min']['#attributes']['class'][] = 'bef-datepicker';
              $form[$field_id]['max']['#attributes']['class'][] = 'bef-datepicker';
            }
            $datepicker_format = str_replace(array_keys($convert), array_values($convert), $format);
          }
          else {
            // Standard Drupal date field.
            if (isset($form[$field_id]['min']) && isset($form[$field_id]['max'])) {
              // Double Drupal date field input elements such as "in-between"
              $form[$field_id]['min']['#attributes']['class'][] = 'bef-datepicker';
              $form[$field_id]['max']['#attributes']['class'][] = 'bef-datepicker';
            }
            else {
              // Single Drupal date field input elements such as "less than"
              $form[$field_id]['#attributes']['class'][] = 'bef-datepicker';
            }
          }
          break;

        case 'bef_links':
          $show_apply = TRUE;

          $form[$field_id]['#theme'] = 'select_as_links';
          break;

        case 'bef_ul':
          $show_apply = TRUE;

          $form[$field_id]['#bef_nested'] = TRUE;
          // Intentionally falling through to case 'bef':

        case 'bef':
          $show_apply = TRUE;

          if (empty($form[$field_id]['#multiple'])) {
            // Single-select -- display as radio buttons
            $form[$field_id]['#type'] = 'radios';
            if (empty($form[$field_id]['#process'])) {
              $form[$field_id]['#process'] = array();
            }
            array_unshift($form[$field_id]['#process'], 'form_process_radios');

            // Clean up objects from the options array (happens for taxonomy-based filters)
            $opts = $form[$field_id]['#options'];
            $form[$field_id]['#options'] = array();
            foreach ($opts as $index => $opt) {
              if (is_object($opt)) {
                list($key, $val) = each($opt->option);
                $form[$field_id]['#options'][$key] = $val;
              }
              else {
                $form[$field_id]['#options'][$index] = $opt;
              }
            }

            if (isset($form[$field_id]['#options']['All'])) {
              // @TODO: The terms 'All' and 'Any' are customizable in Views
              if ($filters[$label]->options['expose']['multiple']) {
                // Some third-party filter handlers still add the "Any" option even if this is not
                // an optional filter.  Zap it here if they do.
                unset($form[$field_id]['#options']['All']);
              }
              else {
                // Otherwise, make sure the "Any" text is clean
                $form[$field_id]['#options']['All'] = check_plain($form[$field_id]['#options']['All']);
              }
            }

            // Render as radio buttons or radio buttons in a collapsible fieldset
            if (!empty($options['more_options']['bef_collapsible'])) {
              // Pass the description and title along in a way such that it doesn't get rendered
              // as part of the exposed form widget.  We'll render them as part of the fieldset.
              if (isset($form['#info']["filter-$label"]['label'])) {
                $form[$field_id]['#bef_title'] = $form['#info']["filter-$label"]['label'];
                unset($form['#info']["filter-$label"]['label']);
              }
              if (!empty($options['more_options']['bef_filter_description'])) {
                $form[$field_id]['#bef_description'] = $options['more_options']['bef_filter_description'];
                if (isset($form[$field_id]['#description'])) {
                  unset($form[$field_id]['#description']);
                }
              }

              // If the operator is exposed as well, put it inside the fieldset
              if ($filters[$label]->options['expose']['use_operator']) {
                $operator_id = $filters[$label]->options['expose']['operator_id'];
                $form[$field_id]['#bef_operator'] = $form[$operator_id];
                unset ($form[$operator_id]);
              }

              // Add collapse/expand Javascript and BEF CSS to prevent collapsed
              // fieldset from disappearing.
              if (empty($form[$field_id]['#attached']['js'])) {
                $form[$field_id]['#attached']['js'] = array();
              }
              $form[$field_id]['#attached']['js'][] = 'misc/form.js';
              $form[$field_id]['#attached']['js'][] = 'misc/collapse.js';

              if (empty($form[$field_id]['#attached']['css'])) {
                $form[$field_id]['#attached']['css'] = array();
              }
              $form[$field_id]['#attached']['css'][] = drupal_get_path('module', 'better_exposed_filters') . '/better_exposed_filters.css';

              // Take care of adding the fieldset in the theme layer
              $form[$field_id]['#theme'] = 'select_as_radios_fieldset';
            } // if (!empty($options['more_options']['bef_collapsible'])) {
            else {
              // Render select element as radio buttons
              $form[$field_id]['#attributes']['class'][] = 'bef-select-as-radios';
              $form[$field_id]['#theme'] = 'select_as_radios';
            }
          } // if (empty($form[$field_id]['#multiple'])) {
          else {
            // Render as checkboxes or checkboxes enclosed in a collapsible fieldset
            if (!empty($options['more_options']['bef_collapsible'])) {
              // Pass the description and title along in a way such that it doesn't get rendered
              // as part of the exposed form widget.  We'll render them as part of the fieldset.
              if (isset($form['#info']["filter-$label"]['label'])) {
                $form[$field_id]['#bef_title'] = $form['#info']["filter-$label"]['label'];
                unset($form['#info']["filter-$label"]['label']);
              }
              if (!empty($options['more_options']['bef_filter_description'])) {
                $form[$field_id]['#bef_description'] = $options['more_options']['bef_filter_description'];
                if (isset($form[$field_id]['#description'])) {
                  unset($form[$field_id]['#description']);
                }
              }

              // If the operator is exposed as well, put it inside the fieldset
              if ($filters[$label]->options['expose']['use_operator']) {
                $operator_id = $filters[$label]->options['expose']['operator_id'];
                $form[$field_id]['#bef_operator'] = $form[$operator_id];
                unset ($form[$operator_id]);
              }

              // Add collapse/expand Javascript and BEF CSS to prevent collapsed
              // fieldset from disappearing.
              if (empty($form[$field_id]['#attached']['js'])) {
                $form[$field_id]['#attached']['js'] = array();
              }
              $form[$field_id]['#attached']['js'][] = 'misc/form.js';
              $form[$field_id]['#attached']['js'][] = 'misc/collapse.js';

              if (empty($form[$field_id]['#attached']['css'])) {
                $form[$field_id]['#attached']['css'] = array();
              }
              $form[$field_id]['#attached']['css'][] = drupal_get_path('module', 'better_exposed_filters') . '/better_exposed_filters.css';

              // Take care of adding the fieldset in the theme layer
              $form[$field_id]['#theme'] = 'select_as_checkboxes_fieldset';
            }
            else {
              $form[$field_id]['#theme'] = 'select_as_checkboxes';
            }

            if ($options['more_options']['bef_select_all_none']) {
              // Add BEF's JavaScript to the mix to handle select all/none functionality
              // @TODO: move to #attached?
              drupal_add_js(drupal_get_path('module', 'better_exposed_filters') . '/better_exposed_filters.js');

              // Add select all/none functionality to this filter.
              if (!isset($form[$field_id]['#attributes']['class'])) {
                $form[$field_id]['#attributes']['class'] = array();
              }
              $form[$field_id]['#bef_select_all_none'] = TRUE;
            }
          } // if (empty($form[$field_id]['#multiple'])) { ... } else {
          break; // case 'bef':

        case 'bef_hidden':
          $form['#info']["filter-$field_id"]['label'] = ''; // Hide the label
          if (empty($form[$field_id]['#multiple'])) {
            $form[$field_id]['#type'] = 'hidden';
          }
          else {
            $form[$field_id]['#theme'] = 'select_as_hidden';
          }
          break;

        default:
          // Handle functionality for exposed filters that are not limited to BEF only filters
          $show_apply = TRUE;

          // Add a description to the exposed filter
          if (!empty($options['more_options']['bef_filter_description'])) {
            $form[$field_id]['#description'] = t($options['more_options']['bef_filter_description']);
          }
          break;

      } // switch ($options['bef_format'])
    } // foreach ($this->options['bef']...)

    // If our form has no visible filters, hide the submit button.
    $form['submit']['#access'] = $show_apply;
    $form['reset']['#access'] = $show_apply;

    // Only add datepicker code once
    drupal_add_js(array('better_exposed_filters' => array('bef_datepicker' => FALSE)), 'setting');
    if ($add_datepicker) {
       // Add Datepicker JS and CSS
      drupal_add_js('misc/ui/jquery.ui.core.min.js');
      drupal_add_js('misc/ui/jquery.ui.datepicker.min.js');
      drupal_add_js(drupal_get_path('module', 'better_exposed_filters') . '/better_exposed_filters.js');
      drupal_add_css('misc/ui/jquery.ui.core.css');
      drupal_add_css('misc/ui/jquery.ui.theme.css');
      drupal_add_css('misc/ui/jquery.ui.datepicker.css');

      // Trigger datepicker in better_exposed_filters.js
      drupal_add_js(array('better_exposed_filters' => array(
        'bef_datepicker' => TRUE,
        'bef_dateformat' => $datepicker_format,
      )), 'setting');
    }

  } // function exposed_form_alter(...)
}