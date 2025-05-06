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

#include "base.h"
#include "ks_basic_xmutable_string_base.h"
#include <istream>


template <class ELEM>
class MODERN_STRING_API ks_basic_mutable_string : public ks_basic_xmutable_string_base<ELEM> {
	using __my_string_base = ks_basic_xmutable_string_base<ELEM>;
	using __my_string_base::__to_basic_string_view;

public:
	using typename __my_string_base::size_type;
	using typename __my_string_base::difference_type;

	using typename __my_string_base::value_type;
	using typename __my_string_base::reference;
	using typename __my_string_base::const_reference;
	using typename __my_string_base::pointer;
	using typename __my_string_base::const_pointer;

	using typename __my_string_base::iterator;
	using typename __my_string_base::const_iterator;
	using typename __my_string_base::reverse_iterator;
	using typename __my_string_base::const_reverse_iterator;

	using __my_string_base::npos;

public:
	//normal ctor
	ks_basic_mutable_string() : __my_string_base() { ASSERT(this->do_check_end_ch0()); }
	ks_basic_mutable_string(const ELEM* p) : __my_string_base(p) { ASSERT(this->do_check_end_ch0()); }
	ks_basic_mutable_string(const ELEM* p, size_t count) : __my_string_base(p, count) { ASSERT(this->do_check_end_ch0()); }

	ks_basic_mutable_string(const ks_basic_string_view<ELEM>& str_view) : __my_string_base(str_view) { ASSERT(this->do_check_end_ch0()); }
	ks_basic_mutable_string(const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count = -1)
		: __my_string_base(str_view.substr(offset, count)) { ASSERT(this->do_check_end_ch0()); }

	ks_basic_mutable_string(size_t count, ELEM ch) : __my_string_base(count, ch) { ASSERT(this->do_check_end_ch0()); }
	
	//copy & move ctor
	ks_basic_mutable_string(const ks_basic_mutable_string& other) = default;
	ks_basic_mutable_string& operator=(const ks_basic_mutable_string& other) = default;
	ks_basic_mutable_string(ks_basic_mutable_string&& other) noexcept = default;
	ks_basic_mutable_string& operator=(ks_basic_mutable_string&& other) noexcept = default;

	//copy & move ctor (from xmutable)
	ks_basic_mutable_string(const ks_basic_xmutable_string_base<ELEM>& other) 
		: __my_string_base(other) { this->do_ensure_end_ch0(true); }
	ks_basic_mutable_string(const ks_basic_xmutable_string_base<ELEM>& other, size_t offset, size_t count = -1)
		: __my_string_base(other.do_substr(offset, count)) { this->do_ensure_end_ch0(true); }
	ks_basic_mutable_string(ks_basic_xmutable_string_base<ELEM>&& other) 
		: __my_string_base(other.do_detach()) { ASSERT(other.is_detached_empty()); this->do_ensure_end_ch0(true); }
	ks_basic_mutable_string(ks_basic_xmutable_string_base<ELEM>&& other, size_t offset, size_t count = -1)
		: __my_string_base(other.do_detach().do_substr(offset, count)) { ASSERT(other.is_detached_empty()); this->do_ensure_end_ch0(true); }

	//copy & move ctor (from std::basic_string)
	template <class AllocType>
	ks_basic_mutable_string(const std::basic_string<ELEM, ks_char_traits<ELEM>, AllocType>& str)
		: __my_string_base(__to_basic_string_view(str)) { ASSERT(this->do_check_end_ch0()); }
	template <class AllocType>
	ks_basic_mutable_string(const std::basic_string<ELEM, ks_char_traits<ELEM>, AllocType>& str, size_t offset, size_t count = -1)
		: __my_string_base(__to_basic_string_view(str, offset, count)) { ASSERT(this->do_check_end_ch0()); }

