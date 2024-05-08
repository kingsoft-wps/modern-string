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

#include "ks_string.h"

namespace ks_string_util {
	// note:
	// pos is IN & OUT param, optional.
	// parsing will begin from *pos, and udpate *pos after finished.
	// if pos is nullptr, like that *pos equals 0.
	// if error, *pos will not be changed.

	//parse int ...
	MODERN_STRING_API
	int parse_int(const ks_string_view& str_view, int def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	int parse_int(const ks_wstring_view& str_view, int def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	int8_t parse_int8(const ks_string_view& str_view, int8_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	int8_t parse_int8(const ks_wstring_view& str_view, int8_t def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	int16_t parse_int16(const ks_string_view& str_view, int16_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	int16_t parse_int16(const ks_wstring_view& str_view, int16_t def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	int32_t parse_int32(const ks_string_view& str_view, int32_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	int32_t parse_int32(const ks_wstring_view& str_view, int32_t def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	int64_t parse_int64(const ks_string_view& str_view, int64_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	int64_t parse_int64(const ks_wstring_view& str_view, int64_t def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	intptr_t parse_intptr(const ks_string_view& str_view, intptr_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	intptr_t parse_intptr(const ks_wstring_view& str_view, intptr_t def_value = 0, size_t* pos = nullptr, int base = 0);

	//parse uint ...
	MODERN_STRING_API
	uint parse_uint(const ks_string_view& str_view, uint def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	uint parse_uint(const ks_wstring_view& str_view, uint def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	uint8_t parse_uint8(const ks_string_view& str_view, uint8_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	uint8_t parse_uint8(const ks_wstring_view& str_view, uint8_t def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	uint16_t parse_uint16(const ks_string_view& str_view, uint16_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	uint16_t parse_uint16(const ks_wstring_view& str_view, uint16_t def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	uint32_t parse_uint32(const ks_string_view& str_view, uint32_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	uint32_t parse_uint32(const ks_wstring_view& str_view, uint32_t def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	uint64_t parse_uint64(const ks_string_view& str_view, uint64_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	uint64_t parse_uint64(const ks_wstring_view& str_view, uint64_t def_value = 0, size_t* pos = nullptr, int base = 0);

	MODERN_STRING_API
	uintptr_t parse_uintptr(const ks_string_view& str_view, uintptr_t def_value = 0, size_t* pos = nullptr, int base = 0);
	MODERN_STRING_API
	uintptr_t parse_uintptr(const ks_wstring_view& str_view, uintptr_t def_value = 0, size_t* pos = nullptr, int base = 0);

	//parse float ...
	MODERN_STRING_API
	float parse_float(const ks_string_view& str_view, float def_value = 0.0, size_t* pos = nullptr);
	MODERN_STRING_API
	float parse_float(const ks_wstring_view& str_view, float def_value = 0.0, size_t* pos = nullptr);

	MODERN_STRING_API
	double parse_double(const ks_string_view& str_view, double def_value = 0.0, size_t* pos = nullptr);
	MODERN_STRING_API
	double parse_double(const ks_wstring_view& str_view, double def_value = 0.0, size_t* pos = nullptr);

	//parse bool ...
	MODERN_STRING_API
	bool parse_bool(const ks_string_view& str_view, bool def_value = false, size_t* pos = nullptr);
	MODERN_STRING_API
	bool parse_bool(const ks_wstring_view& str_view, bool def_value = false, size_t* pos = nullptr);

}
