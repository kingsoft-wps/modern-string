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


template <class ELEM>
void ks_basic_string_view<ELEM>::trim_left() {
	if (!this->empty()) {
		constexpr ELEM space_chars[] = { ' ', '\t', '\r', '\n', '\f', '\v', '\0' };
		constexpr size_t space_char_count = sizeof(space_chars) / sizeof(space_chars[0]);
		size_t pos = this->find_first_not_of(space_chars, 0, space_char_count);
		if (pos != size_t(-1))
			m_p += pos;
		else
			m_p += m_length;
	}
}

template <class ELEM>
void ks_basic_string_view<ELEM>::trim_right() {
	if (!this->empty()) {
		constexpr ELEM space_chars[] = { ' ', '\t', '\r', '\n', '\f', '\v', '\0' };
		constexpr size_t space_char_count = sizeof(space_chars) / sizeof(space_chars[0]);
		size_t pos = this->find_last_not_of(space_chars, size_t(-1), space_char_count);
		if (pos != size_t(-1))
			m_length = pos + 1;
		else
			m_length = 0;
	}
}

template <class ELEM>
std::vector<ks_basic_string_view<ELEM>> ks_basic_string_view<ELEM>::split(const ks_basic_string_view<ELEM>& sep, size_t n) const {
	if (this->empty() || n == 1)
		return { *this };

	size_t fixed_n = size_t(n);
	if (ptrdiff_t(fixed_n) <= 0)
		fixed_n = this->length();
	else if (fixed_n > this->length())
		fixed_n = this->length();


	if (sep.empty()) {
		std::vector<ks_basic_string_view<ELEM>> ret;
		ret.reserve(fixed_n);

		for (size_t i = 0; i + 1 < fixed_n; ++i)
			ret.push_back(this->unsafe_subview(i, 1));

		ret.push_back(this->unsafe_subview(fixed_n, this->length()));

		return ret;
	}
	else {
		std::vector<ks_basic_string_view<ELEM>> ret;
		ret.reserve(std::min(fixed_n, size_t(4))); //init as at-most 4 capa

		size_t next_pos = 0;
		for (size_t i = 0; i + 1 < fixed_n; ++i) {
			size_t next_pos_end = this->do_find(sep, next_pos);
			if (next_pos_end == size_t(-1))
				break;

			ret.push_back(this->unsafe_subview(next_pos, next_pos_end - next_pos));
			next_pos = next_pos_end + sep.length();
		}

		ret.push_back(this->unsafe_subview(next_pos, this->length() - next_pos));

		return ret;
	}
}

template <class ELEM>
size_t ks_basic_string_view<ELEM>::do_find(const ks_basic_string_view<ELEM>& str_view, size_t pos) const {
	const size_t this_length = this->length();
	const size_t right_length = str_view.length();
	if (right_length == 0 || this_length < right_length)
		return size_t(-1);
	if (pos > this_length - right_length)
		return size_t(-1);

	const ELEM* this_data = this->data();
	const ELEM* right_data = str_view.data();
	const ELEM* cur_p = this_data + pos;
	const ELEM* end_p = this_data + this_length - right_length + 1;
	while (cur_p < end_p) {
		cur_p = ks_char_traits<ELEM>::find(cur_p, end_p - cur_p, right_data[0]);
		if (cur_p == nullptr)
			return size_t(-1);
		if (right_length == 1 || cur_p == right_data || ks_char_traits<ELEM>::compare(cur_p + 1, right_data + 1, right_length - 1) == 0)
			return cur_p - this_data;
		++cur_p;
	}

	return size_t(-1);
}

template <class ELEM>
size_t ks_basic_string_view<ELEM>::do_rfind(const ks_basic_string_view<ELEM>& str_view, size_t pos) const {
	const size_t this_length = this->length();
	const size_t right_length = str_view.length();
	if (right_length == 0 || this_length < right_length)
		return size_t(-1);
	if (pos > this_length - right_length)
		pos = this_length - right_length;

	const ELEM* this_data = this->data();
	const ELEM* right_data = str_view.data();
	const ELEM* cur_p = this_data + pos;
	const ELEM  right_first_ch = right_data[0];
	for (; cur_p >= this_data; --cur_p) {
		if (*cur_p == right_first_ch) {
			if (right_length == 1 || cur_p == right_data || ks_char_traits<ELEM>::compare(cur_p + 1, right_data + 1, right_length - 1) == 0)
				return cur_p - this_data;
		}
	}

	return size_t(-1);
}

template <class ELEM>
size_t ks_basic_string_view<ELEM>::do_find_first_of(const ks_basic_string_view<ELEM>& str_view, size_t pos, bool not_mode) const {
	const size_t this_length = this->length();
	const size_t right_length = str_view.length();
	if (this_length == 0 || right_length == 0)
		return size_t(-1);
	else if (right_length == 1 && !not_mode)
		return this->do_find(str_view, pos);
	if (pos >= this_length)
		return size_t(-1);

	const ELEM* this_data = this->data();
	const ELEM* this_data_end = this_data + this_length;
	const ELEM* right_data = str_view.data();
	const ELEM* cur_p = this_data + pos;
	for (; cur_p < this_data_end; ++cur_p) {
		bool found = ks_char_traits<ELEM>::find(right_data, right_length, *cur_p) != nullptr;
		if (!found == not_mode)
			return cur_p - this_data;
	}

	return size_t(-1);
}

