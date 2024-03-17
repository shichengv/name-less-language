#pragma once

#define WINDOWS



#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>


#ifdef WINDOWS

#include <io.h>
#include <fcntl.h>

// ÆôÓÃÄÚ´æÐ¹Â¶¼ì²â
#define _FIND_MEMORY_LEAKS_USING_CRT

#ifdef _FIND_MEMORY_LEAKS_USING_CRT

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#endif // _FIND_MEMORY_LEAKS_USING_CRT

#endif // !WINDOWS

#ifdef LINUX

#define DBG_NEW new

#endif // LINUX




