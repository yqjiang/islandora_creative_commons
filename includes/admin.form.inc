<?php

/**
 * @file
 * Handles the display/submission of the admin settings form for this module.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_creative_commons_admin_form($form, $form_state) {
  $form = array();
  $form['islandora_creative_commons_dsid'] = array(
    '#type' => 'textfield',
    '#title' => t('DSID'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 150,
  );

  $form['islandora_creative_commons_xpath'] = array(
    '#type' => 'textfield',
    '#title' => t('xPath'),
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 150,
  );
  
  $form['islandora_creative_commons_add'] = array(
    '#type' => 'submit',
    '#value' => t('Add/Edit'),
  );
  
    $headers = array(
    'dsid' => t('DSID'),
    'xpath'=> t('xPath'),
  );
  $options = array();
  $dsid_array = variable_get('islandora_creative_commons_dsid_xpath_array', array());
  foreach ($dsid_array as $dsid => $xpath) {
    $options[$dsid] = array(
      'dsid' => $dsid,
      'xpath' => $xpath,
    );
  }
  $form['islandora_creative_commons_table'] = array(
    '#type' => 'tableselect',
    '#title' => t('Creative Commons Datastreams'),
    '#header' => $headers,
    '#options' => $options,
    '#empty' => t('No datastream available.'),
  );

 $form['islandora_creative_commons_delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  return $form;
}
function islandora_creative_commons_admin_form_validate($form, $form_state) {
  switch ($form_state['clicked_button']['#id']) {
    case 'edit-islandora-creative-commons-add':
      if ($form_state['values']['islandora_creative_commons_dsid'] && !$form_state['values']['islandora_creative_commons_xpath']) {
        form_set_error('islandora_creative_commons_xpath', 'empty xpath');
      }
      break;
  }
}

/**
 * Submit the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_creative_commons_admin_form_submit($form, $form_state) {
  $dsid_array = variable_get('islandora_creative_commons_dsid_xpath_array', array());
  switch ($form_state['clicked_button']['#id']) {
    case 'edit-islandora-creative-commons-add':
      $dsid = $form_state['values']['islandora_creative_commons_dsid'];
      $dsid_array[$dsid] = $form_state['values']['islandora_creative_commons_xpath'];
      break;
    case 'edit-islandora-creative-commons-delete':
      foreach ($form_state['values']['islandora_creative_commons_table'] as $dsid => $checked) {
        if ($checked !== 0) {
          unset($dsid_array[$checked]);
        }
      }
      break;
  }
  variable_set('islandora_creative_commons_dsid_xpath_array', $dsid_array);
}

