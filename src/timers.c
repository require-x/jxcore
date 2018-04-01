#include <stdlib.h>
#include <stdio.h>
#include <uv.h>
#include <jerryscript.h>

void print_string_form(jerry_value_t val);

typedef struct {
  jerry_value_t *args_p;
  jerry_length_t args_cnt;
} timer_data_t;

typedef struct timer_array_t {
  struct timer_array_t *n;
  double i;
  uv_timer_t *t;
} timer_array_t;

timer_array_t *array = NULL;

void timer_callback(uv_timer_t *timer) {
  timer_data_t *data = (timer_data_t *)timer->data;

  if (jerry_value_is_function(data->args_p[0])) {
    jerry_value_t *args_p = NULL;
    jerry_length_t args_cnt = 0;
    if (data->args_cnt > 2) {
      args_cnt = data->args_cnt - 2;
      args_p = (jerry_value_t *)malloc(args_cnt * sizeof(jerry_value_t));
    }

    for (int i = 2; i < data->args_cnt; i++) {
      args_p[i - 2] = data->args_p[i];
    }

    jerry_value_t this_val = jerry_create_undefined();
    jerry_value_t ret_val = jerry_call_function(data->args_p[0], this_val, args_p, args_cnt);
    jerry_release_value(this_val);

    for (int i = 0; i < data->args_cnt; i++) {
      jerry_release_value(data->args_p[i]);
    }

    if (jerry_value_has_error_flag(ret_val)) {
      jerry_value_clear_error_flag(&ret_val);
      print_string_form(ret_val);
      exit(1);
    }

    jerry_release_value(ret_val);
  } else {
    jerry_value_t str_val = jerry_value_to_string(data->args_p[0]);
    jerry_size_t str_size = jerry_get_string_size(str_val);

    jerry_char_t *str_buf = malloc(str_size);
    jerry_string_to_char_buffer(str_val, str_buf, str_size);

    jerry_release_value(str_val);

    jerry_value_t ret_val = jerry_eval(str_buf, str_size, false);

    free(str_buf);

    if (jerry_value_has_error_flag(ret_val)) {
      jerry_value_clear_error_flag(&ret_val);
      print_string_form(ret_val);
      exit(1);
    }

    jerry_release_value(ret_val);
  }
}

jerry_value_t setTimeout(const jerry_value_t func_value, /**< function object */
                         const jerry_value_t this_value, /**< this arg */
                         const jerry_value_t *args_p, /**< function arguments */
                         const jerry_length_t args_cnt) /**< number of function arguments */ 
{
  if (args_cnt == 0) {
    jerry_value_t error = jerry_create_error(JERRY_ERROR_TYPE, (const jerry_char_t *)"1 argument required, but only 0 present.");
    jerry_value_set_error_flag(&error);
    return error;
  }

  double timeout = 0;
  if (args_cnt > 1) {
    jerry_value_t number_value = jerry_value_to_number(args_p[1]);
    timeout = jerry_get_number_value(number_value);
    jerry_release_value(number_value);
  }

  timer_data_t *data = (timer_data_t *)malloc(sizeof(timer_data_t));
  data->args_p = (jerry_value_t *)malloc(args_cnt * sizeof(jerry_value_t));
  data->args_cnt = args_cnt;

  for (int i = 0; i < args_cnt; i++) {
    data->args_p[i] = jerry_acquire_value(args_p[i]);
  }

  uv_timer_t *timer = (uv_timer_t *)malloc(sizeof(uv_timer_t));
  timer->data = (void *)data;
  uv_timer_init(uv_default_loop(), timer);

  double id = 0;
  if (array == NULL) {
    array = (timer_array_t *)malloc(sizeof(timer_array_t));
    array->n = NULL;
    array->i = 0;
    array->t = timer;
  } else {
    timer_array_t *n = array;
    while (n->n != NULL) {
      n = n->n;
    }

    n->n = (timer_array_t *)malloc(sizeof(timer_array_t));
    n->n->i = n->i + 1;
    n->n->t = timer;
    id = n->n->i;
  }

  uv_timer_start(timer, timer_callback, timeout, 0);

  return jerry_create_number(id);
}

