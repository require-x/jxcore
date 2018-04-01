#ifndef JXCORE_INCLUDE_JXCORE_H_
#define JXCORE_INCLUDE_JXCORE_H_

#include <jerryscript.h>

#ifdef __cplusplus
extern "C"{
#endif 

void init();
void cleanup();
jerry_value_t get(char *name, int64_t ptr);
jerry_value_t run(char *script, size_t length);

void print_string_form(jerry_value_t val);

#ifdef __cplusplus
}
#endif

#endif  // JXCORE_INCLUDE_JXCORE_H_