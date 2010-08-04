<?php
// $Id: $

/**
 * Implements hook_views_plugins().
 */
function views_pgsql_views_plugins() {
  return array(
    'query' => array(
      'pgsql_query' => array(
        'title' => t('SQL Query'),
        'help' => t('Query will be generated and run using the Drupal database API.'),
        'handler' => 'views_pgsql_plugin_query_pgsql',
        'parent' => 'views_query',
      ),
    ),
  );
}