jerry_value_t setInterval(const jerry_value_t func_value, /**< function object */
                          const jerry_value_t this_value, /**< this arg */
                          const jerry_value_t *args_p, /**< function arguments */
                          const jerry_length_t args_cnt) /**< number of function arguments */ 
{
  if (args_cnt == 0) {
    jerry_value_t error = jerry_create_error(JERRY_ERROR_TYPE, (const jerry_char_t *)"1 argument required, but only 0 present.");
    jerry_value_set_error_flag(&error);
    return error;
  }

  double interval = 0;
  if (args_cnt > 1) {
    jerry_value_t number_value = jerry_value_to_number(args_p[1]);
    interval = jerry_get_number_value(number_value);
    jerry_release_value(number_value);
  }

  if (interval < 10) {
    interval = 10;
  }

  timer_data_t *data = (timer_data_t *)malloc(sizeof(timer_data_t));
  data->args_p = (jerry_value_t *)malloc(args_cnt * sizeof(jerry_value_t));
  data->args_cnt = args_cnt;

  for (int i = 0; i < args_cnt; i++) {
    data->args_p[i] = jerry_acquire_value(args_p[i]);
  }

  uv_timer_t *timer = (uv_timer_t *)malloc(sizeof(uv_timer_t));
  timer->data = (void *)data;
  uv_timer_init(uv_default_loop(), timer);

  double id = 0;
  if (array == NULL) {
    array = (timer_array_t *)malloc(sizeof(timer_array_t));
    array->n = NULL;
    array->i = 0;
    array->t = timer;
  } else {
    timer_array_t *n = array;
    while (n->n != NULL) {
      n = n->n;
    }

    n->n = (timer_array_t *)malloc(sizeof(timer_array_t));
    n->n->i = n->i + 1;
    n->n->t = timer;
    id = n->n->i;
  }

  uv_timer_start(timer, timer_callback, interval, interval);

  return jerry_create_number(id);
}

jerry_value_t clear_timer(const jerry_value_t func_value, /**< function object */
                         const jerry_value_t this_value, /**< this arg */
                         const jerry_value_t *args_p, /**< function arguments */
                         const jerry_length_t args_cnt) /**< number of function arguments */ 
{
  if (args_cnt < 1 || !jerry_value_is_number(args_p[0]) || array == NULL) {
    return jerry_create_undefined();
  }

  double id = jerry_get_number_value(args_p[0]);

  timer_array_t *n = array;
  while (n->n != NULL && n->i < id) {
    n = n->n;
  }

  if (n->i != id) {
    return jerry_create_undefined();
  }

  uv_timer_stop(n->t);

  return jerry_create_undefined();
}

void setup_timers(jerry_value_t global_object, jerry_value_t window, jerry_value_t global) {
  jerry_value_t prop_name = jerry_create_string((const jerry_char_t *)"setTimeout");
  jerry_value_t prop_value = jerry_create_external_function(setTimeout);
  jerry_set_property(global_object, prop_name, prop_value);
  jerry_set_property(window, prop_name, prop_value);
  jerry_set_property(global, prop_name, prop_value);
  jerry_release_value(prop_name);
  jerry_release_value(prop_value);

  prop_name = jerry_create_string((const jerry_char_t *)"setInterval");
  prop_value = jerry_create_external_function(setInterval);
  jerry_set_property(global_object, prop_name, prop_value);
  jerry_set_property(window, prop_name, prop_value);
  jerry_set_property(global, prop_name, prop_value);
  jerry_release_value(prop_name);
  jerry_release_value(prop_value);

  prop_name = jerry_create_string((const jerry_char_t *)"clearTimeout");
  prop_value = jerry_create_external_function(clear_timer);
  jerry_set_property(global_object, prop_name, prop_value);
  jerry_set_property(window, prop_name, prop_value);
  jerry_set_property(global, prop_name, prop_value);
  jerry_release_value(prop_name);

  prop_name = jerry_create_string((const jerry_char_t *)"clearInterval");
  jerry_set_property(global_object, prop_name, prop_value);
  jerry_set_property(window, prop_name, prop_value);
  jerry_set_property(global, prop_name, prop_value);
  jerry_release_value(prop_name);
  jerry_release_value(prop_value);
}