	ks_basic_mutable_string(std::basic_string<ELEM, ks_char_traits<ELEM>, ks_basic_string_allocator<ELEM>>&& str_rvref)
		: __my_string_base(std::move(str_rvref)) { ASSERT(this->do_check_end_ch0()); }
	ks_basic_mutable_string(std::basic_string<ELEM, ks_char_traits<ELEM>, ks_basic_string_allocator<ELEM>>&& str_rvref, size_t offset, size_t count = -1)
		: __my_string_base(__my_string_base(std::move(str_rvref)).substr(offset, count)) { ASSERT(this->do_check_end_ch0()); }

public:
	//assign...
	ks_basic_mutable_string& assign(const ELEM* p) {
		this->do_assign(__to_basic_string_view(p), true);
		return *this;
	}
	ks_basic_mutable_string& assign(const ELEM* p, size_t count) {
		this->do_assign(__to_basic_string_view(p, count), true);
		return *this;
	}

	ks_basic_mutable_string& assign(const ks_basic_string_view<ELEM>& str_view) {
		this->do_assign(str_view, true);
		return *this;
	}
	ks_basic_mutable_string& assign(const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count = -1) {
		this->do_assign(str_view.substr(offset, count), true);
		return *this;
	}

	ks_basic_mutable_string& assign(size_t count, ELEM ch) {
		this->do_assign(count, ch, true, true);
		return *this;
	}

	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	ks_basic_mutable_string& assign(RIGHT&& right) {
		//if right is a xmutable-string, do assign directly, so this will ref right.data
		if (std::is_base_of_v<ks_basic_xmutable_string_base<ELEM>, std::remove_cv_t<std::remove_reference_t<RIGHT>>>)
			*this = ks_basic_mutable_string(std::forward<RIGHT>(right));
		else if (std::is_same_v<RIGHT, std::basic_string<ELEM, ks_char_traits<ELEM>, ks_basic_string_allocator<ELEM>>&&>)
			*this = ks_basic_mutable_string(std::forward<RIGHT>(right));
		else
			this->do_assign(__to_basic_string_view(right), true);
		return *this;
	}
	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	ks_basic_mutable_string& assign(RIGHT&& right, size_t offset, size_t count = -1) {
		//if right is a xmutable-string, do assign directly, so this will ref right.data
		if (std::is_base_of_v<ks_basic_xmutable_string_base<ELEM>, std::remove_cv_t<std::remove_reference_t<RIGHT>>>)
			*this = ks_basic_mutable_string(std::forward<RIGHT>(right), offset, count);
		else if (std::is_same_v<RIGHT, std::basic_string<ELEM, ks_char_traits<ELEM>, ks_basic_string_allocator<ELEM>>&&>)
			*this = ks_basic_mutable_string(std::forward<RIGHT>(right), offset, count);
		else
			this->do_assign(__to_basic_string_view(right, offset, count), true);
		return *this;
	}

	//append...
	ks_basic_mutable_string& append(const ELEM* p) {
		this->do_append(__to_basic_string_view(p), true);
		return *this;
	}
	ks_basic_mutable_string& append(const ELEM* p, size_t count) {
		this->do_append(__to_basic_string_view(p, count), true);
		return *this;
	}

	ks_basic_mutable_string& append(const ks_basic_string_view<ELEM>& str_view) {
		this->do_append(str_view, true);
		return *this;
	}
	ks_basic_mutable_string& append(const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count = -1) {
		this->do_append(str_view.substr(offset, count), true);
		return *this;
	}

	ks_basic_mutable_string& append(size_t count, ELEM ch) {
		this->do_append(count, ch, true, true);
		return *this;
	}

	//insert... (by pos)
	ks_basic_mutable_string& insert(size_t pos, const ELEM* p) {
		this->do_insert(pos, __to_basic_string_view(p), true);
		return *this;
	}
	ks_basic_mutable_string& insert(size_t pos, const ELEM* p, size_t count) {
		this->do_insert(pos, __to_basic_string_view(p, count), true);
		return *this;
	}

	ks_basic_mutable_string& insert(size_t pos, const ks_basic_string_view<ELEM>& str_view) {
		this->do_insert(pos, str_view, true);
		return *this;
	}
	ks_basic_mutable_string& insert(size_t pos, const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count = -1) {
		this->do_insert(pos, str_view.substr(offset, count), true);
		return *this;
	}

	ks_basic_mutable_string& insert(size_t pos, size_t count, ELEM ch) {
		this->do_insert(pos, count, ch, true, true);
		return *this;
	}

