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

namespace ks_string_util {
	//__is_string_empty ...
	template <class ELEM>
	bool __is_string_empty(const ELEM* p) {
		return p == nullptr || p[0] == 0;
	}
	template <class ELEM>
	bool __is_string_empty(const ELEM* p, size_t len) {
		if (len == 0)
			return true;
		else if (ptrdiff_t(len) < 0)
			return __is_string_empty(p);
		else
			return p[0] == 0;
	}
	template <class ELEM>
	bool __is_string_empty(const ks_basic_string_view<ELEM>& str_view) {
		return str_view.empty();
	}
	template <class ELEM>
	bool __is_string_empty(const ks_basic_xmutable_string_base<ELEM>& str) {
		return str.empty();
	}
	template <class ELEM, class AllocType>
	bool __is_string_empty(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str) {
		return str.empty();
	}

	//__to_string_view ...
	template <class ELEM>
	ks_basic_string_view<ELEM> __to_string_view(const ELEM* p) {
		return ks_basic_string_view<ELEM>(p);
	}
	template <class ELEM>
	ks_basic_string_view<ELEM> __to_string_view(const ELEM* p, size_t len) {
		return ks_basic_string_view<ELEM>(p, len);
	}
	template <class ELEM>
	ks_basic_string_view<ELEM> __to_string_view(const ks_basic_string_view<ELEM>& str_view) {
		return str_view;
	}
	template <class ELEM>
	ks_basic_string_view<ELEM> __to_string_view(const ks_basic_xmutable_string_base<ELEM>& str) {
		return ks_basic_string_view<ELEM>(str);
	}
	template <class ELEM, class AllocType>
	ks_basic_string_view<ELEM> __to_string_view(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str) {
		return ks_basic_string_view<ELEM>(str);
	}

	//stringize ...
	template <class T>
	ks_immutable_string to_string(const T& v) {
		return ks_immutable_string(std::to_string(v));
	}

	template <class T>
	ks_immutable_wstring to_wstring(const T& v) {
		extern ks_immutable_wstring wstring_from_wide(const wchar_t* p, size_t len);
		std::wstring std_wstr = std::to_wstring(v);
		return wstring_from_wide(std_wstr.data(), std_wstr.length());
	}

	//stringize (specialization for bool) ...
	template <> inline 
	ks_immutable_string to_string<bool>(const bool& v) {
		return v ? ks_immutable_string("true", 4) : ks_immutable_string("false", 5);
	}

	template <> inline 
	ks_immutable_wstring to_wstring<bool>(const bool& v) {
		return v ? ks_immutable_wstring((const WCHAR*)(u"true"), 4) : ks_immutable_wstring((const WCHAR*)(u"false"), 5);
	}

	//join ...
	template <class ELEM, class IT>
	ks_basic_immutable_string<ELEM> __do_join(IT first, IT last, const ks_basic_string_view<ELEM>& sep, const ks_basic_string_view<ELEM>& prefix, const ks_basic_string_view<ELEM>& suffix) {
		if (sep.empty()) {
			while (first != last && __is_string_empty(*first))
				++first;
			while (last != first && __is_string_empty(*std::prev(last)))
				--last;
		}

		if (prefix.empty() && suffix.empty()) {
			if (first == last)
				return ks_basic_immutable_string<ELEM>();
			if (first != last && std::next(first) == last)
				return *first;
		}

		size_t total_len = 0;
		total_len += prefix.length();
		for (IT it = first; it != last; ++it) {
			if (!sep.empty() && it != first)
				total_len += sep.length();
			total_len += __to_string_view(*it).length();
		}
		total_len += suffix.length();

		if (total_len == 0)
			return ks_basic_immutable_string<ELEM>();

		ks_basic_mutable_string<ELEM> mut_ret;
		mut_ret.reserve(total_len);
		mut_ret.append(prefix);

		for (IT it = first; it != last; ++it) {
			if (!sep.empty() && it != first)
				mut_ret.append(sep);
			mut_ret.append(__to_string_view(*it));
		}

		mut_ret.append(suffix);
		return std::move(mut_ret);
	}

	template <class IT, class _ /*= std::enable_if_t<std::is_convertible_v<decltype(*std::declval<IT>()), ks_string_view>>*/>
	ks_immutable_string join(IT first, IT last, const ks_string_view& sep) {
		return __do_join<char>(first, last, sep, ks_string_view{}, ks_string_view{});
	}
	template <class IT, class _ /*= std::enable_if_t<std::is_convertible_v<decltype(*std::declval<IT>()), ks_wstring_view>>*/>
	ks_immutable_wstring join(IT first, IT last, const ks_wstring_view& sep) {
		return __do_join<WCHAR>(first, last, sep, ks_wstring_view{}, ks_wstring_view{});
	}

