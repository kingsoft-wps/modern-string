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

#if __cplusplus < 202002L
using char8_t = unsigned char; //needed until c++20
#endif

#ifdef _WIN32
using WCHAR = wchar_t;
#else
using WCHAR = unsigned short;  //prefer char16_t than unsigned short
#endif


//#ifndef __Xs
//#   ifdef _WIN32
//#       define __Xs(s)  (L##s)
//#   else
//#       define __Xs(s)  (static_cast<const WCHAR*>(static_cast<const void*>(u##s)))
//#   endif
//#endif
//
//#ifndef __Xc
//#   ifdef _WIN32
//#       define __Xc(c)  (L##c)
//#   else
//#       define __Xc(c)  (static_cast<const WCHAR>(u##c))
//#   endif
//#endif


#ifndef _DEBUG
#	if !defined(NDEBUG)
#		define _DEBUG
#	endif
#endif

#ifndef _ASSERT
#	ifdef _DEBUG
#		if defined(_MSC_VER)
#			include <crtdbg.h>
#		else
#			include <assert.h>
#			define _ASSERT(x)  assert(x)
#		endif
#	else
#			define _ASSERT(x)  ((void)(0))
#	endif
#endif

#ifndef ASSERT
#	define ASSERT(x)  _ASSERT(x)
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


//#ifdef MODERN_STRING_EXPORTS
//#    define MODERN_STRING_API _DECL_EXPORT
//#    define MODERN_STRING_INLINE_API
//#else
//#    define MODERN_STRING_API _DECL_IMPORT
//#    define MODERN_STRING_INLINE_API
//#endif

//the modern-string is a STATIC libaray...
#define MODERN_STRING_API 
#define MODERN_STRING_INLINE_API
