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

#include "ks_basic_pointer_iterator.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <ostream>

template <class ELEM>
class ks_basic_xmutable_string_base;


template <class ELEM>
class MODERN_STRING_API ks_basic_string_view {
	static_assert(std::is_trivial_v<ELEM> && std::is_standard_layout_v<ELEM>, "ELEM must be pod type");

public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using value_type = ELEM;
	using reference = const ELEM&;
	using const_reference = const ELEM&;
	using pointer = const ELEM*;
	using const_pointer = const ELEM*;

	using iterator = ks_basic_pointer_iterator<const ELEM>;
	using const_iterator = ks_basic_pointer_iterator<const ELEM>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	static constexpr size_t npos = size_t(-1);

public:
	constexpr ks_basic_string_view() : m_p(nullptr), m_length(0) {}
	constexpr ks_basic_string_view(const ELEM* p) : m_p(p), m_length(__c_strlen(p)) {}
	constexpr ks_basic_string_view(const ELEM* p, size_t count) : m_p(p), m_length(ptrdiff_t(count) < 0 ? __c_strlen(p) : count) {}

	//copy ctor
	ks_basic_string_view(const ks_basic_string_view& other) = default;
	ks_basic_string_view& operator=(const ks_basic_string_view& other) = default;
	ks_basic_string_view(ks_basic_string_view&& other) noexcept = default;
	ks_basic_string_view& operator=(ks_basic_string_view&& other) noexcept = default;

	//implicit ctor (from ks_xmutable_string, needed until c++17)
	ks_basic_string_view(const ks_basic_xmutable_string_base<ELEM>& str) 
		: m_p(str.data()), m_length(str.length()) {}

	//implicit ctor (from std::basic_string, needed until c++17)
	template <class AllocType>
	ks_basic_string_view(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str) 
		: m_p(str.data()), m_length(str.length()) {}

public:
	iterator begin() const { return iterator{ m_p }; }
	iterator end() const { return iterator{ m_p + m_length }; }
	const_iterator cbegin() const { return const_iterator{ m_p }; }
	const_iterator cend() const { return const_iterator{ m_p + m_length }; }
	reverse_iterator rbegin() const { return reverse_iterator{ this->end() }; }
	reverse_iterator rend() const { return reverse_iterator{ this->begin() }; }
	const_reverse_iterator crbegin() const { return reverse_iterator{ this->cend() }; }
	const_reverse_iterator crend() const { return reverse_iterator{ this->cbegin() }; }

public:
	int compare(const ELEM* p) const { return this->do_compare(__to_basic_string_view(p)); }
	int compare(const ELEM* p, size_t count) const { return this->do_compare(__to_basic_string_view(p, count)); }
	int compare(const ks_basic_string_view<ELEM>& str_view) const { return this->do_compare(str_view); }

	bool equals(const ELEM* p) const { return this->do_equals(__to_basic_string_view(p)); }
	bool equals(const ELEM* p, size_t count) const { return this->do_equals(__to_basic_string_view(p, count)); }
	bool equals(const ks_basic_string_view<ELEM>& str_view) const { return this->do_equals(str_view); }

	bool contains(const ELEM* p) const { return this->do_find(__to_basic_string_view(p), 0) != size_t(-1); }
	bool contains(const ELEM* p, size_t count) const { return this->do_find(__to_basic_string_view(p, count), 0) != size_t(-1); }
	bool contains(const ks_basic_string_view<ELEM>& str_view) const { return this->do_find(str_view, 0) != size_t(-1); }
	bool contains(ELEM ch) const { return this->do_find(__to_basic_string_view(&ch, 1), 0) != size_t(-1); }

	bool starts_with(const ELEM* p) const { return this->starts_with(__to_basic_string_view(p)); }
	bool starts_with(const ELEM* p, size_t count) const { return this->starts_with(__to_basic_string_view(p, count)); }
	bool starts_with(const ks_basic_string_view<ELEM>& str_view) const { return this->length() >= str_view.length() && this->unsafe_subview(0, str_view.length()).do_equals(str_view); }
	bool starts_with(ELEM ch) const { return this->starts_with(__to_basic_string_view(&ch, 1)); }

	bool ends_with(const ELEM* p) const { return this->ends_with(__to_basic_string_view(p)); }
	bool ends_with(const ELEM* p, size_t count) const { return this->ends_with(__to_basic_string_view(p, count)); }
	bool ends_with(const ks_basic_string_view<ELEM>& str_view) const { return this->length() >= str_view.length() && this->unsafe_subview(this->length() - str_view.length(), str_view.length()).do_equals(str_view); }
	bool ends_with(ELEM ch) const { return this->ends_with(__to_basic_string_view(&ch, 1)); }

