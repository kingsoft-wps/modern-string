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
	//stringize ...
	template <class T>
	MODERN_STRING_INLINE_API
	ks_immutable_string to_string(const T& v);
	template <class T>
	MODERN_STRING_INLINE_API
	ks_immutable_wstring to_wstring(const T& v);

	//join ...
	template <class IT, class _ = std::enable_if_t<std::is_convertible_v<decltype(*std::declval<IT>()), ks_string_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_string join(IT first, IT last, const ks_string_view& sep);
	template <class IT, class _ = std::enable_if_t<std::is_convertible_v<decltype(*std::declval<IT>()), ks_wstring_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_wstring join(IT first, IT last, const ks_wstring_view& sep);

	template <class IT, class _ = std::enable_if_t<std::is_convertible_v<decltype(*std::declval<IT>()), ks_string_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_string join(IT first, IT last, const ks_string_view& sep, const ks_string_view& prefix, const ks_string_view& suffix);
	template <class IT, class _ = std::enable_if_t<std::is_convertible_v<decltype(*std::declval<IT>()), ks_wstring_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_wstring join(IT first, IT last, const ks_wstring_view& sep, const ks_wstring_view& prefix, const ks_wstring_view& suffix);

	//concat ...
	template <class T1, class... Ts, class _ = std::enable_if_t<std::is_convertible_v<T1, ks_string_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_string concat(const T1& s1, const Ts&... sx);
	template <class T1, class... Ts, class _ = std::enable_if_t<std::is_convertible_v<T1, ks_wstring_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_wstring concat(const T1& s1, const Ts&... sx);

	//case convert ...
	template <class STR_TYPE, class _ = std::enable_if_t<std::is_convertible_v<STR_TYPE, ks_string_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_string to_lower(STR_TYPE&& str);
	template <class STR_TYPE, class _ = std::enable_if_t<std::is_convertible_v<STR_TYPE, ks_wstring_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_wstring to_lower(STR_TYPE&& str);

	template <class STR_TYPE, class _ = std::enable_if_t<std::is_convertible_v<STR_TYPE, ks_string_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_string to_upper(STR_TYPE&& str);
	template <class STR_TYPE, class _ = std::enable_if_t<std::is_convertible_v<STR_TYPE, ks_wstring_view>>>
	MODERN_STRING_INLINE_API
	ks_immutable_wstring to_upper(STR_TYPE&& str);

	//icase compare ...
	MODERN_STRING_API
	bool icase_equals(const ks_string_view& left, const ks_string_view& right);
	MODERN_STRING_API
	bool icase_equals(const ks_wstring_view& left, const ks_wstring_view& right);

}

#include "ks_string_util.inl"

#include "ks_string_util_parse.h"
#include "ks_string_util_convert.h"
