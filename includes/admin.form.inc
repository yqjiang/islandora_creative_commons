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
    '#title' => t('DSID(s)'),
    '#default_value' => variable_get('islandora_creative_commons_dsid', ''),
    '#size' => 60,
    '#maxlength' => 150,
  );

  $form['islandora_creative_commons_xpath'] = array(
    '#type' => 'textarea',
    '#title' => t('xPath(s)'),
    '#default_value' => variable_get('islandora_creative_commons_xpath', ''),
  );

  $form['islandora_creative_commons_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
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
  variable_set('islandora_creative_commons_dsid', $form_state['values']['islandora_creative_commons_dsid']);
  variable_set('islandora_creative_commons_xpath', $form_state['values']['islandora_creative_commons_xpath']);
}

