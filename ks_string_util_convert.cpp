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

#include "ks_string_util_convert.h"

#ifdef _WIN32
#include <Windows.h>
#include <stringapiset.h>
#else
#include <codecvt>
#include <locale>
#include <clocale>
#endif

namespace ks_string_util {
	//convert from ...
	MODERN_STRING_API
	ks_immutable_wstring wstring_from_utf8(const char8_t* p, size_t len) {
		ks_basic_string_view<uint8_t> str_view8((const uint8_t*)p, len);
		if (str_view8.empty())
			return ks_immutable_wstring();

		const uint8_t* const str_data8 = str_view8.data();
		const size_t str_length8 = str_view8.length();

		//note：we do not use wstring_convert, but implement utf8 converting ourselves.
		//std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> std_converter;
		//std::basic_string<char16_t> std_str16 = std_converter.from_bytes(str_view.data(), str_view.data() + str_view.length());
		//return __wstring_from_utf16(std_str16.data(), std_str16.length());

		size_t wch_count = str_length8;
		for (size_t pos8 = 0; pos8 < str_length8; ) {
			uint8_t ch8 = str_data8[pos8++];
			if (ch8 <= 0xBF) {
				//0xxxxxxx
				//10xxxxxx (invalid)
				ASSERT(ch8 <= 0x7F);
			}
			else if (ch8 <= 0xDF) {
				//110xxxxx 10xxxxxx
				ASSERT(pos8 + 0 < str_length8 && (ch8 & 0xE0) == 0xC0 && (str_data8[pos8 + 0] & 0xC0) == 0x80);
				size_t len8_addi = (std::min)(size_t(1), str_length8 - pos8);
				wch_count -= len8_addi;
				pos8 += len8_addi;
			}
			else if (ch8 <= 0xEF) {
				//1110xxxx 10xxxxxx 10xxxxxx
				ASSERT(pos8 + 1 < str_length8 && (ch8 & 0xF0) == 0xE0 && (str_data8[pos8 + 0] & 0xC0) == 0x80 && (str_data8[pos8 + 1] & 0xC0) == 0x80);
				size_t len8_addi = (std::min)(size_t(2), str_length8 - pos8);
				wch_count -= len8_addi;
				pos8 += len8_addi;
			}
			else if (ch8 <= 0xF7) {
				//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
				ASSERT(pos8 + 2 < str_length8 && (ch8 & 0xF8) == 0xF0 && (str_data8[pos8 + 0] & 0xC0) == 0x80 && (str_data8[pos8 + 1] & 0xC0) == 0x80 && (str_data8[pos8 + 2] & 0xC0) == 0x80);
				size_t len8_addi = (std::min)(size_t(3), str_length8 - pos8);
				wch_count -= len8_addi;
				wch_count += 1; //因为rune >= 0x10000
				pos8 += len8_addi;
			}
			else {
				//11111xxx (invalid)
				ASSERT(false);
			}
		}

		ks_mutable_wstring wstr;
		WCHAR* wstr_data = wstr.__begin_exclusive_writing(wch_count);
		WCHAR* wstr_data_p = wstr_data;
		for (size_t pos8 = 0; pos8 < str_length8; ) {
			uint8_t ch8 = str_data8[pos8++];
			if (ch8 <= 0xBF) {
				//0xxxxxxx
				//10xxxxxx (invalid)
				ASSERT(ch8 <= 0x7F);
				*(wstr_data_p++) = (WCHAR)(ch8);
			}
			else if (ch8 <= 0xDF) {
				//110xxxxx 10xxxxxx
				uint8_t ch8_addi = pos8 < str_length8 ? str_data8[pos8++] : 0;
				uint rune = ((uint(ch8) & 0x01F) << 6) + ((uint(ch8_addi) & 0x3F));
				*(wstr_data_p++) = (WCHAR)(rune);
			}
			else if (ch8 <= 0xEF) {
				//1110xxxx 10xxxxxx 10xxxxxx
				uint8_t ch8_addi_arr[2] = { 0, 0 };
				size_t len8_addi = (std::min)(size_t(2), str_length8 - pos8);
				std::copy_n(str_data8 + pos8, len8_addi, ch8_addi_arr);
				pos8 += len8_addi;
				uint rune = ((uint(ch8) & 0x0F) << 12) + ((uint(ch8_addi_arr[0]) & 0x3F) << 6) + ((uint(ch8_addi_arr[1]) & 0x3F));
				*(wstr_data_p++) = (WCHAR)(rune);
			}
			else if (ch8 <= 0xF7) {
				//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
				uint8_t ch8_addi_arr[3] = { 0, 0, 0 };
				size_t len8_addi = (std::min)(size_t(3), str_length8 - pos8);
				std::copy_n(str_data8 + pos8, len8_addi, ch8_addi_arr);
				pos8 += len8_addi;
				uint rune = ((uint(ch8) & 0x07) << 18) + ((uint(ch8_addi_arr[0]) & 0x3F) << 12) + ((uint(ch8_addi_arr[1]) & 0x3F) << 6) + ((uint(ch8_addi_arr[2]) & 0x3F));
				ASSERT(rune >= 0x10000);
				rune -= 0x10000;
				ASSERT((rune & ~0xFFFFF) == 0);
				*(wstr_data_p++) = (WCHAR)(0xD800 | ((rune & 0xFFC00) >> 10));
				*(wstr_data_p++) = (WCHAR)(0xDC00 | ((rune & 0x003FF)));
			}
			else {
				//11111xxx (invalid)
				ASSERT(false);
				*(wstr_data_p++) = (WCHAR)(ch8);
			}
		}

		ASSERT(wstr_data_p == wstr_data + wch_count);
		wstr.__end_exclusive_writing(wstr_data, wstr_data_p - wstr_data);
		return std::move(wstr);
	}

