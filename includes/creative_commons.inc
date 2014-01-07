<?php

/**
 * @file
 * This file contains all the functions to manitobas creative commons chooser.
 */

/**
 * Draws the creative commons form. Altered in form builder forms.
 */
function islandora_creative_commons_form(&$form, &$form_state) {
  form_load_include($form_state, 'inc', 'islandora_creative_commons', 'includes/creative_commons');

  // Form options.
  $modification_options = array(
    'y' => t('Yes'),
    'n' => t('No'),
    'sa' => t('Yes, as long as others share alike'),
  );

  $countries = array(
    'international' => t('International'),
    'ar' => t('Argentina'),
    'au' => t('Australia'),
    'at' => t('Austria'),
    'be' => t('Belgium'),
    'br' => t('Brazil'),
    'bg' => t('Bulgaria'),
    'ca' => t('Canada'),
    'cl' => t('Chile'),
    'cn' => t('China Mainland'),
    'co' => t('Colombia'),
    'cr' => t('Costa Rica'),
    'hr' => t('Croatia'),
    'cz' => t('Czech Republic'),
    'dk' => t('Denmark'),
    'ec' => t('Ecuador'),
    'eg' => t('Egypt'),
    'ee' => t('Estonia'),
    'fi' => t('Finland'),
    'fr' => t('France'),
    'de' => t('Germany'),
    'gr' => t('Greece'),
    'gt' => t('Guatemala'),
    'hk' => t('Hong kong'),
    'hu' => t('Hungary'),
    'in' => t('India'),
    'ie' => t('Ireland'),
    'il' => t('Israel'),
    'it' => t('Italy'),
    'jp' => t('Japan'),
    'lu' => t('Luxembourg'),
    'mk' => t('Macedonia'),
    'my' => t('Malaysia'),
    'mt' => t('Malta'),
    'mx' => t('Mexico'),
    'nl' => t('Netherlands'),
    'nz' => t('New Zealand'),
    'no' => t('Norway'),
    'pe' => t('Peru'),
    'ph' => t('Philippines'),
    'pl' => t('Poland'),
    'pt' => t('Portugal'),
    'pr' => t('Puerto Rico'),
    'ro' => t('Romania'),
    'rs' => t('Serbia'),
    'sg' => t('Singapore'),
    'si' => t('Slovenia'),
    'za' => t('South Africa'),
    'kr' => t('South Korea'),
    'es' => t('Spain'),
    'se' => t('Sweden'),
    'ch' => t('Switzerland'),
    'tw' => t('Taiwan'),
    'th' => t('Thailand'),
    'uk' => t('UK: England & Wales'),
    'scotland' => t('Uk: Scotland'),
    'ug' => t('Uganda'),
    'us' => t('United states'),
    'vn' => t('Vietnam'),
  );

  $commercial_options = array(
    'y' => t('Yes'),
    'n' => t('No'),
  );

  // Setup defaults:
  $derivatives_default = 'y';
  $commercial_default = 'y';
  $jurisdiction_default = 'ca';

  if (isset($form['form_id']['#id']) && $form['form_id']['#id'] == 'edit-xml-form-builder-edit-datastream-form') {
    // Edit form.
    $item = islandora_creative_commons_find_creative_commons_mods_element($form_state['storage']['objective_form_storage']['document']->document);

    if ($item !== FALSE) {
      $item = explode('/', $item->nodeValue);

      $properties = explode('-', $item[0]);

      $commercial_default = 'y';
      $derivatives_default = 'y';

      foreach ($properties as $property) {
        switch ($property) {
          case 'by':
            break;
          case 'nc':
            $commercial_default = 'n';
            break;
          case 'nd':
            $derivatives_default = 'n';
            break;
          case 'sa':
            $derivatives_default = 'sa';
            break;
        }
      }

      $jurisdiction_default = empty($item[2]) ? 'international' : $item[2];
    }
    $form['update']['#weight'] = 100;
    array_unshift($form['update']['#submit'], 'islandora_creative_commons_form_edit_submit');
  }
  else {
    // Ingest steps.
    $form['next']['#weight'] = 100;
    $form['prev']['#weight'] = 99;
    $form['next']['#submit'][]='islandora_creative_commons_form_submit';
  }

  $derivatives = isset($form_state['values']['license_fieldset']['allow_modifications']) ? $form_state['values']['license_fieldset']['allow_modifications'] : $derivatives_default;
  $commercial = isset($form_state['values']['license_fieldset']['allow_commercial']) ? $form_state['values']['license_fieldset']['allow_commercial'] : $commercial_default;
  $jurisdiction = isset($form_state['values']['license_fieldset']['license_jurisdiction']) ? $form_state['values']['license_fieldset']['license_jurisdiction'] : $jurisdiction_default;

  // Form elements.
  $form['license_fieldset'] = array(
    '#type' => 'fieldset',
    '#collapsed' => TRUE,
    '#collapsible' => FALSE,
    '#title' => t('License'),
  );

  $form['license_fieldset']['allow_modifications'] = array(
    '#type' => 'select',
    '#title' => t('Allow modifications of your work?'),
    '#options' => $modification_options,
    '#default_value' => $derivatives,
    '#ajax' => array(
      'wrapper' => 'license-output',
      'callback' => 'islandora_creative_commons_on_form_input_change',
    ),
  );

  $form['license_fieldset']['allow_commercial'] = array(
    '#type' => 'select',
    '#title' => t('Allow commercial uses of your work?'),
    '#options' => $commercial_options,
    '#default_value' => $commercial,
    '#ajax' => array(
      'wrapper' => 'license-output',
      'callback' => 'islandora_creative_commons_on_form_input_change',
    ),
  );

  $form['license_fieldset']['license_jurisdiction'] = array(
    '#type' => 'select',
    '#title' => t('License Jurisdiction'),
    '#options' => $countries,
    '#default_value' => $jurisdiction,
    '#ajax' => array(
      'wrapper' => 'license-output',
      'callback' => 'islandora_creative_commons_on_form_input_change',
    ),
  );

  $license_url = 'http://api.creativecommons.org/rest/1.5/license/standard/get?commercial=' . $commercial . '&derivatives=' . $derivatives . '&jurisdiction=' . $jurisdiction;
  $response = simplexml_load_file($license_url);

  $form_state['islandora_creative_commons'] = array();
  $form_state['islandora_creative_commons']['cc_license_uri'] = (string)$response->{'license-uri'};

  $form['license_fieldset']['license_output'] = array(
    '#prefix' => '<div id="license-output">',
    '#markup' => '<strong>' . t('Selected license:') . '</strong><br>' . $response->html->asXml(),
    '#suffix' => '</div>',
  );
  dsm($form);
}

