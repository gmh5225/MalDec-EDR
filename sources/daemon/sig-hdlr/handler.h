#pragma once

#define DEFAULT_HANDLER _default_handler

void
_default_handler(int);
int
signal_handler(void (*hdlr)(int));
int
notify_success(void);