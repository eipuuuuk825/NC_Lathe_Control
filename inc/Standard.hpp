#pragma once

#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

void print_err(const char *msg);
std::vector<std::string> split(const std::string &str, char sep);