	MODERN_STRING_API
	ks_immutable_wstring __wstring_from_utf16(const char16_t* p, size_t len) {
		return ks_immutable_wstring((const WCHAR*)p, len);
	}

	MODERN_STRING_API
	ks_immutable_wstring wstring_from_utf32(const char32_t* p, size_t len) {
		ks_basic_string_view<uint32_t> str_view32((const uint32_t*)p, len);
		if (str_view32.empty())
			return ks_immutable_wstring();

		size_t wch_count = str_view32.length();
		wch_count += std::count_if(str_view32.cbegin(), str_view32.cend(), [](uint rune) { return rune >= 0x10000; });

		ks_mutable_wstring wstr;
		WCHAR* wstr_data = wstr.__begin_exclusive_writing(wch_count);
		WCHAR* wstr_data_p = wstr_data;
		for (uint rune : str_view32) {
			if (rune < 0x10000) {
				*(wstr_data_p++) = (WCHAR)rune;
			}
			else {
				rune -= 0x10000;
				ASSERT((rune & ~0xFFFFF) == 0);
				*(wstr_data_p++) = (WCHAR)(0xD800 | ((rune & 0xFFC00) >> 10));
				*(wstr_data_p++) = (WCHAR)(0xDC00 | ((rune & 0x003FF)));
			}
		}

		ASSERT(wstr_data_p == wstr_data + wch_count);
		wstr.__end_exclusive_writing(wstr_data, wstr_data_p - wstr_data);
		return std::move(wstr);
	}