	//insert... (by iter)
	iterator insert(const_iterator iter, const ELEM* p) {
		size_t pos = iter - this->cbegin();
		this->insert(pos, p);
		return this->begin() + pos;
	}
	iterator insert(const_iterator iter, const ELEM* p, size_t count) {
		size_t pos = iter - this->cbegin();
		this->insert(pos, p, count);
		return this->begin() + pos;
	}

	iterator insert(const_iterator iter, const ks_basic_string_view<ELEM>& str_view) {
		size_t pos = iter - this->cbegin();
		this->insert(pos, str_view);
		return this->begin() + pos;
	}
	iterator insert(const_iterator iter, const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count = -1) {
		size_t pos = iter - this->cbegin();
		this->insert(pos, str_view, offset, count);
		return this->begin() + pos;
	}

	iterator insert(const_iterator iter, size_t count, ELEM ch) {
		size_t pos = iter - this->cbegin();
		this->insert(pos, count, ch);
		return this->begin() + pos;
	}

	//replace... (by pos)
	ks_basic_mutable_string& replace(size_t pos, size_t number, const ELEM* p) {
		this->do_replace(pos, number, __to_basic_string_view(p), true);
		return *this;
	}
	ks_basic_mutable_string& replace(size_t pos, size_t number, const ELEM* p, size_t count) {
		this->do_replace(pos, number, __to_basic_string_view(p, count), true);
		return *this;
	}

	ks_basic_mutable_string& replace(size_t pos, size_t number, const ks_basic_string_view<ELEM>& str_view) {
		this->do_replace(pos, number, str_view, true);
		return *this;
	}
	ks_basic_mutable_string& replace(size_t pos, size_t number, const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count = -1) {
		this->do_replace(pos, number, str_view.substr(offset, count), true);
		return *this;
	}

	ks_basic_mutable_string& replace(size_t pos, size_t number, size_t count, ELEM ch) {
		this->do_replace(pos, number, count, ch, true, true);
		return *this;
	}

	//replace... (by iter)
	iterator replace(const_iterator first, const_iterator last, const ELEM* p) {
		size_t pos = first - this->cbegin();
		size_t number = last - first;
		this->replace(pos, number, p);
		return this->begin() + pos;
	}
	iterator replace(const_iterator first, const_iterator last, const ELEM* p, size_t count) {
		size_t pos = first - this->cbegin();
		size_t number = last - first;
		this->replace(pos, number, p, count);
		return this->begin() + pos;
	}

	iterator replace(const_iterator first, const_iterator last, const ks_basic_string_view<ELEM>& str_view) {
		size_t pos = first - this->cbegin();
		size_t number = last - first;
		this->replace(pos, number, str_view);
		return this->begin() + pos;
	}
	iterator replace(const_iterator first, const_iterator last, const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count = -1) {
		size_t pos = first - this->cbegin();
		size_t number = last - first;
		this->replace(pos, number, str_view, offset, count);
		return this->begin() + pos;
	}

	iterator replace(const_iterator first, const_iterator last, size_t count, ELEM ch) {
		size_t pos = first - this->cbegin();
		size_t number = last - first;
		this->replace(pos, number, count, ch, true, true);
		return this->begin() + pos;
	}

	//substitute...
	ks_basic_mutable_string& substitute(const ks_basic_string_view<ELEM>& old_str, const ks_basic_string_view<ELEM>& new_str) {
		this->do_substitute_n(old_str, new_str, size_t(-1), true);
		return *this;
	}

	ks_basic_mutable_string& substitute(ELEM old_ch, const ELEM new_ch) {
		this->do_substitute_n(__to_basic_string_view(&old_ch, 1), __to_basic_string_view(&new_ch, 1), size_t(-1), true);
		return *this;
	}

	ks_basic_mutable_string& substitute_n(const ks_basic_string_view<ELEM>& old_str, const ks_basic_string_view<ELEM>& new_str, size_t n = -1) {
		this->do_substitute_n(old_str, new_str, n, true);
		return *this;
	}

	ks_basic_mutable_string& substitute_n(ELEM old_ch, const ELEM new_ch, size_t n = -1) {
		this->do_substitute_n(__to_basic_string_view(&old_ch, 1), __to_basic_string_view(&new_ch, 1), n, true);
		return *this;
	}

