<?php

/**
 * Class view callback.
 */
function cu_course_view($course) {
  //dsm($course);
  drupal_set_title(entity_label('cu_course', $course));
  return entity_view('cu_course', array(entity_id('cu_course', $course) => $course), 'full');
}

function cu_course_display($course) {
  // If there is a class number in the url, highlight that class
  if ($_GET['class_id'] && is_numeric($_GET['class_id'])) {
    $class_number = $_GET['class_id'];

    // Query course id table
    $query = db_select('field_data_field_class_crse_id', 'courseid');
    $query->fields('courseid', array('entity_id'));

    // Join to get class entities that match course id
    $query->join('cu_class', 'class', 'class.class_id = courseid.entity_id');
    $query->fields('class', array('class_id'));

    // Join to get specific class from url
    $query->join('field_data_field_class_class_nbr', 'class_nbr', 'class_nbr.entity_id = courseid.entity_id');
    $query->fields('class_nbr', array('field_class_class_nbr_value'));
    $query->condition('class_nbr.field_class_class_nbr_value', $class_number, '=');

    // Get only class entities
    $query->condition('class.type', 'class', '=');
    $query->condition('courseid.field_class_crse_id_value', $course->course_id, '=');
    $query->orderBy('courseid.field_class_crse_id_value', 'DESC');
    $highlight_class_ids = $query->execute()->fetchCol();
  }


  // Query course id table
  $query = db_select('field_data_field_class_crse_id', 'courseid');
  $query->fields('courseid', array('entity_id'));

  // Join to get class entities that match course id
  $query->join('cu_class', 'class', 'class.class_id = courseid.entity_id');
  $query->fields('class', array('class_id'));

  // Get only class entities
  $query->condition('class.type', 'class', '=');
  $query->condition('courseid.field_class_crse_id_value', $course->course_id, '=');
  $query->orderBy('courseid.field_class_crse_id_value', 'DESC');
  $class_ids = $query->execute()->fetchCol();



  // Put loaded class entities into an array for viewing
  $classes = array();
  foreach ($class_ids as $class_id) {
    $classes[$class_id] = entity_load_single('cu_class', $class_id);
  }
  // Remove highlighted class from list
  if (!empty($highlight_class_ids)) {
    $current_class = current($highlight_class_ids);
    unset($classes[$current_class]);
  }

  $output = array();
  $output['classes'] = entity_view('cu_class', $classes, 'short');
  $output['classes']['#weight'] = 100;
  $output['classes']['#prefix'] = '<div class="course-classes">';
  $output['classes']['#suffix'] = '</div>';
  $class = current($output['classes']['cu_class']);
  $output['description'] = $class['field_class_ssr_descrlong'];
  $output['description']['#prefix'] = '<div class="course-description">';
  $output['description']['#suffix'] = '</div>';

  $output['#prefix'] = '<div class="course-class-list">';
  $output['#suffix'] = '</div>';

  // If there is a highlighted class, display page differently
  if (!empty($highlight_class_ids)) {
    $classes = array();
    foreach ($highlight_class_ids as $class_id) {
      $classes[$class_id] = entity_load_single('cu_class', $class_id);
    }
    // Highlighted class
    $output['classes_highlighted'] = entity_view('cu_class', $classes, 'short');
    $output['classes_highlighted']['#weight'] = 99;
    $output['classes_highlighted']['#prefix'] = '<div class="course-classes-highlighted">';
    $output['classes_highlighted']['#suffix'] = '</div>';

    // Put the rest of the classes in an expandable section
    $output['classes']['#prefix'] = '<div class="course-classes-all"><a href="#other-classes" id="other-classes-trigger"><i class="fa fa-plus-square"></i> Other Classes/Sections</a><div id="other-classes">';
    $output['classes']['#suffix'] = '</div></div>';
  }
  return $output;
}

function cu_course_title($course) {
  $title = array();
  $title[] = $course->subject;
  $title[] = $course->catalog_nbr;
  $title[] = $course->title;
  return join(' ', $title);
}