#include <jerryscript.h>
#include <jerryscript-ext/handler.h>

void setup_console(jerry_value_t global_object, jerry_value_t window, jerry_value_t global) {
  jerry_value_t console = jerry_create_object();

  jerry_value_t prop_name = jerry_create_string((const jerry_char_t *)"log");
  jerry_value_t prop_value = jerry_create_external_function(jerryx_handler_print);
  jerry_set_property(console, prop_name, prop_value);
  jerry_release_value(prop_name);
  jerry_release_value(prop_value);

  prop_name = jerry_create_string((const jerry_char_t *)"console");
  jerry_set_property(global_object, prop_name, console);
  jerry_set_property(window, prop_name, console);
  jerry_set_property(global, prop_name, console);
  jerry_release_value(prop_name);
  jerry_release_value(console);
}