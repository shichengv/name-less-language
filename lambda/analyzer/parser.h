#pragma once

#include "../framework.h"
#include "./token/token.h"

void parser_init();
void parser_clean();
void parse_toplevel(Token* prog_root);
