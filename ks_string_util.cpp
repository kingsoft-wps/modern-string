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

#include "ks_string_util.h"

namespace ks_string_util {
	//icase compare ...
	template <class ELEM> 
	static int __do_icase_compare(const ks_basic_string_view<ELEM>& left, const ks_basic_string_view<ELEM>& right) {
		const ELEM* left_data = left.data();
		const ELEM* right_data = right.data();
		size_t left_length = left.length();
		size_t right_length = right.length();

		int diff = 0;
		if (left_data != right_data) {
			size_t min_length = std::min(left_length, right_length);
			for (size_t i = 0; i < min_length; ++i) {
				ELEM left_ch = left_data[i];
				ELEM right_ch = right_data[i];
				if (left_ch == right_ch)
					continue;

				if (left_ch >= 'A' && left_ch <= 'Z') {
					left_ch = 'a' + (left_ch - 'A');
					if (left_ch == right_ch)
						continue;
				}
				else if (right_ch >= 'A' && right_ch <= 'Z') {
					right_ch = 'a' + (right_ch - 'A');
					if (left_ch == right_ch)
						continue;
				}

				diff = left_ch < right_ch ? -1 : +1;
				break;
			}
		}

		if (diff == 0 && left_length != right_length)
			diff = left_length < right_length ? -1 : +1;

		return diff;
	}

	template <class ELEM> 
	static bool __do_icase_equals(const ks_basic_string_view<ELEM>& left, const ks_basic_string_view<ELEM>& right) {
		return left.length() == right.length()
			&& __do_icase_compare<ELEM>(left, right) == 0;
	}

	MODERN_STRING_API
	bool icase_equals(const ks_string_view& left, const ks_string_view& right) {
		return __do_icase_equals<char>(left, right);
	}

	MODERN_STRING_API
	bool icase_equals(const ks_wstring_view& left, const ks_wstring_view& right) {
		return __do_icase_equals<WCHAR>(left, right);
	}

}
