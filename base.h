/* Copyright 2024 The Kingsoft's modern-string Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "ks_type_traits.h"

using uint = unsigned int;
using char8_t = unsigned char; //needed until c++20
using WCHAR = std::conditional<sizeof(wchar_t) == 2, wchar_t, unsigned short>::type;

// #ifndef __X
// #   define __X(str)   ((const WCHAR*)(u##str))
// #endif
// #ifndef __Xc
// #   define __Xc(ch)   ((WCHAR)(u##ch))
// #endif


#ifndef _DEBUG
#	if !defined(NDEBUG)
#		define _DEBUG
#	endif
#endif


#ifndef ASSERT
#	ifdef _DEBUG
#		if defined(_MSC_VER)
#			include <crtdbg.h>
#			define ASSERT _ASSERT
#		else
#			include <assert.h>
#			define ASSERT assert
#		endif
#	else
#		define ASSERT(x)  ((void)(0))
#	endif
#endif


#ifndef _DECL_EXPORT
#	if defined(_MSC_VER)
#		define _DECL_EXPORT __declspec(dllexport)
#	elif defined(__GNUC__)
#		define _DECL_EXPORT __attribute__((visibility("default")))
#	else
#		error how to decl-export?
#	endif
#endif

#ifndef _DECL_IMPORT
#	if defined(_MSC_VER)
#		define _DECL_IMPORT __declspec(dllimport)
#	elif defined(__GNUC__)
#		define _DECL_IMPORT __attribute__((visibility("default")))
#	else
#		error how to decl-import?
#	endif
#endif


//MODERN_STRING_API定义
//#ifdef MODERN_STRING_EXPORTS
//#    define MODERN_STRING_API _DECL_EXPORT
//#else
//#    define MODERN_STRING_API _DECL_IMPORT
//#endif

#define MODERN_STRING_API 