	//fill...
	ks_basic_mutable_string& fill(size_t pos, size_t number, ELEM ch) {
		const size_t this_length = this->length();
		if (pos > this_length)
			throw std::out_of_range("ks_basic_mutable_string::fill(pos, number, ch) out-of-range exception");
		if (ptrdiff_t(number) < 0)
			number = this_length - pos;

		if (number == 1) {
			this->set_at(pos, ch);
		}
		else if (number != 0) {
			if (pos + number > this_length || number > this_length)
				throw std::out_of_range("ks_basic_mutable_string::fill(pos, number, ch) out-of-range exception");
			if (this->view().unsafe_subview(pos, number).find_first_not_of(ch) != -1) {
				this->do_ensure_exclusive();
				std::fill_n(this->unsafe_data(), number, ch);
			}
		}

		return *this;
	}

	iterator fill(const_iterator first, const_iterator last, ELEM ch) {
		size_t pos = first - this->cbegin();
		size_t number = last - first;
		this->fill(pos, number, ch);
		return this->begin() + pos;
	}

	//erase...
	ks_basic_mutable_string& erase(size_t pos, size_t number) {
		this->do_erase(pos, number, true);
		return *this;
	}

	iterator erase(const_iterator first, const_iterator last) {
		size_t pos = first - this->cbegin();
		size_t number = last - first;
		this->erase(pos, number);
		return this->begin() + pos;
	}

	iterator erase(const_iterator iter) {
		size_t pos = iter - this->cbegin();
		this->erase(pos, 1);
		return this->begin() + pos;
	}

	//push-back, pop-back, set-at
	void push_back(ELEM ch) {
		this->do_append(1, ch, true, true);
	}

	void pop_back() {
		this->do_erase(this->length() - 1, this->length(), true);
	}

	void set_at(size_t pos, ELEM ch) {
		const size_t this_length = this->length();
		if (pos >= this_length)
			throw std::out_of_range("ks_basic_mutable_string::set_at(pos, ch) out-of-range exception");
		if (this->data()[pos] != ch) {
			this->do_ensure_exclusive();
			this->unsafe_data()[pos] = ch;
		}
	}

	//clear
	void clear() {
		this->do_clear(true);
	}

	//resize & reserve
	void resize(size_t count, ELEM ch = ELEM{}) {
		this->do_resize(count, ch, true, true);
		this->do_ensure_exclusive(); //for compatibility, ensure exclusive！
	}

	void reserve(size_t capa) {
		this->do_reserve(capa);
		this->do_ensure_exclusive(); //for compatibility, ensure exclusive！
	}

	//exclusive
	ELEM* __begin_exclusive_writing(size_t capa) {
		this->do_resize(capa, ELEM{}, false, false);
		this->do_ensure_exclusive();
		return this->unsafe_data();
	}

	void __end_exclusive_writing(ELEM* p, size_t count) {
		if (!this->is_exclusive())
			throw std::logic_error("ks_basic_mutable_string::end_exclusive_writing(p, count) non-exclusive exception");
		if (p != this->data() || count > this->length())
			throw std::out_of_range("ks_basic_mutable_string::end_exclusive_writing(p, count) out-of-range exception");
		this->do_resize(count, ELEM{}, true, false);
		this->do_ensure_end_ch0(true);
	}

	void __ensure_exclusive_writable_directly(size_t count) {
		this->do_resize(count, ELEM{}, true, true);
		this->do_ensure_exclusive();
	}

public:
	void trim() {
		this->do_trim(true);
	}
	void trim_left() {
		this->do_trim_left(true);
	}
	void trim_right() {
		this->do_trim_right(true);
	}

	void shrink_to_fit() {
		this->do_shrink();
		ASSERT(this->do_check_end_ch0());
	}

public:
	//注：for optimization, use immutable-string as return-type
	std::vector<ks_basic_immutable_string<ELEM>> split(const ks_basic_string_view<ELEM>& sep, size_t n = -1) const {
		return this->template do_split<ks_basic_immutable_string<ELEM>>(sep, n);
	}

public:
	//注：for optimization, use immutable-string as return-type
	ks_basic_immutable_string<ELEM> slice(size_t from, size_t to = size_t(-1)) const& { return this->to_immutable().slice(from, to); }
	ks_basic_immutable_string<ELEM> slice(size_t from, size_t to = size_t(-1))&& { return this->detach_to_immutable().slice(from, to); }

