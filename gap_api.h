#pragma once

#define TYPETEXT		0
#define TYPERADIO		1
#define TYPESELECT		2
#define TYPETEXTAREA	3
#define TYPECHECKBOX	4
#define TYPESUBMIT		5

#include <stdbool.h>

void gapInit(const char* chat_id, const char* token);
bool sendText(const char*);