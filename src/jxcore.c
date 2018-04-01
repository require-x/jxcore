#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <uv.h>
#include <jerryscript.h>
#include <jerryscript-ext/handler.h>

#include <console.h>
#include <timers.h>

void print_string_form(jerry_value_t val) {
  jerry_value_t str_val = jerry_value_to_string (val);
  jerry_size_t str_size = jerry_get_string_size (str_val);

  jerry_char_t *str_buf = malloc(str_size + 1);
  jerry_string_to_char_buffer (str_val, str_buf, str_size);
  str_buf[str_size] = 0;

  jerry_release_value (str_val);

  printf ("%s\n", str_buf);

  free(str_buf);
}

void init() {
  jerry_init (JERRY_INIT_EMPTY);

  jerry_value_t global_object = jerry_get_global_object();
  jerry_value_t window = jerry_create_object();
  jerry_value_t global = jerry_create_object();

  // setup_buffers(global, window);
  setup_console(global_object, window, global);
  setup_timers(global_object, window, global);

  jerry_value_t prop_name = jerry_create_string((const jerry_char_t *)"window");
  jerry_set_property(global_object, prop_name, window);
  jerry_release_value(prop_name);

  prop_name = jerry_create_string((const jerry_char_t *)"global");
  jerry_set_property(global_object, prop_name, global);
  jerry_release_value(prop_name);

  jerry_release_value(global);
  jerry_release_value(window);
  jerry_release_value(global_object);
}

void cleanup() {
  jerry_cleanup();
}

jerry_value_t get(char *name, int64_t ptr) {
  jerry_value_t obj;
  if (ptr > -1) {
    obj = (jerry_value_t)ptr;
  } else {
    obj = jerry_get_global_object();
  }

  jerry_value_t jname = jerry_create_string(name);

  jerry_value_t ret = jerry_get_property(obj, jname);

  jerry_release_value(jname);

  if (ptr < 0) {
    jerry_release_value(obj);
  }

  return ret;
}

jerry_value_t run(char *script, size_t length) {
  jerry_value_t ret = jerry_eval (script, length, false);

  if (jerry_value_has_error_flag(ret)) {
    jerry_value_clear_error_flag(&ret);
    print_string_form(ret);
    jerry_release_value(ret);
    exit(1);
  }

  return ret;
}