template <class ELEM>
size_t ks_basic_string_view<ELEM>::do_find_last_of(const ks_basic_string_view<ELEM>& str_view, size_t pos, bool not_mode) const {
	const size_t this_length = this->length();
	const size_t right_length = str_view.length();
	if (this_length == 0 || right_length == 0)
		return size_t(-1);
	else if (right_length == 1 && !not_mode)
		return this->do_rfind(str_view, pos);
	if (pos > this_length - 1)
		pos = this_length - 1;

	const ELEM* this_data = this->data();
	const ELEM* right_data = str_view.data();
	const ELEM* cur_p = this_data + pos;
	for (; cur_p >= this_data; --cur_p) {
		bool found = ks_char_traits<ELEM>::find(right_data, right_length, *cur_p) != nullptr;
		if (!found == not_mode)
			return cur_p - this_data;
	}

	return size_t(-1);
}



template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator==(const LEFT& left, const ks_basic_string_view<ELEM>& right) { return ks_basic_string_view<ELEM>(left) == right; }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator!=(const LEFT& left, const ks_basic_string_view<ELEM>& right) { return ks_basic_string_view<ELEM>(left) != right; }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator<(const LEFT& left, const ks_basic_string_view<ELEM>& right) { return ks_basic_string_view<ELEM>(left) < right; }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator<=(const LEFT& left, const ks_basic_string_view<ELEM>& right) { return ks_basic_string_view<ELEM>(left) <= right; }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator>(const LEFT& left, const ks_basic_string_view<ELEM>& right) { return ks_basic_string_view<ELEM>(left) > right; }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator>=(const LEFT& left, const ks_basic_string_view<ELEM>& right) { return ks_basic_string_view<ELEM>(left) >= right; }


namespace std {
	template <class ELEM>
	struct hash<ks_basic_string_view<ELEM>> {
		using argument_type = ks_basic_string_view<ELEM>;
		using result_type = size_t;

		size_t operator()(const ks_basic_string_view<ELEM>& str_view) const noexcept {
			constexpr size_t _FNV_offset_basis = sizeof(size_t) == 8 ? (size_t)14695981039346656037ULL : (size_t)2166136261UL;
			constexpr size_t _FNV_prime = sizeof(size_t) == 8 ? (size_t)1099511628211ULL : (size_t)16777619UL;

			size_t hash_val = _FNV_offset_basis;
			const ELEM* data = str_view.data();
			const ELEM* data_end = data + str_view.length();
			for (; data < data_end; ++data) {
				hash_val ^= static_cast<size_t>(*data);
				hash_val *= _FNV_prime;
			}

			return hash_val;
		}
	};
}


template <class ELEM>
std::basic_ostream<ELEM, std::char_traits<ELEM>>& operator<<(std::basic_ostream<ELEM, std::char_traits<ELEM>>& strm, const ks_basic_string_view<ELEM>& str_view) {
	using StreamType = std::basic_ostream<ELEM, std::char_traits<ELEM>>;
	using TraitType = std::char_traits<ELEM>;

	const ELEM* const data = str_view.data();
	const size_t length = str_view.length();
	size_t pad = (strm.width() <= 0 || static_cast<size_t>(strm.width()) <= length) ? 0 : static_cast<size_t>(strm.width()) - length;
	typename StreamType::iostate state = StreamType::goodbit;

	const typename StreamType::sentry strm_sentry(strm);
	if (!strm_sentry) {
		state |= StreamType::badbit;
	}
	else {
		try {
			if ((strm.flags() & StreamType::adjustfield) != StreamType::left) {
				for (; 0 < pad; --pad) { // pad on left
					if (TraitType::eq_int_type(TraitType::eof(), strm.rdbuf()->sputc(strm.fill()))) {
						state |= StreamType::badbit; // insertion failed, quit
						break;
					}
				}
			}

			if (state == StreamType::goodbit
				&& strm.rdbuf()->sputn(data, static_cast<std::streamsize>(length)) != static_cast<std::streamsize>(length)) {
				state |= StreamType::badbit;
			}
			else {
				for (; 0 < pad; --pad) { // pad on right
					if (TraitType::eq_int_type(TraitType::eof(), strm.rdbuf()->sputc(strm.fill()))) {
						state |= StreamType::badbit; // insertion failed, quit
						break;
					}
				}
			}

			strm.width(0);
		}
		catch (...) {
			strm.setstate(StreamType::badbit);
			throw; //re-throw current exception
		}
	}

	strm.setstate(state);
	return strm;
}