/**
 * Ajax callback to render the CC license.
 */
function islandora_creative_commons_on_form_input_change($form, &$form_state) {
  return $form['license_fieldset']['license_output'];
}

/**
 * Submit function called when using ingest steps.
 */
function islandora_creative_commons_form_submit($form, &$form_state) {
  $dsid_array = variable_get('islandora_creative_commons_dsid_xpath_array', array());
  dsm($form_state);
  foreach ($dsid_array as $dsid => $xpath) {
    dsm(isset($form_state['islandora']['objects'][0][$dsid]),"$dsid");
    if (isset($form_state['islandora']['objects'][0][$dsid])) {
      // We are inside of ingest steps.
      $document = new DOMDocument();
      $loaded = $document->loadXML($form_state['islandora']['objects'][0][$dsid]->content);
      dsm($loaded);
      if ($loaded) {
        islandora_creative_commons_create_new_element($document, $form_state['islandora_creative_commons']['cc_license_uri'], $xpath);
        $document->formatOutput = TRUE;
        $form_state['islandora']['objects'][0][$dsid]->content = $document->saveXML();
      }
    }
  }
}

/**
 * Submit function called when using the edit form.
 */
function islandora_creative_commons_form_edit_submit($form, &$form_state) {
  /*
  $item = islandora_creative_commons_find_creative_commons_mods_element($form_state['storage']['objective_form_storage']['document']->document);
  if ($item) {
    $item->nodeValue = islandora_creative_commons_hack_up_url($form_state['manitoba']['cc_license_uri']);
  }
  else {
    islandora_creative_commons_create_new_element($form_state['storage']['objective_form_storage']['document']->document, $form_state['manitoba']['cc_license_uri']);
  }*/
}

/**
 * Given a DomDocument, returns the accessCondition element.
 *
 * @param DomDocument $document
 *   The document to search.
 *
 * @return mixed
 *   The DomElement if its found FALSE otherwise.
 */
function islandora_creative_commons_find_creative_commons_element($document) {
 /* // Exit early if null is passed in.
  if (empty($document)) {
    return FALSE;
  }
  $xpath = new DOMXPath($document);
  $xpath->registerNamespace('mods', 'http://www.loc.gov/mods/v3');
  $result = $xpath->query("/mods:mods/mods:accessCondition[@type='Creative Commons License']");
  if ($result->length > 0) {
    return $result->item(0);
  }
  else {
    return FALSE;
  }*/
}

/**
 * Hacks up the URL to fit with existing URLs.
 */
function islandora_creative_commons_hack_up_url($url) {
  return str_replace('http://creativecommons.org/licenses/', '', $url);
}

/**
 * Creates a new element for accessCondition, inserts the CC URL and appends.
 */
function islandora_creative_commons_create_new_element($document, $url, $xpath) {
    // Substring here to cut of the CC URL for UofM.
    dsm($xpath);
   /* $access_condition = $document->createElementNS('http://www.loc.gov/mods/v3', 'accessCondition', islandora_creative_commons_hack_up_url($url));
    $access_condition->setAttribute('type', 'Creative Commons License');
    $document->documentElement->appendChild($access_condition);*/
}
