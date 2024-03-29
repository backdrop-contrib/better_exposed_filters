<?php
/**
 * @file
 * Provides theming functions to display exposed forms using different
 * interfaces.
 */

/**
 * Themes a select element as checkboxes enclosed in a collapsible fieldset.
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element
 *
 * @return string
 *   HTML representing the form element.
 */
function theme_select_as_checkboxes_fieldset($vars) {
  // Merge incoming element with some default values. Prevents a lot of this.
  // $foo = isset($bar) ? $bar : $bar_default;
  $element = array_merge(
    array(
      '#bef_title' => '',
      '#bef_description' => '',
      '#bef_operator' => array(),
    ),
    $vars['element']
  );

  $fieldset = array(
    '#title' => $element['#bef_title'],
    '#description' => $element['#bef_description'],
    '#attributes' => array(
      'class' => array(
        'bef-select-as-checkboxes-fieldset',
        'collapsible',
      ),
    ),
  );
  if (empty($element['#value'])) {
    // Using the FAPI #collapsible and #collapsed attribute doesn't work here
    // TODO: not sure why...
    $fieldset['#attributes']['class'][] = 'collapsed';
  }

  // We rendered the description as part of the fieldset element, don't render
  // it again along with the checkboxes.
  unset($element['#bef_description']);

  $children = '';
  if (!empty($element['#bef_operator'])) {
    // Put an exposed operator inside the fieldset.
    $children = backdrop_render($element['#bef_operator']);
  }

  // Render the checkboxes.
  $children .= theme('select_as_checkboxes', array('element' => $element));

  $fieldset['#children'] = $children;
  return theme('fieldset', array('element' => $fieldset));
}

/**
 * Themes a select element as a set of checkboxes.
 *
 * @see http://api.drupal.org/api/function/theme_select/7
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element.
 *
 * @return string
 *   HTML representing the form element.
 */
function theme_select_as_checkboxes($vars) {
  $element = $vars['element'];
  if (!empty($element['#bef_nested'])) {
    if (empty($element['#attributes']['class'])) {
      $element['#attributes']['class'] = array();
    }
    $element['#attributes']['class'][] = 'form-checkboxes';
    return theme('select_as_tree', array('element' => $element));
  }

  // The selected keys from #options.
  $selected_options = empty($element['#value']) ? (empty($element['#default_value']) ? array() : $element['#default_value']) : $element['#value'];
  if (!is_array($selected_options)) {
    $selected_options = array($selected_options);
  }

  // Grab exposed filter description.  We'll put it under the label where it
  // makes more sense.
  $description = '';
  if (!empty($element['#bef_description'])) {
    $description = '<div class="description">' . $element['#bef_description'] . '</div>';
  }

  $output = '<div class="bef-checkboxes">';
  foreach ($element['#options'] as $option => $elem) {
    if ('All' === $option) {
      // TODO: 'All' text is customizable in Views.
      // No need for an 'All' option -- either unchecking or checking all the
      // checkboxes is equivalent.
      continue;
    }

    // Check for Taxonomy-based filters.
    if (is_object($elem)) {
      $slice = array_slice($elem->option, 0, 1, TRUE);
      $option = key($slice);
      $elem = current($slice);
    }

    // Check for optgroups.  Put subelements in the $element_set array and add
    // a group heading. Otherwise, just add the element to the set.
    $element_set = array();
    $is_optgroup = FALSE;
    if (is_array($elem)) {
      $output .= '<div class="bef-group">';
      $output .= '<div class="bef-group-heading">' . $option . '</div>';
      $output .= '<div class="bef-group-items">';
      $element_set = $elem;
      $is_optgroup = TRUE;
    }
    else {
      $element_set[$option] = $elem;
    }

    foreach ($element_set as $key => $value) {
      $output .= theme('bef_checkbox', array('element' => $element, 'value' => $key, 'label' => $value, 'selected' => array_search($key, $selected_options) !== FALSE));
    }

    if ($is_optgroup) {
      // Close group and item <div>s.
      $output .= '</div></div>';
    }

  }
  $output .= '</div>';

  // Fake theme_checkboxes() which we can't call because it calls
  // theme_form_element() for each option.
  $attributes['class'] = array('form-checkboxes', 'bef-select-as-checkboxes');
  if (!empty($element['#bef_select_all_none'])) {
    $attributes['class'][] = 'bef-select-all-none';
  }
  if (!empty($element['#bef_select_all_none_nested'])) {
    $attributes['class'][] = 'bef-select-all-none-nested';
  }
  if (!empty($element['#attributes']['class'])) {
    $attributes['class'] = array_merge($element['#attributes']['class'], $attributes['class']);
  }

  return '<div' . backdrop_attributes($attributes) . ">$description$output</div>";
}

