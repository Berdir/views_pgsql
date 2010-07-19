<?php
// $Id: $

/**
 * Implements hook_views_plguins().
 */
function views_pqsql_views_plguins() {
  return array(
    'query' => array(
      'pqsql_query' => array(
        'title' => t('SQL Query'),
        'help' => t('Query will be generated and run using the Drupal database API.'),
        'handler' => 'views_pqsql_plugin_query_pqsql',
        'parent' => 'views_query',
      ),
    ),
    'pager' => array(
      'time'
    ),
  );
}