	size_t find(const ELEM* p, size_t pos = 0) const { return this->do_find(__to_basic_string_view(p), pos); }
	size_t find(const ELEM* p, size_t pos, size_t count) const { return this->do_find(__to_basic_string_view(p, count), pos); }
	size_t find(const ks_basic_string_view<ELEM>& str_view, size_t pos = 0) const { return this->do_find(str_view, pos); }
	size_t find(ELEM ch, size_t pos = 0) const { return this->do_find(__to_basic_string_view(&ch, 1), pos); }

	size_t rfind(const ELEM* p, size_t pos = -1) const { return this->do_rfind(__to_basic_string_view(p), pos); }
	size_t rfind(const ELEM* p, size_t pos, size_t count) const { return this->do_rfind(__to_basic_string_view(p, count), pos); }
	size_t rfind(const ks_basic_string_view<ELEM>& str_view, size_t pos = -1) const { return this->do_rfind(str_view, pos); }
	size_t rfind(ELEM ch, size_t pos = -1) const { return this->do_rfind(__to_basic_string_view(&ch, 1), pos); }

	size_t find_first_of(const ELEM* p, size_t pos = 0) const { return this->do_find_first_of(__to_basic_string_view(p), pos, false); }
	size_t find_first_of(const ELEM* p, size_t pos, size_t count) const { return this->do_find_first_of(__to_basic_string_view(p, count), pos, false); }
	size_t find_first_of(const ks_basic_string_view<ELEM>& str_view, size_t pos = 0) const { return this->do_find_first_of(str_view, pos, false); }
	size_t find_first_of(ELEM ch, size_t pos = 0) const { return this->do_find_first_of(__to_basic_string_view(&ch, 1), pos, false); }

	size_t find_last_of(const ELEM* p, size_t pos = -1) const { return this->do_find_last_of(__to_basic_string_view(p), pos, false); }
	size_t find_last_of(const ELEM* p, size_t pos, size_t count) const { return this->do_find_last_of(__to_basic_string_view(p, count), pos, false); }
	size_t find_last_of(const ks_basic_string_view<ELEM>& str_view, size_t pos = -1) const { return this->do_find_last_of(str_view, pos, false); }
	size_t find_last_of(ELEM ch, size_t pos = -1) const { return this->do_find_last_of(__to_basic_string_view(&ch, 1), pos, false); }

	size_t find_first_not_of(const ELEM* p, size_t pos = 0) const { return this->do_find_first_of(__to_basic_string_view(p), pos, true); }
	size_t find_first_not_of(const ELEM* p, size_t pos, size_t count) const { return this->do_find_first_of(__to_basic_string_view(p, count), pos, true); }
	size_t find_first_not_of(const ks_basic_string_view<ELEM>& str_view, size_t pos = 0) const { return this->do_find_first_of(str_view, pos, true); }
	size_t find_first_not_of(ELEM ch, size_t pos = 0) const { return this->do_find_first_of(__to_basic_string_view(&ch, 1), pos, true); }

	size_t find_last_not_of(const ELEM* p, size_t pos = -1) const { return this->do_find_last_of(__to_basic_string_view(p), pos, true); }
	size_t find_last_not_of(const ELEM* p, size_t pos, size_t count) const { return this->do_find_last_of(__to_basic_string_view(p, count), pos, true); }
	size_t find_last_not_of(const ks_basic_string_view<ELEM>& str_view, size_t pos = -1) const { return this->do_find_last_of(str_view, pos, true); }
	size_t find_last_not_of(ELEM ch, size_t pos = -1) const { return this->do_find_last_of(__to_basic_string_view(&ch, 1), pos, true); }

public:
	void trim_left();
	void trim_right();

	void trim() {
		this->trim_right();
		this->trim_left();
	}


	std::vector<ks_basic_string_view<ELEM>> split(const ks_basic_string_view<ELEM>& sep, size_t n = -1) const;

protected:
	int do_compare(const ks_basic_string_view<ELEM>& right) const {
		const ELEM* left_data = this->data();
		const ELEM* right_data = right.data();
		size_t left_length = this->length();
		size_t right_length = right.length();
		int diff = left_data == right_data ? 0 : ks_char_traits<ELEM>::compare(left_data, right_data, std::min(left_length, right_length));
		if (diff == 0 && left_length != right_length)
			diff = left_length < right_length ? -1 : +1;
		return diff;
	}

