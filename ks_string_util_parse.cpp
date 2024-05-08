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

#include "ks_string_util_parse.h"

namespace ks_string_util {
	template <class ELEM, class CH_PRED>
	static void __do_prepare_buff_for_parse(std::string* buff, const ks_basic_string_view<ELEM>& str_view, size_t len_limit, char trunc_ch, CH_PRED ch_pred) {
		ASSERT(buff->empty());
		if (len_limit > str_view.length())
			len_limit = str_view.length();

		size_t valid_len = 0;
		while (valid_len < len_limit) {
			ELEM ch = str_view[valid_len];
			if (ch > 0 && ch < 128 && ch_pred((char)ch))
				++valid_len;
			else
				break;
		}

		if (valid_len != 0) {
			buff->reserve(valid_len + 1);
			buff->resize(valid_len);
			for (size_t i = 0; i < valid_len; ++i)
				(*buff)[i] = (char)str_view[i];
			if (valid_len < str_view.length() && str_view[valid_len] != 0)
				buff->append(1, trunc_ch); //if trunc, we append a invalid-ch as tail
		}
	}

	static long long __strtointx(const char* ptr, char** endptr, int base, size_t type_size, bool is_signed) {
		ASSERT(type_size <= sizeof(long long));
		ASSERT(endptr != nullptr);
		*endptr = const_cast<char*>(ptr);

		long long parsed_val;
		if (is_signed) {
			if (type_size <= sizeof(long))
				parsed_val = (long long)std::strtol(ptr, endptr, base);
			else
				parsed_val = (long long)std::strtoull(ptr, endptr, base);
		}
		else {
			if (type_size <= sizeof(long))
				parsed_val = (long long)std::strtoul(ptr, endptr, base);
			else
				parsed_val = (long long)std::strtoull(ptr, endptr, base);
		}

		if (*endptr > ptr && errno != ERANGE && type_size < sizeof(long)) {
			//check bits range
			const long long max_legal_value = is_signed ? (long long)(((unsigned long long)1 << (type_size * 8 - 1)) - 1) : ((long long)1 << type_size * 8) - 1;
			const long long min_legal_value = is_signed ? ~max_legal_value : 0;
			if (parsed_val < min_legal_value || parsed_val > max_legal_value) 
				errno = ERANGE;
		}

		return parsed_val;
	}

	template <class INT_TYPE, class ELEM>
	static INT_TYPE __do_parse_intx(const ks_basic_string_view<ELEM>& str_view, INT_TYPE def_value, size_t* pos, int base) {
		static_assert(std::is_integral_v<INT_TYPE>, "the INT_TYPE is invalid");
		if (pos != nullptr && *pos >= str_view.length())
			return def_value;

		ks_basic_string_view<ELEM> str_view2 = pos != nullptr && *pos != 0 ? str_view.substr(*pos, -1) : str_view;

		std::string buff;
		constexpr size_t len_limit = sizeof(INT_TYPE) * 8 + 10;
		if (base != 0 && base <= 10) {
			__do_prepare_buff_for_parse(&buff, str_view2, len_limit, ';', [](char ch) -> bool {
				return (ch >= '0' && ch <= '9')
					|| (ch == '+') || (ch == '-');
				});
		}
		else {
			__do_prepare_buff_for_parse(&buff, str_view2, len_limit, ';', [](char ch) -> bool {
				return (ch >= '0' && ch <= '9')
					|| (ch >= 'a' && ch <= 'z')
					|| (ch >= 'A' && ch <= 'Z')
					|| (ch == '+') || (ch == '-');
				});
		}
		
		const char* ptr = buff.c_str();
		if (ptr[0] == 0)
			return def_value;

		char* endptr = const_cast<char*>(ptr);
		INT_TYPE parsed_value = (INT_TYPE)__strtointx(buff.c_str(), &endptr, base, sizeof(INT_TYPE), std::is_signed_v<INT_TYPE>);
		bool has_error = endptr <= ptr || errno == ERANGE;
		if (has_error)
			return def_value;

		if (pos != nullptr)
			*pos = str_view2.data() + (endptr - ptr) - str_view.data();
		return parsed_value;
	}