	ks_basic_immutable_string<ELEM> substr(size_t offset, size_t count = size_t(-1)) const& { return this->to_immutable().substr(offset, count); }
	ks_basic_immutable_string<ELEM> substr(size_t offset, size_t count = size_t(-1))&& { return this->detach_to_immutable().substr(offset, count); }

	ks_basic_immutable_string<ELEM> slice(const_iterator from, const_iterator to) const& { size_t from_pos = from - this->cbegin(), to_pos = to - this->cbegin(); return this->to_immutable().slice(from_pos, to_pos); }
	ks_basic_immutable_string<ELEM> slice(const_iterator from, const_iterator to)&& { size_t from_pos = from - this->cbegin(), to_pos = to - this->cbegin(); return this->detach_to_immutable().slice(from_pos, to_pos); }

	ks_basic_immutable_string<ELEM> substr(const_iterator from, const_iterator to) const& { size_t offset = from - this->cbegin(), count = to - from; return this->to_immutable().substr(offset, count); }
	ks_basic_immutable_string<ELEM> substr(const_iterator from, const_iterator to)&& { size_t offset = from - this->cbegin(), count = to - from; return this->detach_to_immutable().substr(offset, count); }

	ks_basic_immutable_string<ELEM> trimmed() const& { return this->to_immutable().trimmed(); }
	ks_basic_immutable_string<ELEM> trimmed()&& { return this->detach_to_immutable().trimmed(); }

	ks_basic_immutable_string<ELEM> shrunk() const& { return this->to_immutable().shrunk(); }
	ks_basic_immutable_string<ELEM> shrunk()&& { return this->detach_to_immutable().shrunk(); }

	const ELEM* c_str() const {
		ASSERT(this->do_check_end_ch0());
		return this->data();
	}

public:
	ks_basic_immutable_string<ELEM> to_immutable() const& { return ks_basic_immutable_string<ELEM>(*this); }
	ks_basic_immutable_string<ELEM> to_immutable()&& { return ks_basic_immutable_string<ELEM>(this->detach()); }

	ks_basic_mutable_string detach() { return ks_basic_mutable_string(std::move(*this)); }
	ks_basic_immutable_string<ELEM> detach_to_immutable() { return ks_basic_immutable_string<ELEM>(this->detach()); }

public:
	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	ks_basic_mutable_string& operator+=(RIGHT&& right) {
		this->do_self_add(std::forward<RIGHT>(right), false, true);
		return *this;
	}

	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	ks_basic_immutable_string<ELEM> operator+(RIGHT&& right) const& {
		return this->to_immutable() + right;
	}

	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	ks_basic_immutable_string<ELEM> operator+(RIGHT&& right)&& {
		return this->detach_to_immutable() + right;
	}
};


template <class ELEM, class LEFT, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline ks_basic_immutable_string<ELEM> operator+(const LEFT& left, const ks_basic_mutable_string<ELEM>& right) {
	return left + right.to_immutable();
}

template <class ELEM, class LEFT, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline ks_basic_immutable_string<ELEM> operator+(const LEFT& left, ks_basic_mutable_string<ELEM>&& right) {
	return left + right.detach_to_immutable();
}


namespace std {
	template <class ELEM>
	struct hash<ks_basic_mutable_string<ELEM>> : hash<ks_basic_xmutable_string_base<ELEM>> {
	};
}


template <class ELEM>
std::basic_ostream<ELEM, std::char_traits<ELEM>>& operator<<(std::basic_ostream<ELEM, std::char_traits<ELEM>>& strm, const ks_basic_mutable_string<ELEM>& str) {
	return strm << str.view();
}

template <class ELEM>
std::basic_istream<ELEM, std::char_traits<ELEM>>& operator>>(std::basic_istream<ELEM, std::char_traits<ELEM>>& strm, ks_basic_mutable_string<ELEM>& str) {
	std::basic_string<ELEM, std::char_traits<ELEM>, ks_basic_string_allocator<ELEM>> std_str;
	strm >> std_str;
	str = ks_basic_mutable_string<ELEM>(std::move(std_str));
	return strm;
}