/**
 * Themes a select element as a series of hidden fields.
 *
 * @see http://api.drupal.org/api/function/theme_select/7
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element.
 *
 * @return string
 *   HTML representing the form element.
 */
function theme_select_as_hidden($vars) {
  $element = $vars['element'];
  $output = '';
  $selected_options = empty($element['#value']) ? $element['#default_value'] : $element['#value'];
  $properties = array(
    'title' => isset($element['#title']) ? $element['#title'] : '',
    'description' => isset($element['#bef_description']) ? $element['#bef_description'] : '',
    'required' => FALSE,
  );

  foreach ($element['#options'] as $option => $elem) {
    // Check for Taxonomy-based filters.
    if (is_object($elem)) {
      $slice = array_slice($elem->option, 0, 1, TRUE);
      $option = key($slice);
      $elem = current($slice);
    }

    // Check for optgroups.  Put subelements in the $element_set array and add a
    // group heading. Otherwise, just add the element to the set.
    $element_set = array();
    if (is_array($elem)) {
      $element_set = $elem;
    }
    else {
      $element_set[$option] = $elem;
    }

    foreach ($element_set as $key => $value) {
      // Only render fields for selected values -- no selected values renders
      // zero fields.
      if (array_search($key, $selected_options) !== FALSE) {
        // Custom ID for each hidden field based on the <select> element's
        // original ID.
        $id = backdrop_html_id($element['#id'] . '-' . $key);
        $hidden = array(
          'id' => $id,
          'name' => check_plain($element['#name']) . '[]',
          'value' => check_plain($key),
        );
        $output .= theme('form_element', array(
          'element' => array_merge($properties, array(
            '#id' => $id,
            '#children' => theme_hidden(array('element' => $hidden)),
          ))
        ));
      }
    }
  }
  return $output;
}

/**
 * Themes a select element as radio buttons enclosed in a collapsible fieldset.
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element.
 *
 * @return string
 *   HTML representing the form element.
 */
function theme_select_as_radios_fieldset($vars) {
  // Merge incoming element with some default values. Prevents a lot of this.
  // $foo = isset($bar) ? $bar : $bar_default;
  $element = array_merge(
    array(
      '#bef_title' => '',
      '#bef_description' => '',
      '#bef_operator' => array(),
    ),
    $vars['element']
  );

  // The "all" option is the first in the list. If the selected radio button is
  // the all option, then leave the fieldset collapsed.  Otherwise, render it
  // opened.
  $keys = array_keys($element['#options']);
  $all = array_shift($keys);

  $fieldset = array(
    '#title' => $element['#bef_title'],
    '#description' => $element['#bef_description'],
    '#attributes' => array(
      'class' => array(
        'bef-select-as-checkboxes-fieldset',
        'collapsible',
      ),
    ),
  );
  if (empty($element['#value'])) {
    // Using the FAPI #collapsible and #collapsed attribute doesn't work here.
    // TODO: not sure why...
    $fieldset['#attributes']['class'][] = 'collapsed';
  }

  // We rendered the description as part of the fieldset element, don't render
  // it again along with the checkboxes.
  unset($element['#bef_description']);

  $children = '';
  if (!empty($element['#bef_operator'])) {
    // Put an exposed operator inside the fieldset.
    $children = backdrop_render($element['#bef_operator']);
  }

  // Render the radio buttons.
  $children .= theme('select_as_radios', $element);

  $fieldset['#children'] = $children;
  return theme('fieldset', array('element' => $fieldset));
}

