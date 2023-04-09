#pragma once

#ifdef __cplusplus
extern "C" {
#endif

unsigned global = 0;

void hello();

unsigned add(unsigned, unsigned);

unsigned add_file_side_effect(unsigned a, unsigned b);

unsigned add_global_side_effect(unsigned a, unsigned b);

unsigned add_printf_side_effect(unsigned a, unsigned b);

unsigned add_ptr_side_effect(unsigned a, unsigned b, unsigned *ptr);

void echo(const char *str);

void call_cb(void (*cb)(const char *str));

#ifdef __cplusplus
}
#endif
