<?php
// $Id: $

class views_pgsql_plugin_query_pgsql extends views_plugin_query_default {
  function init($base_table = 'node', $base_field = 'nid') {
    require_once drupal_get_path('module', 'views_pgsql') . '/views_pgsql.inc';

    parent::init($base_table, $base_field);
  }

  /**
   * Returns the array of every field of the field-part of the query.
   *
   * This function differs from the parent one by not using GLOBALS['db_type'].
   */
  function compile_fields($fields_array) {
    $fields = $distinct = array();
    $non_aggregates = array();

    foreach ($fields_array as $field) {
      $string = '';
      if (!empty($field['table'])) {
        $string .= $field['table'] . '.';
      }
      $string .= $field['field'];
      // store for use with non-aggregates below
      $fieldname = (!empty($field['alias']) ? $field['alias'] : $string);

      if (!empty($field['distinct'])) {
        $string = "DISTINCT($string)";
      }

      if (!empty($field['count'])) {
        // Retained for compatibility
        $field['function'] = 'count';
      }

      if (!empty($field['function'])) {
        $info = $this->get_aggregation_info();
        if (!empty($info[$field['function']]['method']) && function_exists($info[$field['function']]['method'])) {
          $string = $info[$field['function']]['method']($field['function'], $string);
        }

        $this->has_aggregate = TRUE;
      }
      elseif ($this->distinct && !in_array($fieldname, $this->groupby)) {
        // This line was changed
        $string = "FIRST($string)";
      }
      elseif (empty($field['aggregate'])) {
        $non_aggregates[] = $fieldname;
      }

      if ($field['alias']) {
        $string .= " AS $field[alias]";
      }

      if (!empty($field['distinct']) && empty($field['function'])) {
        $distinct[] = $string;
      }
      else {
        $fields[] = $string;
      }

      if (!empty($get_count_optimized)) {
        // We only want the first field in this case.
        break;
      }
    }
    return array(
      $distinct,
      $fields,
      $non_aggregates,
    );
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->current_page.
   * 
   * The changes here is that it uses the views_pgsql_ functions instead of db_.
   */
  function execute(&$view) {
    $external = FALSE; // Whether this query will run against an external database.
    $query = db_rewrite_sql($view->build_info['query'], $view->base_table, $view->base_field, array('view' => &$view));
    $count_query = db_rewrite_sql($view->build_info['count_query'], $view->base_table, $view->base_field, array('view' => &$view));
    $args = $view->build_info['query_args'];

    vpr($query);

    $items = array();
    if ($query) {
      $replacements = module_invoke_all('views_query_substitutions', $view);
      $query = str_replace(array_keys($replacements), $replacements, $query);
      $count_query = 'SELECT COUNT(*) FROM (' . str_replace(array_keys($replacements), $replacements, $count_query) . ') count_alias';

      if (is_array($args)) {
        foreach ($args as $id => $arg) {
          $args[$id] = str_replace(array_keys($replacements), $replacements, $arg);
        }
      }

      // Detect an external database.
      if (isset($view->base_database)) {
        views_pgsql_set_active($view->base_database);
        $external = TRUE;
      }

      $start = views_microtime();

      if ($this->pager->use_count_query() || !empty($view->get_total_rows)) {
        $this->pager->execute_count_query($count_query, $args);
      }

      // Let the pager modify the query to add limits.
      $this->pager->pre_execute($query, $args);

      if (!empty($this->limit) || !empty($this->offset)) {
        // We can't have an offset without a limit, so provide a very large limit instead.
        $limit  = intval(!empty($this->limit) ? $this->limit : 999999);
        $offset = intval(!empty($this->offset) ? $this->offset : 0);
        $result = views_pgsql_query_range($query, $args, $offset, $limit);
      }
      else {
        $result = views_pgsql_query($query, $args);
      }

      $view->result = array();
      while ($item = views_pgsql_fetch_object($result)) {
        $view->result[] = $item;
      }

      $this->pager->post_execute($view->result);

      if ($this->pager->use_pager()) {
        $view->total_rows = $this->pager->get_total_items();
      }

      if ($external) {
        views_pgsql_set_active();
      }
    }
    $view->execute_time = views_microtime() - $start;
  }
}