/**
 * Themes a select drop-down as a collection of radio buttons.
 *
 * @see http://api.drupal.org/api/function/theme_select/7
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element.
 *
 * @return string
 *   HTML representing the form element.
 */
function theme_select_as_radios($vars) {
  $element = &$vars['element'];

  if (!empty($element['#bef_nested'])) {
    return theme('select_as_tree', $vars);
  }

  $output = '';
  foreach (element_children($element) as $key) {
    if (isset($element['#bef_term_descriptions'][$key])) {
      $element[$key]['#description'] = $element['#bef_term_descriptions'][$key];
    }
    $element[$key]['#default_value'] = NULL;
    $element[$key]['#children'] = theme('radio', array('element' => $element[$key]));
    $output .= theme('form_element', array('element' => $element[$key]));
  }

  return $output;
}

/**
 * Themes a taxonomy-based exposed filter as a nested unordered list.
 *
 * Note: this routine depends on the '-' char prefixed on the term names by
 * Views to determine depth.
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element.
 *
 * @return string
 *   Nested, unordered list of filter options
 */
function theme_select_as_tree($vars) {
  $element = $vars['element'];

  // The selected keys from #options.
  $selected_options = empty($element['#value']) ? $element['#default_value'] : $element['#value'];

  // Build a bunch of nested unordered lists to represent the hierarchy based
  // on the '-' prefix added by Views or optgroup structure.
  $output = '<ul class="bef-tree">';
  $curr_depth = 0;
  foreach ($element['#options'] as $option_value => $option_label) {

    // Check for Taxonomy-based filters.
    if (is_object($option_label)) {
      $slice = array_slice($option_label->option, 0, 1, TRUE);
      $option_value = key($slice);
      $option_label = current($slice);
    }

    // Check for optgroups -- which is basically a two-level deep tree.
    if (is_array($option_label)) {
      // TODO:
    }
    else {
      // Build hierarchy based on prefixed '-' on the element label.
      if (t('- Any -') == $option_label) {
        $depth = 0;
      }
      else {
        preg_match('/^(-*).*$/', $option_label, $matches);
        $depth = strlen($matches[1]);
        $option_label = ltrim($option_label, '-');
      }

      // Build either checkboxes or radio buttons, depending on Views' settings.
      $html = '';
      if (!empty($element['#multiple'])) {
        if (isset($element['#bef_term_descriptions'][$option_value])) {
          $element[$option_value]['#description'] = $element['#bef_term_descriptions'][$option_value];
        }
        $html = theme('bef_checkbox', array(
          'element' => $element,
          'value' => $option_value,
          'label' => $option_label,
          'selected' => (array_search($option_value, $selected_options) !== FALSE),
        ));
      }
      else {
        if (isset($element['#bef_term_descriptions'][$option_value])) {
          $element[$option_value]['#description'] = $element['#bef_term_descriptions'][$option_value];
        }
        $element[$option_value]['#title'] = $option_label;
        $element[$option_value]['#children'] = theme('radio', array('element' => $element[$option_value]));
        $html .= theme('form_element', array('element' => $element[$option_value]));
      }

      if ($depth > $curr_depth) {
        // We've moved down a level: create a new nested <ul>.
        // TODO: Is there is a way to jump more than one level deeper at a time?
        // I don't think so...
        $output .= "<ul class='bef-tree-child bef-tree-depth-$depth'><li>$html";
        $curr_depth = $depth;
      }
      elseif ($depth < $curr_depth) {
        // We've moved up a level: finish previous <ul> and <li> tags, once for
        // each level, since we can jump multiple levels up at a time.
        while ($depth < $curr_depth) {
          $output .= '</li></ul>';
          $curr_depth--;
        }
        $output .= "</li><li>$html";
      }
      else {
        if (-1 == $curr_depth) {
          // No </li> needed -- this is the first element.
          $output .= "<li>$html";
          $curr_depth = 0;
        }
        else {
          // Remain at same level as previous entry.
          $output .= "</li><li>$html";
        }
      }
    }
  } // foreach ($element['#options'] as $option_value => $option_label)

  if (!$curr_depth) {
    // Close last <li> tag.
    $output .= '</li>';
  }
  else {
    // Finish closing <ul> and <li> tags.
    while ($curr_depth) {
      $curr_depth--;
      $output .= '</li></ul></li>';
    }
  }

  // Close the opening <ul class="bef-tree"> tag.
  $output .= '</ul>';

  // Add exposed filter description.
  $description = '';
  if (!empty($element['#bef_description'])) {
    $description = '<div class="description">' . $element['#bef_description'] . '</div>';
  }

  // Add the select all/none option, if needed.
  if (!empty($element['#bef_select_all_none'])) {
    if (empty($element['#attributes']['class'])) {
      $element['#attributes']['class'] = array();
    }
    $element['#attributes']['class'][] = 'bef-select-all-none';
  }
  // Add the select all/none nested option, if needed.
  if (!empty($element['#bef_select_all_none_nested'])) {
    if (empty($element['#attributes']['class'])) {
      $element['#attributes']['class'] = array();
    }
    $element['#attributes']['class'][] = 'bef-select-all-none-nested';
  }

  // Name and multiple attributes are not valid for <div>'s.
  if (isset($element['#attributes']['name'])) {
    unset($element['#attributes']['name']);
  }
  if (isset($element['#attributes']['multiple'])) {
    unset($element['#attributes']['multiple']);
  }

  return '<div' . backdrop_attributes($element['#attributes']) . ">$description$output</div>";
}