	bool do_equals(const ks_basic_string_view<ELEM>& right) const {
		return this->length() == right.length() 
			&& this->do_compare(right) == 0;
	}

	size_t do_find(const ks_basic_string_view<ELEM>& str_view, size_t pos) const;
	size_t do_rfind(const ks_basic_string_view<ELEM>& str_view, size_t pos) const;

	size_t do_find_first_of(const ks_basic_string_view<ELEM>& str_view, size_t pos, bool not_mode) const;
	size_t do_find_last_of(const ks_basic_string_view<ELEM>& str_view, size_t pos, bool not_mode) const;

public:
	ks_basic_string_view slice(size_t from, size_t to = size_t(-1)) const {
		const size_t this_length = this->length();
		if (from > this_length)
			throw std::out_of_range("ks_basic_string_view::slice(from, to) out-of-range exception");
		if (to > this_length)
			to = this_length;
		else if (to < from)
			to = from;
		return this->unsafe_subview(from, to - from);
	}

	ks_basic_string_view substr(size_t pos, size_t count = size_t(-1)) const {
		const size_t this_length = this->length();
		if (pos > this_length)
			throw std::out_of_range("ks_basic_string_view::substr(pos, count) out-of-range exception");
		if (count > this_length - pos)
			count = this_length - pos;
		return this->unsafe_subview(pos, count);
	}

	ks_basic_string_view trimmed() const {
		ks_basic_string_view ret(*this);
		ret.trim();
		return ret;
	}

protected:
	ks_basic_string_view unsafe_subview(size_t pos, size_t count) const {
		return ks_basic_string_view(m_p + (ptrdiff_t)pos, count);
	}

public:
	bool is_subview_of(const ks_basic_string_view& other) const {
		return this->data() >= other.data() && this->data_end() <= other.data_end();
	}

	bool is_overlapped_with(const ks_basic_string_view& other) const {
		if (this->empty() || other.empty())
			return false;
		if (this->data() >= other.data_end())
			return false;
		if (this->data_end() <= other.data())
			return false;
		return true;
	}

public:
	const ELEM& front() const {
		ASSERT(!this->empty());
		return *this->data();
	}

	const ELEM& back() const {
		ASSERT(!this->empty());
		return *(this->data_end() - 1);
	}

	const ELEM& at(size_t pos) const {
		if (pos >= this->length())
			throw std::out_of_range("ks_basic_string_view::at(pos) out-of-range exception");
		return this->data()[pos];
	}

	const ELEM& operator[](size_t pos) const {
		ASSERT(pos < this->length());
		return this->data()[pos];
	}

public:
	const ELEM* data() const { return m_p; }
	const ELEM* data_end() const { return m_p + m_length; }
	size_t length() const { return m_length; }
	size_t size() const { return m_length; }
	bool empty() const { return m_length == 0; }

private:
	const ELEM* m_p;
	size_t m_length;

public:
	bool operator==(const ks_basic_string_view& right) const { return this->equals(right); }
	bool operator!=(const ks_basic_string_view& right) const { return !this->equals(right); }
	bool operator<(const ks_basic_string_view& right) const { return this->compare(right) < 0; }
	bool operator<=(const ks_basic_string_view& right) const { return this->compare(right) <= 0; }
	bool operator>(const ks_basic_string_view& right) const { return this->compare(right) > 0; }
	bool operator>=(const ks_basic_string_view& right) const { return this->compare(right) >= 0; }

protected:
	static constexpr inline size_t __c_strlen(const ELEM* p) {
		const ELEM* t = p;
		if (t != nullptr) {
			while (*t)
				++t;
			return t - p;
		}
		else {
			return 0;
		}
	}

	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ELEM* p) { return ks_basic_string_view<ELEM>(p); }
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ELEM* p, size_t count) { return ks_basic_string_view<ELEM>(p, count); }

	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ks_basic_string_view<ELEM>& str_view) { return str_view; }
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count) { return str_view.substr(offset, count); }

	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ks_basic_xmutable_string_base<ELEM>& str) { return str.view(); }
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ks_basic_xmutable_string_base<ELEM>& str, size_t offset, size_t count) { return str.view().substr(offset, count); }

	template <class AllocType>
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str) { return ks_basic_string_view<ELEM>(str.data(), str.length()); }
	template <class AllocType>
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str, size_t offset, size_t count) { return ks_basic_string_view<ELEM>(str.data(), str.length()).substr(offset, count); }

	friend class ks_basic_xmutable_string_base<ELEM>;
};


#include "ks_basic_string_view.inl"
