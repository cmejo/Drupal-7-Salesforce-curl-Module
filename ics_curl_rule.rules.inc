<?php

/**
 * @file
 * A simple mechanism for rules to make http requests.
 */

/**
 * Implements hook_rules_action_info().
 */
function ics_curl_rule_rules_action_info() {
  $items = array();
  $items['request_url'] = array(
    'label' => t('ICS Salesforce cURL'),
    'base' => 'ics_curl_rule_request_url',
    'group' => t('Data'),
    'parameter' => array(
      'url' => array(
        'type' => 'uri',
        'label' => 'URL',
      ),
      'subject' => array(
        'type' => 'text',
        'label' => 'Salesforce Sync subject',
      ),
      'salesforce_oid' => array(
        'type' => 'text',
        'label' => 'Salesforce OID',
      ),
    ),
    'provides' => array(
      'http_response' => array(
        'type' => 'text',
        'label' => t('HTTP data'),
      ),
    ),
  );
  return $items;
}

/**
 * Performs the request action.
 */
function ics_curl_rule_request_url($url, $subject, $salesforce_oid) {
  if (!function_exists('drupal_curl_request')) {
    drupal_set_message("Webclient.module is not setup on this drupal site and is required for this rule", 'error');
  }

  global $user;
  $account = user_load($user->uid);

  $node = menu_get_object();

  // Assign salesforce oid to data array.
  $sf_data = array(
    'oid' => $salesforce_oid,
  );

  // Loop thru salesforce map, assign user data to array.
  $sf_map = array(
  // App => salesforce.
    'field_first_name' => 'first_name',
    'field_last_name' => 'last_name',
    'field_address' => 'street',
    'field_address_2' => '+street',
    'field_city' => 'city',
    'field_state_province' => 'state',
    'field_zip_postal_code' => 'zip',
    'field_country' => 'country',
    'field_phone' => 'phone',
    'field_fax' => 'fax',
    'field_company'=> 'company',
  );
  
  foreach ($sf_map as $d_key => $sf_key) {
    if (!empty($account->{$d_key})) {
      $sf_data[$sf_key] = stripslashes($account->{$d_key}['und'][0]['value']);
    }
  }
  // Mail is not in an array.
  $sf_data['email'] = $account->mail;
  
  // Add node data to array.
  $sf_data['title'] = $node->title;
  $sf_data['node_url'] = url("node/" . $node->nid);

  $options = array(
    'method' => 'POST',
    'data' => http_build_query($sf_data),
    'return' => DRUPAL_CURL_RETURN_RESPONSE,
  );
  $response = drupal_curl_request($url, $options);
}