/**
 * Themes a select drop-down as a collection of links.
 *
 * @see http://api.drupal.org/api/function/theme_select/7
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element.
 *
 * @return string
 *   HTML representing the form element.
 */
function theme_select_as_links($vars) {
  $element = $vars['element'];

  $output = '';
  $name = $element['#name'];

  // Collect selected values so we can properly style the links later.
  $selected_options = array();
  if (isset($element['#value'])) {
    $selected_options[] = $element['#value'];
  }
  else {
    if (isset($element['#default_value'])) {
      $selected_options[] = $element['#default_value'];
    }
  }

  // Add to the selected options specified by Views whatever options are in the
  // URL query string, but only for this filter.
  $urllist = parse_url(request_uri());
  if (isset($urllist['query'])) {
    $query = array();
    parse_str($urllist['query'], $query);
    foreach ($query as $key => $value) {
      if ($key != $name) {
        continue;
      }
      if (is_array($value)) {
        // This filter allows multiple selections, so put each one on the
        // selected_options array.
        foreach ($value as $option) {
          $selected_options[] = $option;
        }
      }
      else {
        $selected_options[] = $value;
      }
    }
  }

  // Clean incoming values to prevent XSS attacks.
  if (is_array($element['#value'])) {
    foreach ($element['#value'] as $index => $item) {
      unset($element['#value'][$index]);
      $element['#value'][check_plain($index)] = check_plain($item);
    }
  }
  elseif (is_string($element['#value'])) {
    $element['#value'] = check_plain($element['#value']);
  }

  // Go through each filter option and build the appropriate link or plain text.
  foreach ($element['#options'] as $option => $elem) {
    if (!empty($element['#hidden_options'][$option])) {
      continue;
    }
    // Check for Taxonomy-based filters.
    if (is_object($elem)) {
      $slice = array_slice($elem->option, 0, 1, TRUE);
      $option = key($slice);
      $elem = current($slice);
    }

    // Check for optgroups.  Put subelements in the $element_set array and add
    // a group heading. Otherwise, just add the element to the set.
    $element_set = array();
    if (is_array($elem)) {
      $element_set = $elem;
    }
    else {
      $element_set[$option] = $elem;
    }

    $links = array();
    $multiple = !empty($element['#multiple']);

    // Always use the current path for the links since this will work for
    // exposed forms in blocks as well as regular exposed forms.
    $path = current_path();

    foreach ($element_set as $key => $value) {
      $element_output = '';
      // Custom ID for each link based on the <select>'s original ID.
      $id = backdrop_html_id($element['#id'] . '-' . $key);
      $elem = array(
        '#id' => $id,
        '#markup' => '',
        '#type' => 'bef-link',
        '#name' => $id,
      );

      $link_options = array();
      // Add "active" class to the currently active filter link.
      if (in_array((string) $key, $selected_options)) {
        $link_options['attributes'] = array('class' => array('active'));
        $url = bef_replace_query_string_arg($name, $key, $multiple, TRUE, $path);
      }
      else {
        $url = bef_replace_query_string_arg($name, $key, $multiple, FALSE, $path);
      }
      $elem['#children'] = l($value, $url, $link_options);
      $element_output = theme('form_element', array('element' => $elem));

      if (!empty($element['#settings']['combine_param']) && $element['#name'] == $element['#settings']['combine_param'] && !empty($element['#settings']['toggle_links'])) {
        $sort_pair = explode(' ', $key);
        if (count($sort_pair) == 2) {
          // Highlight the link if it is the selected sort_by (can be either
          // asc or desc, it doesn't matter).
         if (strpos($selected_options[0], $sort_pair[0]) === 0) {
            $element_output = str_replace('form-item', 'form-item selected', $element_output);
          }
        }
      }
      $output .= $element_output;

    }
  }

  $properties = array(
    '#description' => isset($element['#bef_description']) ? $element['#bef_description'] : '',
    '#children' => $output,
  );

  $output = '<div class="bef-select-as-links">';
  $output .= theme('form_element', array('element' => $properties));

  // Add attribute that hides the select form element.
  $vars['element']['#attributes']['style'] = 'display: none;';
  $output .= theme('select', array('element' => $vars['element']));
  if (!empty($element['#value'])) {
    if (is_array($element['#value'])) {
      foreach ($element['#value'] as $value) {
        $output .= '<input type="hidden" class="bef-new-value" name="' . $name . '[]" value="' . $value . '" />';
      }
    }
    else {
      $output .= '<input type="hidden" class="bef-new-value" name="' . $name . '" value="' . $element['#value'] . '" />';
    }
  }
  $output .= '</div>';

  return $output;
}

