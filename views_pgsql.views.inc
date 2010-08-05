<?php
// $Id$

/**
 * Implements hook_views_plugins().
 */
function views_pgsql_views_plugins() {
  return array(
    'query' => array(
      'pgsql_query' => array(
        'title' => t('PGSQL Query'),
        'help' => t('Query will be generated and run using the Drupal database API.'),
        'handler' => 'views_pgsql_plugin_query_pgsql',
        'parent' => 'views_query',
      ),
    ),
    'pager' => array(
      'pgsql_full' => array(
        'title' => t('Paged output, full pager(PGSQL)'),
        'help' => t('Paged output, full Drupal style(PGSQL)'),
        'handler' => 'views_pgsql_plugin_pager_pgsql_full',
//         'help topic' => 'pager-full',
        'uses options' => TRUE,
        'parent' => 'full',
      ),
    ),
  );
}