	template <class FLOAT_TYPE, class ELEM>
	static FLOAT_TYPE __do_parse_floatx(const ks_basic_string_view<ELEM>& str_view, FLOAT_TYPE def_value, size_t* pos) {
		static_assert(std::is_floating_point_v<FLOAT_TYPE>, "the FLOAT_TYPE is invalid");
		if (pos != nullptr && *pos >= str_view.length())
			return def_value;

		ks_basic_string_view<ELEM> str_view2 = pos != nullptr && *pos != 0 ? str_view.substr(*pos, -1) : str_view;

		std::string buff;
		__do_prepare_buff_for_parse(&buff, str_view2, str_view2.length(), ';', [](char ch) -> bool {
			return (ch >= '0' && ch <= '9')
				|| (ch >= 'a' && ch <= 'z')
				|| (ch >= 'A' && ch <= 'Z')
				|| (ch == '+') || (ch == '-')
				|| (ch == '.');
			});

		const char* ptr = buff.c_str();
		if (ptr[0] == 0)
			return def_value;

		FLOAT_TYPE parsed_val = 0;
		char* endptr = const_cast<char*>(ptr);
		if (std::is_same_v<FLOAT_TYPE, float>)
			parsed_val = (FLOAT_TYPE)std::strtof(ptr, &endptr);
		else if (std::is_same_v<FLOAT_TYPE, double>)
			parsed_val = (FLOAT_TYPE)std::strtod(ptr, &endptr);
		else if (std::is_same_v<FLOAT_TYPE, long double>)
			parsed_val = (FLOAT_TYPE)std::strtold(ptr, &endptr);
		else
			ASSERT(false);

		bool has_error = endptr <= ptr || errno == ERANGE;
		if (has_error)
			return def_value;

		//parse ok
		if (pos != nullptr)
			*pos = str_view2.data() + (endptr - ptr) - str_view.data();
		return parsed_val;
	}