	template <class IT, class _ /*= std::enable_if_t<std::is_convertible_v<decltype(*std::declval<IT>()), ks_string_view>>*/>
	ks_immutable_string join(IT first, IT last, const ks_string_view& sep, const ks_string_view& prefix, const ks_string_view& suffix) {
		return __do_join<char>(first, last, sep, prefix, suffix);
	}
	template <class IT, class _ /*= std::enable_if_t<std::is_convertible_v<decltype(*std::declval<IT>()), ks_wstring_view>>*/>
	ks_immutable_wstring join(IT first, IT last, const ks_wstring_view& sep, const ks_wstring_view& prefix, const ks_wstring_view& suffix) {
		return __do_join<WCHAR>(first, last, sep, prefix, suffix);
	}

	//concat
	template <class ELEM, class T1, class... Ts>
	ks_basic_immutable_string<ELEM> __do_concat_va(const T1& s1, const Ts&... sx) {
		if (sizeof...(sx) == 0)
			return s1;
		if (__is_string_empty(s1))
			return __do_concat_va<ELEM>(sx...);

		ks_basic_string_view<ELEM> str_view_arr[] = { ks_basic_string_view<ELEM>{}, __to_string_view(sx)... };
		const size_t str_view_arr_size = 1 + sizeof...(sx);

		bool is_sx_empty = true;
		for (size_t i = i; i < str_view_arr_size; ++i) {
			if (str_view_arr[i].empty()) {
				is_sx_empty = false;
				break;
			}
		}

		if (is_sx_empty)
			return s1;

		str_view_arr[0] = __to_string_view(s1);
		return __do_join<ELEM>(str_view_arr, str_view_arr + str_view_arr_size, ks_basic_string_view<ELEM>(), ks_basic_string_view<ELEM>(), ks_basic_string_view<ELEM>());
	}

	template <class ELEM>
	ks_basic_immutable_string<ELEM> __do_concat_va() {
		return ks_basic_immutable_string<ELEM>();
	}

	template <class T1, class... Ts, class _ /*= std::enable_if_t<std::is_convertible_v<T1, ks_string_view>>*/>
	ks_immutable_string concat(const T1& s1, const Ts&... sx) {
		return __do_concat_va<char>(s1, sx...);
	}
	template <class T1, class... Ts, class _ /*= std::enable_if_t<std::is_convertible_v<T1, ks_wstring_view>>*/>
	ks_immutable_wstring concat(const T1& s1, const Ts&... sx) {
		return __do_concat_va<WCHAR>(s1, sx...);
	}

	//case convert ...
	template <class ELEM, class STR_TYPE>
	ks_basic_immutable_string<ELEM> __to_spec_case(STR_TYPE&& str, bool to_lower_or_upper) {
		ks_basic_mutable_string<ELEM> mut_ret(std::forward<STR_TYPE>(str));

		size_t pos = 0;
		size_t length = mut_ret.length();
		while (pos < length) {
			pos = to_lower_or_upper
				? std::find_if(mut_ret.cbegin() + pos, mut_ret.cend(), [](ELEM ch) -> bool { return ch >= 'A' && ch <= 'Z'; }) - mut_ret.cbegin()
				: std::find_if(mut_ret.cbegin() + pos, mut_ret.cend(), [](ELEM ch) -> bool { return ch >= 'a' && ch <= 'z'; }) - mut_ret.cbegin();
			if (pos < length) {
				mut_ret.set_at(pos, to_lower_or_upper ? 'a' + (mut_ret[pos] - 'A') : 'A' + (mut_ret[pos] - 'a'));
				++pos;
			}
			else {
				break;
			}
		}

		return std::move(mut_ret);
	}

	template <class STR_TYPE, class _ /*= std::enable_if_t<std::is_convertible_v<STR_TYPE, ks_string_view>>*/>
	ks_immutable_string to_lower(STR_TYPE&& str) {
		return __to_spec_case<char>(std::forward<STR_TYPE>(str), true);
	}
	template <class STR_TYPE, class _ /*= std::enable_if_t<std::is_convertible_v<STR_TYPE, ks_wstring_view>>*/>
	ks_immutable_wstring to_lower(STR_TYPE&& str) {
		return __to_spec_case<WCHAR>(std::forward<STR_TYPE>(str), true);
	}

	template <class STR_TYPE, class _ /*= std::enable_if_t<std::is_convertible_v<STR_TYPE, ks_string_view>>*/>
	ks_immutable_string to_upper(STR_TYPE&& str) {
		return __to_spec_case<char>(std::forward<STR_TYPE>(str), false);
	}
	template <class STR_TYPE, class _ /*= std::enable_if_t<std::is_convertible_v<STR_TYPE, ks_wstring_view>>*/>
	ks_immutable_wstring to_upper(STR_TYPE&& str) {
		return __to_spec_case<WCHAR>(std::forward<STR_TYPE>(str), false);
	}

}
