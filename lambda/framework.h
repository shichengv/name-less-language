#pragma once

// #define WINDOWS
#define LINUX

// Visual Studio ����ʹ��
// #define DEBUG



// C++ ͷ�ļ�
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

// C ͷ�ļ�
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef WINDOWS
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

// �����ڴ�й¶���
#define _FIND_MEMORY_LEAKS_USING_CRT

#ifdef _FIND_MEMORY_LEAKS_USING_CRT

#define _CRTDBG_MAP_ALLOC
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




