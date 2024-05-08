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
#include "ks_basic_xmutable_string_base.inl"


template <class ELEM>
class ks_basic_immutable_string : public ks_basic_xmutable_string_base<ELEM> {
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
	ks_basic_immutable_string() : __my_string_base() {}
	ks_basic_immutable_string(const ELEM* p) : __my_string_base(p) {}
	ks_basic_immutable_string(const ELEM* p, size_t count) : __my_string_base(p, count) {}

	ks_basic_immutable_string(const ks_basic_string_view<ELEM>& str_view) : __my_string_base(str_view) {}
	ks_basic_immutable_string(const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count = -1)
		: __my_string_base(str_view.substr(offset, count)) {}

	ks_basic_immutable_string(size_t count, ELEM ch) : __my_string_base(count, ch) {}

	//copy & move ctor
	ks_basic_immutable_string(const ks_basic_immutable_string& other) = default;
	ks_basic_immutable_string& operator=(const ks_basic_immutable_string& other) = default;
	ks_basic_immutable_string(ks_basic_immutable_string&& other) noexcept = default;
	ks_basic_immutable_string& operator=(ks_basic_immutable_string&& other) noexcept = default;

	//copy & move ctor (from xmutable)
	ks_basic_immutable_string(const ks_basic_xmutable_string_base<ELEM>& other) 
		: __my_string_base(other) {}
	ks_basic_immutable_string(const ks_basic_xmutable_string_base<ELEM>& other, size_t offset, size_t count = -1)
		: __my_string_base(other.do_substr(offset, count)) {}
	ks_basic_immutable_string(ks_basic_xmutable_string_base<ELEM>&& other) 
		: __my_string_base(other.do_detach()) { ASSERT(other.is_detached_empty()); }
	ks_basic_immutable_string(ks_basic_xmutable_string_base<ELEM>&& other, size_t offset, size_t count = -1)
		: __my_string_base(other.do_detach().do_substr(offset, count)) { ASSERT(other.is_detached_empty()); }

	//copy & move ctor (from std::basic_string)
	template <class AllocType>
	ks_basic_immutable_string(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str) 
		: __my_string_base(__to_basic_string_view(str)) {}
	template <class AllocType>
	ks_basic_immutable_string(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str, size_t offset, size_t count = -1)
		: __my_string_base(__to_basic_string_view(str, offset, count)) {}

	ks_basic_immutable_string(std::basic_string<ELEM, std::char_traits<ELEM>, ks_basic_string_allocator<ELEM>>&& str_rvref) 
		: __my_string_base(std::move(str_rvref)) {}
	ks_basic_immutable_string(std::basic_string<ELEM, std::char_traits<ELEM>, ks_basic_string_allocator<ELEM>>&& str_rvref, size_t offset, size_t count = -1)
		: __my_string_base(__my_string_base(std::move(str_rvref)).substr(offset, count)) {}

public:
	std::vector<ks_basic_immutable_string> split(const ks_basic_string_view<ELEM>& sep, size_t n = -1) const {
		return this->template do_split<ks_basic_immutable_string>(sep, n);
	}

public:
	ks_basic_immutable_string slice(size_t from, size_t to) const& { return this->do_slice(from, to); }
	ks_basic_immutable_string slice(size_t from, size_t to)&& { return this->detach().do_slice(from, to); }

	ks_basic_immutable_string slice(const_iterator from, const_iterator to) const& { size_t from_pos = from - this->cbegin(), to_pos = to - this->cbegin(); return this->do_slice(from_pos, to_pos); }
	ks_basic_immutable_string slice(const_iterator from, const_iterator to)&& { size_t from_pos = from - this->cbegin(), to_pos = to - this->cbegin(); return this->detach().do_slice(from_pos, to_pos); }

	ks_basic_immutable_string substr(size_t pos, size_t count) const& { return this->do_substr(pos, count); }
	ks_basic_immutable_string substr(size_t pos, size_t count)&& { return this->detach().do_substr(pos, count); }

	ks_basic_immutable_string substr(const_iterator from, const_iterator to) const& { size_t offset = from - this->cbegin(), count = to - from; return this->do_substr(offset, count); }
	ks_basic_immutable_string substr(const_iterator from, const_iterator to)&& { size_t offset = from - this->cbegin(), count = to - from; return this->detach().do_substr(offset, count); }

	ks_basic_immutable_string trimmed() const& { ks_basic_immutable_string ret(*this); ret.do_trim(false); return ret; }
	ks_basic_immutable_string trimmed()&& { this->do_trim(false); return this->detach(); }

	ks_basic_immutable_string shrunk() const& { ks_basic_immutable_string ret(*this); ret.do_shrink(); return ret; }
	ks_basic_immutable_string shrunk()&& { this->do_shrink(); return this->detach(); }

public:
	ks_basic_mutable_string<ELEM> to_mutable() const& { return ks_basic_mutable_string<ELEM>(*this); }
	ks_basic_mutable_string<ELEM> to_mutable()&& { return ks_basic_mutable_string<ELEM>(this->detach()); }

	ks_basic_immutable_string detach() { return ks_basic_immutable_string(std::move(*this)); }
	ks_basic_mutable_string<ELEM> detach_to_mutable() { return ks_basic_mutable_string<ELEM>(this->detach()); }

public:
	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	ks_basic_immutable_string& operator+=(RIGHT&& right) {
		this->do_self_add(std::forward<RIGHT>(right), true, false);
		return *this;
	}

	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	ks_basic_immutable_string operator+(const ks_basic_string_view<ELEM>& right) const& {
		ks_basic_immutable_string ret(*this);
		ret.do_self_add(std::forward<RIGHT>(right), true, false);
		return ret;
	}

	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	ks_basic_immutable_string operator+(const ks_basic_string_view<ELEM>& right)&& {
		this->do_self_add(std::forward<RIGHT>(right), true, false);
		return this->detach();
	}
};


template <class ELEM, class LEFT, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline ks_basic_immutable_string<ELEM> operator+(const LEFT& left, const ks_basic_immutable_string<ELEM>& right) {
	ks_basic_immutable_string<ELEM> ret(right);
	const ks_basic_string_view<ELEM> left_view(left);
	if (!left_view.empty())
		ret = ret.detach_to_mutable().insert(0, left_view);
	return ret;
}

template <class ELEM, class LEFT, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline ks_basic_immutable_string<ELEM> operator+(const LEFT& left, ks_basic_immutable_string<ELEM>&& right) {
	ks_basic_immutable_string<ELEM> ret(std::move(right));
	const ks_basic_string_view<ELEM> left_view(left);
	if (!left_view.empty())
		ret = ret.detach_to_mutable().insert(0, left_view);
	return ret;
}


namespace std {
	template <class ELEM>
	struct hash<ks_basic_immutable_string<ELEM>> : hash<ks_basic_xmutable_string_base<ELEM>> {
	};
}