	MODERN_STRING_API
	ks_immutable_wstring wstring_from_native_chars(const char* p, size_t len) {
		ks_basic_string_view<char> str_view(p, len);
		if (str_view.empty())
			return ks_immutable_wstring();

#ifdef _WIN32
		UINT acp = ::GetACP();
		if (acp == 65001) {
			return wstring_from_utf8((const char8_t*)p, len);
		}

		if (acp == 936 || acp == 20936)
			acp = 54936; //auto promote GBK or GB2312 to GB18030

		int wch_count = ::MultiByteToWideChar(acp, 0, str_view.data(), (int)str_view.length(), NULL, 0);
		ASSERT(wch_count > 0);
		if (wch_count <= 0) 
			return ks_immutable_wstring();

		ks_mutable_wstring wstr;
		WCHAR* wstr_data = wstr.__begin_exclusive_writing((size_t)(wch_count));
		wch_count = ::MultiByteToWideChar(acp, 0, str_view.data(), (int)str_view.length(), (LPWSTR)wstr_data, wch_count);
		wstr.__end_exclusive_writing(wstr_data, (size_t)(std::max)(wch_count, 0));
		return std::move(wstr);

#else
		const char* loc_name = std::setlocale(LC_ALL, nullptr);
		std::string loc_name_str(loc_name != nullptr ? loc_name : "");
		std::string loc_name_str_up(loc_name_str);
		for (auto& c : loc_name_str_up)
			c = (char)toupper(c);

		if (loc_name_str_up.empty() || loc_name_str_up == "C" || loc_name_str_up == "POSIX" || 
			ks_string_view(loc_name_str_up).ends_with(".UTF-8") || ks_string_view(loc_name_str_up).ends_with(".UTF8")) {
			return wstring_from_utf8((const char8_t*)p, len);
		}

		using codecvt_by_loc_t = struct codecvt_byname_facet : std::codecvt_byname<wchar_t, char, std::mbstate_t> { using std::codecvt_byname<wchar_t, char, std::mbstate_t>::codecvt_byname; };
		if (sizeof(wchar_t) == 2) {
			using std_converter_t = std::wstring_convert<codecvt_by_loc_t, wchar_t, ks_basic_string_allocator<wchar_t>>;
			std_converter_t std_converter(new codecvt_by_loc_t(loc_name_str.c_str()));
			auto std_wstr = std_converter.from_bytes(str_view.data(), str_view.data() + str_view.length());
			return ks_immutable_wstring(std::move((std::basic_string<WCHAR, std::char_traits<WCHAR>, ks_basic_string_allocator<WCHAR>>&)(std_wstr)));
		}
		else {
			using std_converter_t = std::wstring_convert<codecvt_by_loc_t, wchar_t>;
			std_converter_t std_converter(new codecvt_by_loc_t(loc_name_str.c_str()));
			auto std_wstr = std_converter.from_bytes(str_view.data(), str_view.data() + str_view.length());
			return wstring_from_native_wchars(std_wstr.data(), std_wstr.length());
		}
#endif
	}