/**
 * Themes some exposed form elements in a collapsible fieldset.
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element.
 *
 * @return string
 *   HTML to render the form element.
 */
function theme_secondary_exposed_elements($vars) {
  $element = $vars['element'];

  // Render child elements in the order they would appear as exposed filters.
  // First collect the elements that have a specified position and order them
  // based on that position. Then render those without a position.
  $children = array();
  $unordered = array();
  foreach (element_children($element) as $id) {
    if (isset($element[$id]['#bef_position'])) {
      $children[$element[$id]['#bef_position']] = $element[$id];
    }
    else {
      $unordered[] = $element[$id];
    }
  }
  ksort($children, SORT_NUMERIC);
  $children = array_merge($children, $unordered);

  $output = '<div class="bef-secondary-options">';
  foreach ($children as $child) {
    $output .= backdrop_render($child);
  }
  $output .= '</div>';

  return $output;
}

/*
 *
 * Helper functions
 *
 */

/**
 * Build a BEF checkbox.
 *
 * @see http://api.drupal.org/api/function/theme_checkbox/7
 *
 * @param array $element
 *   Original <select> element generated by Views.
 * @param string $value
 *   Return value of this checkbox option.
 * @param string $label
 *   Label of this checkbox option.
 * @param bool $selected
 *   Checked or not.
 *
 * @return [type]
 *   HTML to render a checkbox.
 */
