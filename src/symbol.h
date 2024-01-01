#pragma once
#include <unordered_map>

extern std::unordered_map<std::string, int> const_val;

enum TYPE {CONST_INT,VAR};
extern std::unordered_map<std::string, TYPE> var_type;

