<?php

/**
 * Returns an array of all the possible blocks a page can have
 *
 */
function mado_get_blocks($all_views = FALSE){
  $blocks_and_views = array();
  $default_to_ignore = array(
    'views' => array(
      'biblio', // Hide Biblio as we have the improved biblio_table view
      'darwincore',
      'darwincorelocation',
      'group',
      'image_gallery',
      'nexus_controlled_state',
      'nexus_free_state',
      'nexus_note',
      'nexus_project',
      'og',
      'og_files',
      'og_ghp_ron',
      'og_members',
      'og_members_block',
      'og_members_faces',
      'og_my',
      'og_mytracker',
      'og_recent_type_term',
      'og_search',
      'og_tracker',
      'og_unread',
      'posts_by_term',
      'profile',
      'taxonomy_term',
      'matrix_page',
      'matrix_editor_specimens',
      'matrix_images'
    ),
    'blocks' => array(
      'blog' => array(
        'all' => TRUE
      ),
      'biblio' => array(
        'all' => TRUE
      ),
      'boost' => array(
        'all' => TRUE
      ),
      'classification' => array(
        'all' => TRUE
      ),
      'comment' => array(
        'all' => TRUE
      ),
      'creativecommons_lite' => array(
        'all' => TRUE
      ),
      'forum' => array(
        'ids' => array(
          '0',
          '1'
        )
      ),
      'glossary' => array(
        'all' => TRUE
      ),
      'gmap_location' => array(
        'all' => TRUE
      ),
      'image' => array(
        'ids' => array(
          '0'
        )
      ),
      'image_attach' => array(
        'all' => TRUE
      ),
      'locale' => array(
        'all' => TRUE
      ),
      'menu' => array(
        'all' => TRUE
      ),
      'node' => array(
        'ids' => array(
          '0'
        )
      ),
      'og' => array(
        'all' => TRUE
      ),
      'og_views' => array(
        'all' => TRUE
      ),
      'of_forum' => array(
        'all' => TRUE
      ),
      'og_user_roles' => array(
        'all' => TRUE
      ),
      'poll' => array(
        'all' => TRUE
      ),
      'print' => array(
        'ids' => array(
          '1'
        )
      ),
      'profile' => array(
        'all' => TRUE
      ),
      'scratchpadify' => array(
        'all' => TRUE
      ),
      'search' => array(
        'ids' => array(
          '0'
        )
      ),
      'sharethis' => array(
        'all' => TRUE
      ),
      'simplenews' => array(
        'all' => TRUE
      ),
      'statistics' => array(
        'all' => TRUE
      ),
      'system' => array(
        'all' => TRUE
      ),
      'user' => array(
        'all' => TRUE
      ),
      'views' => array(
        'ids' => array(
          'og_members_block-block',
          'og_my-block_1',
          'og_files-block_1',
          'og_members_block-block_1'
        )
      )
    )
  );
  foreach(array_keys(node_get_types('names')) as $name){
    $default_to_ignore['views'][] = 'slickgrid_' . $name;
  }
  $views_blocks_to_ignore = variable_get('mado_ignore_views_and_blocks', $default_to_ignore);
  foreach(views_get_all_views() as $view){
    $add_view = FALSE;
    if((($view->display['default']->display_options['arguments']['tid'] || $view->display['default']->display_options['arguments']['term_node_tid_depth']) || $all_views) && !in_array($view->name, $views_blocks_to_ignore['views'])){
      $add_view = TRUE;
    } else {
      if(isset($view->display['default']->display_options['arguments']) && is_array($view->display['default']->display_options['arguments'])){
        foreach($view->display['default']->display_options['arguments'] as $arg_id => $arg){
          if($arg['validate_type'] == 'taxonomy_term' && $arg['validate_argument_type'] == 'tid'){
            $add_view = TRUE;
            break;
          }
        }
      }
    }
    if($add_view){      
      $blocks_and_views['view-' . $view->name] = array(
        '#type' => 'view',
        '#name' => $view->name,
        '#scroll' => TRUE,
        '#css' => 'mado_block_2'
      );
    }
  }
  foreach(module_implements('block') as $module){
    if(isset($views_blocks_to_ignore['blocks'][$module]['all']) && $views_blocks_to_ignore['blocks'][$module]['all']){
      continue;
    }
    foreach(array_keys(call_user_func($module . '_block', 'list')) as $id){
      if(isset($views_blocks_to_ignore['blocks'][$module]['ids']) && in_array($id, $views_blocks_to_ignore['blocks'][$module]['ids'])){
        continue;
      }
      $blocks_and_views['block-' . $module . '/' . $id] = array(
        '#type' => 'block',
        '#name' => "$module/$id",
        '#scroll' => TRUE,
        '#css' => 'mado_block_2'
      );
    }
  }
  $blocks_and_views['divider'] = array(
    '#type' => 'divider'
  );
  return $blocks_and_views;
}

/**
 * Get the theme that this vocabulary is set to use.  Returns false if this 
 * vocabulary should use the standard layout.
 */
function mado_get_theme($vid, $tid = false){
  $mado_theme_cache = cache_get("mado-$vid-$tid");
  if($mado_theme_cache){
    return $mado_theme_cache->data;
  }
  // Check if this vocabulary is set to use Mado
  $mado_vocabularies = variable_get('mado_vocabularies', array());
  if(!isset($mado_vocabularies[$vid]) || !$mado_vocabularies[$vid]){return FALSE;}
  // try and get the blocks
  $blocks = db_fetch_array(db_query("SELECT blocks FROM {mado} WHERE identifier = '%s' OR identifier = '%s' ORDER BY identifier DESC", $vid, "$vid/$tid"));
  if($blocks){
    $blocks = unserialize(array_pop($blocks));
  }else{
    $blocks = array();
  }
  $additional_blocks = mado_get_blocks();
  if(!$blocks){return $additional_blocks;}
  foreach($additional_blocks as $key => $additional_block){
    if(!isset($blocks[$key])){
      $blocks[$key] = $additional_block;
      $blocks[$key]['#hidden'] = TRUE;
    }
  }
  // Cache the data for at least a day.
  cache_set("mado-$vid-$tid", $blocks, 'cache', time()+86400);
  return $blocks;
}

/**
 * Get the theme for the front page.
 */
function mado_get_theme_front_page(){
  // try and get the blocks
  $blocks = db_fetch_array(db_query("SELECT blocks FROM {mado} WHERE identifier = 'frontpage'"));
  if($blocks){
    $blocks = unserialize(array_pop($blocks));
  }else{
    $blocks = array();
  }
  $additional_blocks = mado_get_blocks(TRUE); // True to not filter by views with term arguments
  if(!$blocks){return $additional_blocks;}
  foreach($additional_blocks as $key => $additional_block){
    if(!isset($blocks[$key])){
      $blocks[$key] = $additional_block;
      $blocks[$key]['#hidden'] = TRUE;
    }
  }
  return $blocks;
}