function theme_bef_checkbox($variables) {
  $element = $variables['element'];
  $value = check_plain($variables['value']);
  $label = filter_xss_admin($variables['label']);
  $selected = $variables['selected'];
  $id = backdrop_html_id($element['#id'] . '-' . $value);
  // Custom ID for each checkbox based on the <select>'s original ID.
  $properties = array(
    '#required' => FALSE,
    '#id' => $id,
    '#type' => 'bef-checkbox',
    '#name' => $id,
    '#description' => isset($element['#bef_term_descriptions'][$value]) ? $element['#bef_term_descriptions'][$value] :
      '',
  );

  // Prevent the select-all-none class from cascading to all checkboxes.
  if (!empty($element['#attributes']['class'])
      && FALSE !== ($key = array_search('bef-select-all-none', $element['#attributes']['class']))) {
    unset($element['#attributes']['class'][$key]);
  }

  // Unset the name attribute as we are setting it manually.
  unset($element['#attributes']['name']);

  // Unset the multiple attribute as it doesn't apply for checkboxes.
  unset ($element['#attributes']['multiple']);

  $checkbox = '<input type="checkbox" '
    // Brackets are key -- just like select.
    . 'name="' . $element['#name'] . '[]" '
    . 'id="' . $id . '" '
    . 'value="' . $value . '" '
    . ($selected ? 'checked="checked" ' : '')
    . backdrop_attributes($element['#attributes']) . ' />';
  $properties['#children'] = "$checkbox <label class='option' for='$id'>$label</label>";
  $output = theme('form_element', array('element' => $properties));
  return $output;
}

/**
 * Replaces/adds a given query string argument to the current URL.
 *
 * @param string $key
 *   Query string key (argument).
 * @param string $value
 *   Query string value.
 * @param bool $multiple
 *   (optional) TRUE if this key/value pair allows multiple values.
 * @param bool $remove
 *   (optional) TRUE if this key/value should be a link to remove/unset the
 *   filter.
 * @param string $path
 *   (optional) Use this specify the View results page when the exposed form
 *   is displayed as a block and may be a different URL from the results.
 *   Defaults to the current path if unspecified.
 *
 * @return string
 *   URL.
 */
function bef_replace_query_string_arg($key, $value, $multiple = FALSE, $remove = FALSE, $path = '') {
  if (!$path) {
    $path = implode('/', arg());
  }

  // Prevents us from having to check for each index from parse_url that we may
  // use.
  $urllist = array('path' => '', 'fragment' => '', 'query' => '');
  $urllist = array_merge($urllist, parse_url(request_uri()));
  $fragment = urldecode($urllist['fragment']);
  $query = array();
  parse_str($urllist['query'], $query);
  if (isset($query[$key]) && is_array($query[$key])) {
    // Multiple values allowed for this existing key.
    if ($remove && ($key_remove = array_search($value, $query[$key])) !== FALSE) {
      unset($query[$key][$key_remove]);
    }
    else {
      $query[$key][] = $value;
    }
  }
  else {
    // Create a new key.
    if ($multiple && !$remove) {
      $query[$key] = array($value);
    }
    elseif (!$remove) {
      $query[$key] = $value;
    }
  }
  // Unset page arg so we don't land on an empty page off the end of the newly
  // filtered listing.
  unset($query['page']);
  return url(ltrim($path, '/'), array(
    'query' => $query,
    'fragment' => $fragment,
    'absolute' => TRUE,
  ));
}