	MODERN_STRING_API
	ks_immutable_wstring wstring_from_native_wchars(const wchar_t* p, size_t len) {
		static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "the size of wchar_t must be 2 or 4");
		if (sizeof(wchar_t) == 2)
			return __wstring_from_utf16((const char16_t*)p, len);
		else
			return wstring_from_utf32((const char32_t*)p, len);
	}


	//convert to ...
	MODERN_STRING_API
	std::basic_string<char8_t> wstring_to_std_u8string(const ks_wstring_view& str_view) {
		if (str_view.empty())
			return std::basic_string<char8_t>();

		//note：we do not use wstring_convert, but implement utf8 converting ourselves.
		//std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> std_converter;
		//return std_converter.to_bytes((char16_t*)str_view.data(), (char16_t*)str_view.data() + str_view.length());

		const WCHAR* const str_data = str_view.data();
		const size_t str_length = str_view.length();

		size_t char8_count = str_length;
		for (size_t pos = 0; pos < str_length; ) {
			WCHAR ch = str_data[pos++];
			if ((ch & 0xFC00) == 0xD800) {
				ASSERT(pos < str_length && (str_data[pos] & 0xFC00) == 0xDC00);
				size_t len_addi = (std::min)(size_t(1), str_length - pos);
				char8_count -= len_addi;
				char8_count += 3; //11110xxxx 10xxxxxx 10xxxxxx 10xxxxxx
				pos += len_addi;
			}
			else {
				ASSERT((ch & 0xFC00) != 0xDC00);
				if (ch <= 0x7F)
					(void)0; //0xxxxxxx
				else if (ch <= 0x7FF)
					char8_count += 1; //110xxxxx 10xxxxxx
				else
					char8_count += 2; //1110xxxx 10xxxxxx 10xxxxxx
			}
		}

		std::basic_string<char8_t> str8;
		str8.resize(char8_count);
		uint8_t* str8_data_p = (uint8_t*)str8.data();
		for (size_t pos = 0; pos < str_length; ) {
			WCHAR ch = str_data[pos++];
			if ((ch & 0xFC00) == 0xD800) {
				WCHAR ch_addi = pos < str_length ? str_data[pos++] : 0;
				uint rune = 0x10000 + (((uint(ch) & 0x3FF) << 10) | (uint(ch_addi) & 0x3FF));
				//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
				*(str8_data_p++) = (uint8_t)(0xF0 | ((rune & 0x1C0000) >> 18));
				*(str8_data_p++) = (uint8_t)(0x80 | ((rune & 0x03F000) >> 12));
				*(str8_data_p++) = (uint8_t)(0x80 | ((rune & 0x000FC0) >> 6));
				*(str8_data_p++) = (uint8_t)(0x80 | ((rune & 0x00003F)));
			}
			else {
				if (ch <= 0x7F) {
					//0xxxxxxx
					*(str8_data_p++) = (uint8_t)(ch);
				}
				else if (ch <= 0x7FF) {
					//110xxxxx 10xxxxxx
					*(str8_data_p++) = (uint8_t)(0xC0 | ((ch & 0x7C0) >> 6));
					*(str8_data_p++) = (uint8_t)(0x80 | ((ch & 0x003F)));
				}
				else {
					//1110xxxx 10xxxxxx 10xxxxxx
					*(str8_data_p++) = (uint8_t)(0xE0 | ((ch & 0xF000) >> 12));
					*(str8_data_p++) = (uint8_t)(0x80 | ((ch & 0x0FC0) >> 6));
					*(str8_data_p++) = (uint8_t)(0x80 | ((ch & 0x003F)));
				}
			}
		}

		ASSERT(str8_data_p == (uint8_t*)str8.data() + str8.length());
		return str8;
	}

	MODERN_STRING_API
	std::basic_string<char16_t> __wstring_to_std_u16string(const ks_wstring_view& str_view) {
		return std::basic_string<char16_t>((char16_t*)str_view.data(), str_view.length());
	}

	MODERN_STRING_API
	std::basic_string<char32_t> wstring_to_std_u32string(const ks_wstring_view& str_view) {
		if (str_view.empty())
			return std::basic_string<char32_t>();

		const WCHAR* const str_data = str_view.data();
		const size_t str_length = str_view.length();

		size_t char32_count = str_length;
		for (size_t pos = 0; pos < str_length; ) {
			WCHAR ch = str_data[pos++];
			if ((ch & 0xFC00) == 0xD800) {
				ASSERT(pos < str_length && (str_data[pos] & 0xFC00) == 0xDC00);
				size_t len_addi = (std::min)(size_t(1), str_length - pos);
				char32_count -= len_addi;
				pos += len_addi;
			}
			else {
				ASSERT((ch & 0xFC00) != 0xDC00);
			}
		}

		std::basic_string<char32_t> str32;
		str32.resize(char32_count);
		uint32_t* str32_data_p = (uint32_t*)str32.data();
		for (size_t pos = 0; pos < str_length; ) {
			WCHAR ch = str_data[pos++];
			if ((ch & 0xFC00) == 0xD800) {
				WCHAR ch_addi = pos < str_length ? str_data[pos++] : 0;
				uint rune = 0x10000 + (((uint(ch) & 0x3FF) << 10) | (uint(ch_addi) & 0x3FF));
				*(str32_data_p++) = (uint32_t)rune;
			}
			else {
				*(str32_data_p++) = (uint32_t)ch;
			}
		}

		ASSERT(str32_data_p == (uint32_t*)str32.data() + str32.length());
		return str32;
	}

	MODERN_STRING_API
	std::string wstring_to_std_string(const ks_wstring_view& str_view) {
		if (str_view.empty())
			return std::string();

#ifdef _WIN32
		UINT acp = ::GetACP();
		if (acp == 65001) {
			std::basic_string<char8_t> str8 = wstring_to_std_u8string(str_view);
			return std::move((std::string&)(str8));
		}

		if (acp == 936 || acp == 20936)
			acp = 54936; //auto promote GBK or GB2312 to GB18030

		int ach_count = ::WideCharToMultiByte(acp, 0, str_view.data(), (int)str_view.length(), NULL, 0, NULL, NULL);
		ASSERT(ach_count > 0);
		if (ach_count <= 0)
			return std::string();

		std::string std_str;
		std_str.resize((size_t)(ach_count));
		char* std_str_data = (char*)std_str.data();
		ach_count = ::WideCharToMultiByte(acp, 0, str_view.data(), (int)str_view.length(), (LPSTR)std_str_data, ach_count, NULL, NULL);
		std_str.resize((size_t)(std::max)(ach_count, 0));
		return std_str;

#else
		const char* loc_name = std::setlocale(LC_ALL, nullptr);
		std::string loc_name_str(loc_name != nullptr ? loc_name : "");
		std::string loc_name_str_up(loc_name_str);
		for (auto& c : loc_name_str_up)
			c = (char)toupper(c);

		if (loc_name_str_up.empty() || loc_name_str_up == "C" || loc_name_str_up == "POSIX" || 
			ks_string_view(loc_name_str_up).ends_with(".UTF-8") || ks_string_view(loc_name_str_up).ends_with(".UTF8")) {
			std::basic_string<char8_t> str8 = wstring_to_std_u8string(str_view);
			return std::move((std::string&)(str8));
		}

		using codecvt_by_loc_t = struct codecvt_byname_facet : std::codecvt_byname<wchar_t, char, std::mbstate_t> { using std::codecvt_byname<wchar_t, char, std::mbstate_t>::codecvt_byname; };
		using std_converter_t = std::wstring_convert<codecvt_by_loc_t, wchar_t>;
		std_converter_t std_converter(new codecvt_by_loc_t(loc_name_str.c_str()));
		if (sizeof(wchar_t) == 2) {
			return std_converter.to_bytes((wchar_t*)str_view.data(), (wchar_t*)str_view.data() + str_view.length());
		}
		else {
			std::wstring std_wstr = wstring_to_std_wstring(str_view);
			return std_converter.to_bytes(std_wstr.data(), std_wstr.data() + std_wstr.length());
		}
#endif
	}

	MODERN_STRING_API
	std::wstring wstring_to_std_wstring(const ks_wstring_view & str_view) {
		static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "the size of wchar_t must be 2 or 4");
		if (sizeof(wchar_t) == 2) {
			std::basic_string<char16_t> str16 = __wstring_to_std_u16string(str_view);
			return std::move((std::wstring&)(str16));
		}
		else {
			std::basic_string<char32_t> str32 = wstring_to_std_u32string(str_view);
			return std::move((std::wstring&)(str32));
		}
	}

	MODERN_STRING_API
	char32_t __take_next_rune(const ks_wstring_view& str_view, size_t* pos) {
		size_t temp_pos;
		if (pos == nullptr) {
			temp_pos = 0;
			pos = &temp_pos;
		}

		ASSERT(*pos < str_view.length());
		WCHAR ch = *pos < str_view.length() ? str_view[(*pos)++] : 0;
		if ((ch & 0xFC00) == 0xD800) {
			ASSERT(*pos < str_view.length() && (str_view[*pos] & 0xFC00) == 0xDC00);
			WCHAR ch_addi = *pos < str_view.length() ? str_view[(*pos)++] : 0;
			uint rune = 0x10000 + (((uint(ch) & 0x3FF) << 10) | (uint(ch_addi) & 0x3FF));
			return (char32_t)(rune);
		}
		else {
			ASSERT((ch & 0xFC00) != 0xDC00);
			return (char32_t)(ch);
		}
	}

}