	template <class BOOL_TYPE, class ELEM>
	static BOOL_TYPE __do_parse_boolx(const ks_basic_string_view<ELEM>& str_view, BOOL_TYPE def_value, size_t* pos) {
		static_assert(std::is_same_v<BOOL_TYPE, bool>, "the BOOL_TYPE is invalid");
		if (pos != nullptr && *pos >= str_view.length())
			return def_value;

		ks_basic_string_view<ELEM> str_view2 = pos != nullptr && *pos != 0 ? str_view.substr(*pos, -1) : str_view;

		if (str_view2.empty()) 
			return def_value;

		const WCHAR first_wch = str_view2[0];
		if (first_wch == 'f' || first_wch == 'F') {
			std::string buff;
			__do_prepare_buff_for_parse(&buff, str_view2, 6, ';', [](char ch) -> bool {
				return (ch >= 'a' && ch <= 'z')
					|| (ch >= 'A' && ch <= 'Z');
				});
			ks_string_view buff_view = buff;
			if (buff_view.length() > 5 && buff_view[5] == ';')
				buff_view = buff_view.substr(0, 5);
			bool is_false_literal = buff_view.length() == 5 && (buff_view == "false" || buff_view == "False" || buff_view == "FALSE");
			if (!is_false_literal)
				return def_value;
			if (pos != nullptr)
				*pos = str_view2.data() + 5 - str_view.data();
			return false;
		}
		else if (first_wch == 't' || first_wch == 'T') {
			std::string buff;
			__do_prepare_buff_for_parse(&buff, str_view2, 5, ';', [](char ch) -> bool {
				return (ch >= 'a' && ch <= 'z')
					|| (ch >= 'A' && ch <= 'Z');
				});
			ks_string_view buff_view = buff;
			if (buff_view.length() > 4 && buff_view[4] == ';')
				buff_view = buff_view.substr(0, 4);
			bool is_true_literal = buff_view.length() == 4 && (buff_view == "true" || buff_view == "True" || buff_view == "TRUE");
			if (!is_true_literal)
				return def_value;
			if (pos != nullptr)
				*pos = str_view2.data() + 4 - str_view.data();
			return true;
		}
		else if ((first_wch >= '0' && first_wch <= '9') || first_wch == '+' || first_wch == '-') {
			size_t pos2 = 0;
			int int_val = parse_int(str_view2, 0, &pos2, 0);
			if (pos2 == 0)
				return def_value;
			if (pos != nullptr)
				*pos = str_view2.data() + pos2 - str_view.data();
			return (int_val != 0);
		}
		else {
			return def_value;
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	//parse int ...
	MODERN_STRING_API
	int parse_int(const ks_string_view& str_view, int def_value, size_t* pos, int base) {
		return __do_parse_intx<int>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	int parse_int(const ks_wstring_view& str_view, int def_value, size_t* pos, int base) {
		return __do_parse_intx<int>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	int8_t parse_int8(const ks_string_view& str_view, int8_t def_value, size_t* pos, int base) {
		return __do_parse_intx<int8_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	int8_t parse_int8(const ks_wstring_view& str_view, int8_t def_value, size_t* pos, int base) {
		return __do_parse_intx<int8_t>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	int16_t parse_int16(const ks_string_view& str_view, int16_t def_value, size_t* pos, int base) {
		return __do_parse_intx<int16_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	int16_t parse_int16(const ks_wstring_view& str_view, int16_t def_value, size_t* pos, int base) {
		return __do_parse_intx<int16_t>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	int32_t parse_int32(const ks_string_view& str_view, int32_t def_value, size_t* pos, int base) {
		return __do_parse_intx<int32_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	int32_t parse_int32(const ks_wstring_view& str_view, int32_t def_value, size_t* pos, int base) {
		return __do_parse_intx<int32_t>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	int64_t parse_int64(const ks_string_view& str_view, int64_t def_value, size_t* pos, int base) {
		return __do_parse_intx<int64_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	int64_t parse_int64(const ks_wstring_view& str_view, int64_t def_value, size_t* pos, int base) {
		return __do_parse_intx<int64_t>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	intptr_t parse_intptr(const ks_string_view& str_view, intptr_t def_value, size_t* pos, int base) {
		return __do_parse_intx<intptr_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	intptr_t parse_intptr(const ks_wstring_view& str_view, intptr_t def_value, size_t* pos, int base) {
		return __do_parse_intx<intptr_t>(str_view, def_value, pos, base);
	}

	//parse uint ...
	MODERN_STRING_API
	uint parse_uint(const ks_string_view& str_view, uint def_value, size_t* pos, int base) {
		return __do_parse_intx<uint>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	uint parse_uint(const ks_wstring_view& str_view, uint def_value, size_t* pos, int base) {
		return __do_parse_intx<uint>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	uint8_t parse_uint8(const ks_string_view& str_view, uint8_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uint8_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	uint8_t parse_uint8(const ks_wstring_view& str_view, uint8_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uint8_t>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	uint16_t parse_uint16(const ks_string_view& str_view, uint16_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uint16_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	uint16_t parse_uint16(const ks_wstring_view& str_view, uint16_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uint16_t>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	uint32_t parse_uint32(const ks_string_view& str_view, uint32_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uint32_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	uint32_t parse_uint32(const ks_wstring_view& str_view, uint32_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uint32_t>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	uint64_t parse_uint64(const ks_string_view& str_view, uint64_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uint64_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	uint64_t parse_uint64(const ks_wstring_view& str_view, uint64_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uint64_t>(str_view, def_value, pos, base);
	}

	MODERN_STRING_API
	uintptr_t parse_uintptr(const ks_string_view& str_view, uintptr_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uintptr_t>(str_view, def_value, pos, base);
	}
	MODERN_STRING_API
	uintptr_t parse_uintptr(const ks_wstring_view& str_view, uintptr_t def_value, size_t* pos, int base) {
		return __do_parse_intx<uintptr_t>(str_view, def_value, pos, base);
	}

	//parse float ...
	MODERN_STRING_API
	float parse_float(const ks_string_view& str_view, float def_value, size_t* pos) {
		return __do_parse_floatx<float>(str_view, def_value, pos);
	}
	MODERN_STRING_API
	float parse_float(const ks_wstring_view& str_view, float def_value, size_t* pos) {
		return __do_parse_floatx<float>(str_view, def_value, pos);
	}

	MODERN_STRING_API
	double parse_double(const ks_string_view& str_view, double def_value, size_t* pos) {
		return __do_parse_floatx<double>(str_view, def_value, pos);
	}
	MODERN_STRING_API
	double parse_double(const ks_wstring_view& str_view, double def_value, size_t* pos) {
		return __do_parse_floatx<double>(str_view, def_value, pos);
	}

	//parse bool ...
	MODERN_STRING_API
	bool parse_bool(const ks_string_view& str_view, bool def_value, size_t* pos) {
		return __do_parse_boolx<bool>(str_view, def_value, pos);
	}
	MODERN_STRING_API
	bool parse_bool(const ks_wstring_view& str_view, bool def_value, size_t* pos) {
		return __do_parse_boolx<bool>(str_view, def_value, pos);
	}

}
