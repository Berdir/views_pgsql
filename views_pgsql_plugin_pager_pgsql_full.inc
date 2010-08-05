<?php
// $Id$

class views_pgsql_plugin_pager_pgsql_full extends views_plugin_pager_full {
  /**
   * Execute the count query, which will be done just prior to the query
   * itself being executed.
   */
  function execute_count_query(&$count_query, $args) {
    $this->total_items = views_pgsql_result(views_pgsql_query($count_query, $args));
    if (!empty($this->options['offset'])) {
      $this->total_items -= $this->options['offset'];
    }

    $this->update_page_info();
    return $this->total_items;
  }
}