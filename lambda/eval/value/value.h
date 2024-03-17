#pragma once

#include "value_atom.h"
#include "value_lambda.h"
#include "value_pair.h"

#define VALUE_IS_SELF_EVALUTABLE(flag)				((flag == VALUE_IS_LOGICAL) || (flag == VALUE_IS_STRING) || (flag == VALUE_IS_NUM))
#define DELETE_VALUE(value)							(value->ref == 1 ? delete value : value->ref)
