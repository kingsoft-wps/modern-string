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

#include <iterator>


template <class ELEM>
struct MODERN_STRING_INLINE_API ks_basic_pointer_iterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = ptrdiff_t;
	using value_type = ELEM;
	using pointer = ELEM*;
	using reference = ELEM&;

public:
	ks_basic_pointer_iterator() noexcept = default;
	ks_basic_pointer_iterator(const ks_basic_pointer_iterator&) noexcept = default;
	ks_basic_pointer_iterator& operator=(const ks_basic_pointer_iterator&) noexcept = default;
	ks_basic_pointer_iterator(ks_basic_pointer_iterator&&) noexcept = default;
	ks_basic_pointer_iterator& operator=(ks_basic_pointer_iterator&&) noexcept = default;

	template <class ELEM2, class _ = std::enable_if_t<std::is_const_v<ELEM> && !std::is_const_v<ELEM2>>>
	ks_basic_pointer_iterator(const ks_basic_pointer_iterator<ELEM2>& r) noexcept : _p(&(*r)) {
		static_assert(std::is_const_v<ELEM> && !std::is_const_v<ELEM2>, "the type of both ELEMs must be consistent");
	}

	explicit ks_basic_pointer_iterator(ELEM* p) noexcept : _p(p) {}

public:
	pointer operator->() const { return _p; }
	reference operator*() const { return *_p; }

	ks_basic_pointer_iterator& operator++() { ++_p; return *this; }
	ks_basic_pointer_iterator& operator--() { --_p; return *this; }
	ks_basic_pointer_iterator  operator++(int) { return ks_basic_pointer_iterator(_p++); }
	ks_basic_pointer_iterator  operator--(int) { return ks_basic_pointer_iterator(_p--); }

	ks_basic_pointer_iterator  operator+(const difference_type offs) const { return ks_basic_pointer_iterator(_p + offs); }
	ks_basic_pointer_iterator  operator-(const difference_type offs) const { return ks_basic_pointer_iterator(_p - offs); }
	ks_basic_pointer_iterator& operator+=(const difference_type offs) { _p += offs; return *this; }
	ks_basic_pointer_iterator& operator-=(const difference_type offs) { _p -= offs; return *this; }

	difference_type operator-(const ks_basic_pointer_iterator& r) const noexcept { return _p - r._p; }
	reference operator[](const difference_type offs) const { return *(*this + offs); }

	bool operator<(const ks_basic_pointer_iterator& r) const noexcept { return _p < r._p; }
	bool operator>(const ks_basic_pointer_iterator& r) const noexcept { return _p > r._p; }
	bool operator<=(const ks_basic_pointer_iterator& r) const noexcept { return _p <= r._p; }
	bool operator>=(const ks_basic_pointer_iterator& r) const noexcept { return _p >= r._p; }
	bool operator==(const ks_basic_pointer_iterator& r) const noexcept { return _p == r._p; }
	bool operator!=(const ks_basic_pointer_iterator& r) const noexcept { return _p != r._p; }

private:
	ELEM* _p = nullptr;
};
