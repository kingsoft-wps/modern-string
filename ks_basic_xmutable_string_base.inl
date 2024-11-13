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
#include "ks_basic_string_allocator.inl"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <ostream>

template <class ELEM>
class ks_basic_mutable_string;
template <class ELEM>
class ks_basic_immutable_string;


template <class ELEM>
class ks_basic_xmutable_string_base {
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
	//def ctor
	ks_basic_xmutable_string_base() {
		if (sizeof(_SSO_STRUCT) / 8 != 0)
			std::fill_n((uint64_t*)(_my_sso_ptr()), sizeof(_SSO_STRUCT) / 8, 0);
		if (sizeof(_SSO_STRUCT) % 8 != 0)
			std::fill_n((uint8_t*)(_my_sso_ptr()) + sizeof(_SSO_STRUCT) - sizeof(_SSO_STRUCT) % 8, sizeof(_SSO_STRUCT) % 8, 0);
	}

	//copy & move ctor
	ks_basic_xmutable_string_base(const ks_basic_xmutable_string_base& other) {
		if (other.is_sso_mode()) {
			*_my_sso_ptr() = *other._my_sso_ptr();
		}
		else {
			*_my_ref_ptr() = *other._my_ref_ptr();
			ks_basic_string_allocator<ELEM>::_atomic_addref(_my_ref_ptr()->alloc_addr());
		}
	}

	ks_basic_xmutable_string_base(ks_basic_xmutable_string_base&& other) noexcept {
		if (other.is_sso_mode()) 
			*_my_sso_ptr() = *other._my_sso_ptr();
		else 
			*_my_ref_ptr() = *other._my_ref_ptr();
		::new (&other) ks_basic_xmutable_string_base{};
	}

	ks_basic_xmutable_string_base& operator=(const ks_basic_xmutable_string_base& other) {
		if (this != &other) {
			if (other.is_sso_mode()) {
				this->~ks_basic_xmutable_string_base();
				*_my_sso_ptr() = *other._my_sso_ptr();
			}
			else {
				if (_my_ref_ptr()->alloc_addr() == other._my_ref_ptr()->alloc_addr()) {
					*_my_ref_ptr() = *other._my_ref_ptr();
				}
				else {
					this->~ks_basic_xmutable_string_base();
					*_my_ref_ptr() = *other._my_ref_ptr();
					ks_basic_string_allocator<ELEM>::_atomic_addref(_my_ref_ptr()->alloc_addr());
				}
			}
		}
		return *this;
	}

	ks_basic_xmutable_string_base& operator=(ks_basic_xmutable_string_base&& other) noexcept {
		ASSERT(this != &other);
		this->~ks_basic_xmutable_string_base();
		if (other.is_sso_mode()) 
			*_my_sso_ptr() = *other._my_sso_ptr();
		else 
			*_my_ref_ptr() = *other._my_ref_ptr();
		::new (&other) ks_basic_xmutable_string_base{};
		return *this;
	}

	//dtor
	~ks_basic_xmutable_string_base() {
		if (this->is_ref_mode()) {
			ks_basic_string_allocator<ELEM>::_atomic_release(_my_ref_ptr()->alloc_addr());
		}
	}

protected:
	//explicit ctor
	explicit ks_basic_xmutable_string_base(const ELEM* p) : ks_basic_xmutable_string_base(__to_basic_string_view(p)) {}
	explicit ks_basic_xmutable_string_base(const ELEM* p, size_t count) : ks_basic_xmutable_string_base(__to_basic_string_view(p, count)) {}

	explicit ks_basic_xmutable_string_base(const ks_basic_string_view<ELEM>& str_view) : ks_basic_xmutable_string_base() {
		const ELEM* p = str_view.data();
		size_t count = str_view.length();
		if (count > _STR_LENGTH_LIMIT)
			throw std::overflow_error("ks_basic_xmutable_string_base(p, count) overflow exception");

		if (count <= _SSO_BUFFER_SPACE - 1) {
			auto* sso_ptr = _my_sso_ptr();
			sso_ptr->mode = _SSO_MODE;
			sso_ptr->length8 = uint8_t(count);
			std::copy_n(p, count, sso_ptr->buffer);
			sso_ptr->buffer[count] = 0;
		}
		else {
			ELEM* new_alloc_addr = ks_basic_string_allocator<ELEM>::_atomic_alloc(count + 1);
			std::copy_n(p, count, new_alloc_addr);
			new_alloc_addr[count] = 0;

			auto* ref_ptr = _my_ref_ptr();
			ref_ptr->mode = _REF_MODE;
			ref_ptr->offset32 = 0;
			ref_ptr->length32 = uint32_t(count);
			ref_ptr->p = new_alloc_addr;
		}
	}

	explicit ks_basic_xmutable_string_base(size_t count, ELEM ch) : ks_basic_xmutable_string_base() {
		if (count > _STR_LENGTH_LIMIT)
			throw std::overflow_error("ks_basic_xmutable_string_base(count, ch) overflow exception");

		if (count <= _SSO_BUFFER_SPACE - 1) {
			auto* sso_ptr = _my_sso_ptr();
			sso_ptr->mode = _SSO_MODE;
			sso_ptr->length8 = uint8_t(count);
			std::fill_n(sso_ptr->buffer, count, ch);
			sso_ptr->buffer[count] = 0;
		}
		else {
			ELEM* new_alloc_addr = ks_basic_string_allocator<ELEM>::_atomic_alloc(count + 1);
			std::fill_n(new_alloc_addr, count, ch);
			new_alloc_addr[count] = 0;

			auto* ref_ptr = _my_ref_ptr();
			ref_ptr->mode = _REF_MODE;
			ref_ptr->offset32 = 0;
			ref_ptr->length32 = uint32_t(count);
			ref_ptr->p = new_alloc_addr;
		}
	}

	explicit ks_basic_xmutable_string_base(std::basic_string<ELEM, std::char_traits<ELEM>, ks_basic_string_allocator<ELEM>>&& str_rvref) : ks_basic_xmutable_string_base() {
		if (str_rvref.length() > _STR_LENGTH_LIMIT)
			throw std::overflow_error("ks_basic_xmutable_string_base(&&str) overflow exception");

		ELEM* strdata_addr = (ELEM*)str_rvref.data();
		if (str_rvref.length() <= _SSO_BUFFER_SPACE - 1 ||
			(uintptr_t(strdata_addr) >= uintptr_t(&str_rvref) && uintptr_t(strdata_addr) < uintptr_t(&str_rvref) + sizeof(str_rvref))) {
			*this = ks_basic_xmutable_string_base(ks_basic_string_view<ELEM>(str_rvref));
			str_rvref.clear();
			str_rvref.shrink_to_fit(); //moved-like
		}
		else {
			ASSERT(strdata_addr[str_rvref.length()] == 0); //should have end-ch0 already
			ASSERT(strdata_addr[str_rvref.capacity()] == 0); //should have end-ch0 already
			ASSERT(ks_basic_string_allocator<ELEM>::_get_refcount32_value(strdata_addr) == 0);
			*(volatile uint32_t*)ks_basic_string_allocator<ELEM>::_get_refcount32_p(strdata_addr) = 1;

			auto* ref_ptr = _my_ref_ptr();
			ref_ptr->mode = _REF_MODE;
			ref_ptr->offset32 = 0;
			ref_ptr->length32 = uint32_t(str_rvref.length());
			ref_ptr->p = strdata_addr;

			::new (&str_rvref) std::basic_string<ELEM, std::char_traits<ELEM>, ks_basic_string_allocator<ELEM>>{}; //moved-like
		}
	}

	//detach-void
	ks_basic_xmutable_string_base do_detach() {
		ks_basic_xmutable_string_base ret(std::move(*this));
		this->do_detach_void();
		return ret;
	}

	void do_detach_void() {
		this->~ks_basic_xmutable_string_base();
		::new (this) ks_basic_xmutable_string_base{};
	}

public:
	iterator begin() const { return iterator{ this->data() }; }
	iterator end() const { return iterator{ this->data_end() }; }
	const_iterator cbegin() const { return const_iterator{ this->data() }; }
	const_iterator cend() const { return const_iterator{ this->data_end() }; }
	reverse_iterator rbegin() const { return reverse_iterator{ this->end() }; }
	reverse_iterator rend() const { return reverse_iterator{ this->begin() }; }
	const_reverse_iterator crbegin() const { return reverse_iterator{ this->cend() }; }
	const_reverse_iterator crend() const { return reverse_iterator{ this->cbegin() }; }

protected:
	bool do_check_end_ch0() const { return this->data()[this->length()] == 0; }
	void do_ensure_end_ch0(bool ensure_end_ch0) {
		if (ensure_end_ch0 && !this->do_check_end_ch0()) {
			this->do_ensure_exclusive();
			this->unsafe_data()[this->length()] = 0;
		}
	}

	void do_ensure_exclusive() {
		if (!this->is_exclusive()) {
			ELEM* forked_alloc_addr = ks_basic_string_allocator<ELEM>::_atomic_alloc(this->capacity() + 1);
			std::copy_n(this->data(), this->length(), forked_alloc_addr);
			std::fill_n(forked_alloc_addr + this->length(), this->capacity() - this->length() + 1, 0);

			ks_basic_xmutable_string_base forked;
			auto* forked_ref_ptr = forked._my_ref_ptr();
			forked_ref_ptr->mode = _REF_MODE;
			forked_ref_ptr->offset32 = 0;
			forked_ref_ptr->length32 = uint32_t(this->length());
			forked_ref_ptr->p = forked_alloc_addr;

			*this = std::move(forked);
		}
	}

	bool do_determine_need_grow(size_t grow) { return ptrdiff_t(grow) > 0 && this->length() + grow > this->capacity(); }
	void do_auto_grow(size_t grow) {
		if (this->do_determine_need_grow(grow)) {
			size_t new_capa = std::max(this->length() + grow, this->capacity() + this->capacity() / 2);
			if (new_capa > _STR_LENGTH_LIMIT) {
				new_capa = _STR_LENGTH_LIMIT;
				if (new_capa < this->length() + grow)
					throw std::overflow_error("ks_basic_xmutable_string_base::grow(capa) overflow exception");
			}

			this->do_reserve(new_capa);
		}
	}

	void do_reserve(size_t capa) {
		if (capa > this->capacity()) {
			size_t new_capa = capa;
			if (new_capa > _STR_LENGTH_LIMIT) {
				new_capa = _STR_LENGTH_LIMIT;
				if (new_capa < capa)
					throw std::overflow_error("ks_basic_xmutable_string_base::reserve(capa) overflow exception");
			}

			if (new_capa <= _SSO_BUFFER_SPACE - 1) {
				*this = ks_basic_xmutable_string_base(this->data(), this->length());
			}
			else {
				ELEM* grown_alloc_addr = ks_basic_string_allocator<ELEM>::_atomic_alloc(new_capa + 1);
				std::copy_n(this->data(), this->length(), grown_alloc_addr);
				std::fill_n(grown_alloc_addr + this->length(), new_capa - this->length() + 1, 0);

				ks_basic_xmutable_string_base grown;
				auto* grown_ref_ptr = grown._my_ref_ptr();
				grown_ref_ptr->mode = _REF_MODE;
				grown_ref_ptr->offset32 = 0;
				grown_ref_ptr->length32 = uint32_t(this->length());
				grown_ref_ptr->p = grown_alloc_addr;

				*this = std::move(grown);
			}
		}
	}

	void do_resize(size_t count, ELEM ch, bool ch_valid, bool ensure_end_ch0) {
		size_t old_length = this->length();
		if (count < old_length)
			return this->do_erase(count, old_length - count, ensure_end_ch0);
		else if (count > old_length)
			return this->do_append(count - old_length, ch, ch_valid, ensure_end_ch0);
	}

	void do_trim(bool ensure_end_ch0) {
		const auto this_view = this->view();
		auto trimmed_view = this_view.trimmed();
		if (trimmed_view.length() != this_view.length()) {
			*this = this->unsafe_substr(trimmed_view.data() - this_view.data(), trimmed_view.length());
			this->do_ensure_end_ch0(ensure_end_ch0);
		}
	}
	void do_trim_left(bool ensure_end_ch0) {
		const auto this_view = this->view();
		auto trimmed_view = this_view.trim_left();
		if (trimmed_view.length() != this_view.length()) {
			*this = this->unsafe_substr(trimmed_view.data() - this_view.data(), trimmed_view.length());
			this->do_ensure_end_ch0(ensure_end_ch0);
		}
	}
	void do_trim_right(bool ensure_end_ch0) {
		const auto this_view = this->view();
		auto trimmed_view = this_view.trim_right();
		if (trimmed_view.length() != this_view.length()) {
			*this = this->unsafe_substr(trimmed_view.data() - this_view.data(), trimmed_view.length());
			this->do_ensure_end_ch0(ensure_end_ch0);
		}
	}

	void do_shrink() {
		if (this->is_ref_mode()) {
			auto* ref_ptr = _my_ref_ptr();
			if (ref_ptr->offset32 != 0 ||
				ref_ptr->offset32 + ref_ptr->length32 != ks_basic_string_allocator<ELEM>::_get_space32_value(ref_ptr->alloc_addr()) - 1) {
				*this = ks_basic_xmutable_string_base(this->data(), this->length());
			}
		}
	}

protected:
	void do_assign(const ks_basic_string_view<ELEM>& str_view, bool ensure_end_ch0) {
		if (str_view.empty())
			return this->do_clear(ensure_end_ch0);

		const auto this_whole_view = this->unsafe_whole_view();
		if (str_view.is_subview_of(this_whole_view.unsafe_subview(0, this_whole_view.length() - 1))) {
			//if strview is subview of this.wholeview, we can return the subview directly
			*this = this->unsafe_substr(str_view.data() - this->data(), str_view.length());
			this->do_ensure_end_ch0(ensure_end_ch0);
			return;
		}

		//performer
		auto do_assign_imp = [this](const ks_basic_string_view<ELEM>& str_view, bool ensure_end_ch0) -> void {
			this->do_clear(false);
			this->do_append(str_view, ensure_end_ch0);
		};

		if (str_view.is_overlapped_with(this_whole_view)) {
			//here is doning simple fault tolerance, because checking has be done above
			ASSERT(false);
			std::basic_string<ELEM> strview_dup(str_view.data(), str_view.length());
			return do_assign_imp(__to_basic_string_view(strview_dup), ensure_end_ch0);
		}
		else {
			return do_assign_imp(str_view, ensure_end_ch0);
		}
	}

	void do_assign(size_t count, ELEM ch, bool ch_valid, bool ensure_end_ch0) {
		this->do_clear(false);
		this->do_append(ch, ch_valid, ensure_end_ch0);
	}

	void do_append(const ks_basic_string_view<ELEM>& str_view, bool ensure_end_ch0) {
		this->do_insert(this->length(), str_view, ensure_end_ch0);
	}

	void do_append(size_t count, ELEM ch, bool ch_valid, bool ensure_end_ch0) {
		this->do_insert(this->length(), count, ch, ch_valid, ensure_end_ch0);
	}

	void do_insert(size_t pos, const ks_basic_string_view<ELEM>& str_view, bool ensure_end_ch0) {
		if (pos > this->length())
			throw std::out_of_range("ks_basic_xmutable_string_base::insert(pos, ...) out-of-range exception");
		if (str_view.empty())
			return;

		bool is_argview_safe = true;
		const auto this_whole_view = this->unsafe_whole_view();
		if (this->is_exclusive() && str_view.is_overlapped_with(this_whole_view.unsafe_subview(0, this_whole_view.length() + str_view.length()))) {
			if (this->do_determine_need_grow(str_view.length())) {
				//if strview is overlapped with this.wholeview, and this is exclusive, and need growing, it means that this.data will invalidate after growing, so we need clone strview
				is_argview_safe = false;
			}
			else if (str_view.is_overlapped_with(this->view().unsafe_subview(pos, this->length() + str_view.length() - pos))) {
				//if strview is overlapped with writing this.subview, and this is exclusive, and need growing, it means that this.data will invalidate after growing, so we need clone strview
				is_argview_safe = false;
			}
		}

		auto do_insert_imp = [this, pos](const ks_basic_string_view<ELEM>& str_view, bool ensure_end_ch0) -> void {
			this->do_auto_grow(str_view.length());
			this->do_ensure_exclusive();

			std::move_backward(this->data() + pos, this->data() + pos + str_view.length(), this->unsafe_data_end() + str_view.length());
			std::copy_n(str_view.data(), str_view.length(), this->unsafe_data() + pos);

			if (this->is_sso_mode())
				this->_my_sso_ptr()->length8 += uint8_t(str_view.length());
			else
				this->_my_ref_ptr()->length32 += uint32_t(str_view.length());

			this->do_ensure_end_ch0(ensure_end_ch0);
		};

		if (!is_argview_safe) {
			std::basic_string<ELEM> strview_dup(str_view.data(), str_view.length());
			return do_insert_imp(__to_basic_string_view(strview_dup), ensure_end_ch0);
		}
		else {
			return do_insert_imp(str_view, ensure_end_ch0);
		}
	}

	void do_insert(size_t pos, size_t count, ELEM ch, bool ch_valid, bool ensure_end_ch0) {
		if (pos > this->length())
			throw std::out_of_range("ks_basic_xmutable_string_base::insert(pos, ...) out-of-range exception");
		if (count == 0)
			return;

		this->do_auto_grow(count);
		this->do_ensure_exclusive();

		std::move_backward(this->data() + pos, this->data() + pos + count, this->unsafe_data_end() + count);

		if (ch_valid)
			std::fill_n(this->unsafe_data() + pos, count, ch);

		if (this->is_sso_mode())
			_my_sso_ptr()->length8 += uint8_t(count);
		else
			_my_ref_ptr()->length32 += uint32_t(count);

		this->do_ensure_end_ch0(ensure_end_ch0);
	}

	void do_replace(size_t pos, size_t number, const ks_basic_string_view<ELEM>& str_view, bool ensure_end_ch0) {
		if (ptrdiff_t(number) < 0)
			number = this->length() - pos;

		const size_t pos_end = pos + number;
		const ptrdiff_t len_delta = (ptrdiff_t)(str_view.length() - number);
		if (pos > pos_end || pos_end > this->length())
			throw std::out_of_range("ks_basic_xmutable_string_base::replace(pos, number, ...) out-of-range exception");
		if (number == 0)
			return this->do_insert(pos, str_view, ensure_end_ch0);
		if (str_view.empty())
			return this->do_erase(pos, number, ensure_end_ch0);

		bool is_argview_safe = true;
		const auto this_whole_view = this->unsafe_whole_view();
		if (this->is_exclusive() && str_view.is_overlapped_with(this_whole_view.unsafe_subview(0, this_whole_view.length() + std::max(len_delta, ptrdiff_t(0))))) {
			if (len_delta > 0 && this->do_determine_need_grow(len_delta)) {
				//if strview is overlapped with this.wholeview, and this is exclusive, and need growing, it means that this.data will invalidate after growing, so we need clone strview
				is_argview_safe = false;
			}
			else if (str_view.is_overlapped_with(this->view().unsafe_subview(pos, this->length() + std::max(len_delta, ptrdiff_t(0)) - pos))) {
				//if strview is overlapped with writing this.subview, and this is exclusive, and need growing, it means that this.data will invalidate after growing, so we need clone strview
				is_argview_safe = false;
			}
		}

		auto do_replace_imp = [this, pos, pos_end, len_delta](const ks_basic_string_view<ELEM>& str_view, bool ensure_end_ch0) -> void {
			if (len_delta > 0)
				this->do_auto_grow(len_delta);
			this->do_ensure_exclusive();

			if (len_delta < 0)
				std::move(this->data() + pos_end, this->data_end(), this->unsafe_data() + pos_end + len_delta);
			else if (len_delta > 0)
				std::move_backward(this->data() + pos_end, this->data_end(), this->unsafe_data() + pos_end + len_delta);

			std::copy_n(str_view.data(), str_view.length(), this->unsafe_data() + pos);

			if (this->is_sso_mode())
				this->_my_sso_ptr()->length8 += int8_t(len_delta);
			else
				this->_my_ref_ptr()->length32 += int32_t(len_delta);

			this->do_ensure_end_ch0(ensure_end_ch0);
		};

		if (!is_argview_safe) {
			std::basic_string<ELEM> strview_dup(str_view.data(), str_view.length());
			return do_replace_imp(__to_basic_string_view(strview_dup), ensure_end_ch0);
		}
		else {
			return do_replace_imp(str_view, ensure_end_ch0);
		}
	}

	void do_replace(size_t pos, size_t number, size_t count, ELEM ch, bool ch_valid, bool ensure_end_ch0) {
		if (ptrdiff_t(number) < 0)
			number = this->length() - pos;

		size_t pos_end = pos + number;
		if (pos > pos_end || pos_end > this->length())
			throw std::out_of_range("ks_basic_xmutable_string_base::replace(pos, number, ...) out-of-range exception");
		if (number == 0)
			return this->do_insert(pos, count, ch, ch_valid, ensure_end_ch0);
		if (count == 0)
			return this->do_erase(pos, number, ensure_end_ch0);

		ptrdiff_t len_delta = (ptrdiff_t)(count - number);

		if (len_delta > 0)
			this->do_auto_grow(len_delta);
		this->do_ensure_exclusive();

		if (len_delta < 0)
			std::move(this->data() + pos_end, this->data_end(), this->unsafe_data() + pos_end + len_delta);
		else if (len_delta > 0)
			std::move_backward(this->data() + pos_end, this->data_end(), this->unsafe_data() + pos_end + len_delta);

		if (ch_valid)
			std::fill_n(this->unsafe_data() + pos, count, ch);

		if (this->is_sso_mode())
			_my_sso_ptr()->length8 += int8_t(len_delta);
		else
			_my_ref_ptr()->length32 += int32_t(len_delta);

		this->do_ensure_end_ch0(ensure_end_ch0);
	}

	void do_substitute(const ks_basic_string_view<ELEM>& sub, const ks_basic_string_view<ELEM>& replacement, size_t n, bool ensure_end_ch0) {
		if (n == 0 || sub.empty())
			return;

		//find first match
		size_t pos = this->find(sub);
		if (ptrdiff_t(pos) < 0)
			return;
		//after first match being found, we check whether sub and replacement are equal, and if they are, we need do nothing.
		if (sub == replacement)
			return;

		//collect all metches
		std::basic_string<uint32_t> pos32_list{ uint32_t(pos) };
		pos += sub.length();
		for (size_t i = 1; i < n; ++i) {
			pos = this->find(sub, pos);
			if (ptrdiff_t(pos) < 0)
				break;
			pos32_list.push_back(uint32_t(pos));
			pos += sub.length();
		}

		//no match
		if (pos32_list.empty())
			return;
		//one match only
		if (pos32_list.size() == 1)
			return this->do_replace(pos32_list[0], sub.length(), replacement, ensure_end_ch0);

		//multiple metches ...
		ptrdiff_t len_delta_total = (ptrdiff_t)(replacement.length() - sub.length()) * (ptrdiff_t)(pos32_list.size());
		//if the final len is 0, we can do clear simply
		if (len_delta_total > 0 && size_t(len_delta_total) == this->length())
			return this->do_clear(ensure_end_ch0);

		//check repview safe
		bool is_replacement_safe = true;
		if (len_delta_total == 0) {
			//the len won't change ...
			const auto this_view = this->view();
			if (this->is_exclusive() && replacement.is_overlapped_with(this_view.unsafe_subview(pos32_list.front(), pos32_list.back() + replacement.length() - pos32_list.front()))) {
				for (size_t pos : pos32_list) {
					if (replacement.is_overlapped_with(this_view.unsafe_subview(pos, replacement.length()))) {
						is_replacement_safe = false;
						break;
					}
				}
			}
		}
		else {
			//the len will change ...
			const auto this_whole_view = this->unsafe_whole_view();
			if (this->is_exclusive() && replacement.is_overlapped_with(this_whole_view.unsafe_subview(0, this_whole_view.length() + std::max(len_delta_total, ptrdiff_t(0))))) {
				if (len_delta_total > 0 && this->do_determine_need_grow(len_delta_total)) {
					//if strview is overlapped with this.wholeview, and this is exclusive, and need growing, it means that this.data will invalidate after growing, so we need clone strview
					is_replacement_safe = false;
				}
				else if (replacement.is_overlapped_with(this->view().unsafe_subview(pos32_list.front(), this->length() + std::max(len_delta_total, ptrdiff_t(0)) - pos32_list.front()))) {
					//if strview is overlapped with writing this.subview, and this is exclusive, and need growing, it means that this.data will invalidate after growing, so we need clone strview
					is_replacement_safe = false;
				}
			}
		}

		//多段替换执行过程
		auto do_substitute_imp = [this, &pos32_list, sub_length = sub.length(), len_delta_total](const ks_basic_string_view<ELEM>& replacement, bool ensure_end_ch0) -> void {
			if (len_delta_total > 0)
				this->do_auto_grow(len_delta_total);
			this->do_ensure_exclusive();

			ELEM* that_data = this->unsafe_data();
			ELEM* that_data_end = this->unsafe_data_end();

			if (len_delta_total == 0) {
				//length no change, no shift
				ASSERT(sub_length == replacement.length());
				for (size_t pos : pos32_list) {
					std::copy_n(replacement.data(), sub_length, that_data + pos);
				}
			}
			else if (len_delta_total < 0) {
				//shrink, shift data to left
				ELEM* write_p = that_data;
				ELEM* read_p = that_data;
				for (size_t pos : pos32_list) {
					ELEM* sub_p = that_data + pos;
					if (read_p != write_p)
						std::move(read_p, sub_p, write_p);
					write_p += (sub_p - read_p);
					std::copy_n(replacement.data(), replacement.length(), write_p);
					write_p += replacement.length();
					read_p = sub_p + sub_length;
				}
				if (read_p != write_p)
					std::move(read_p, that_data_end, write_p);
				write_p += (that_data_end - read_p);
				read_p = that_data_end;
				ASSERT(write_p == that_data_end - (-len_delta_total));
			}
			else {
				//enlarge, shift data to right
				ELEM* write_p_end = that_data_end + (+len_delta_total);
				ELEM* read_p_end = that_data_end;
				for (ptrdiff_t i = (ptrdiff_t)pos32_list.size() - 1; i >= 0; --i) {
					ELEM* sub_p_end = that_data + pos32_list[i] + sub_length;
					if (sub_p_end != write_p_end - (read_p_end - sub_p_end))
						std::move_backward(sub_p_end, read_p_end, write_p_end);
					write_p_end -= (read_p_end - sub_p_end);
					std::copy_n(replacement.data(), replacement.length(), write_p_end - replacement.length());
					write_p_end -= replacement.length();
					read_p_end = sub_p_end - sub_length;
				}
				if (that_data != write_p_end - (read_p_end - that_data))
					std::move_backward(that_data, read_p_end, write_p_end);
				write_p_end -= (read_p_end - that_data);
				read_p_end = that_data;
				ASSERT(write_p_end == that_data);
			}

			if (len_delta_total != 0) {
				if (this->is_sso_mode())
					this->_my_sso_ptr()->length8 += int8_t(len_delta_total);
				else
					this->_my_ref_ptr()->length32 += int32_t(len_delta_total);
			}

			this->do_ensure_end_ch0(ensure_end_ch0);
		};

		if (!is_replacement_safe) {
			std::basic_string<ELEM> replacement_dup(replacement.data(), replacement.length());
			return do_substitute_imp(__to_basic_string_view(replacement_dup), ensure_end_ch0);
		}
		else {
			return do_substitute_imp(replacement, ensure_end_ch0);
		}
	}

	void do_erase(size_t pos, size_t number, bool ensure_end_ch0) {
		if (ptrdiff_t(number) < 0)
			number = this->length() - pos;

		size_t pos_end = pos + number;
		if (pos > pos_end || pos_end > this->length())
			throw std::out_of_range("ks_basic_xmutable_string_base::erase(pos, number) out-of-range exception");
		if (number == 0)
			return;

		if (pos == 0 && number == this->length())
			return this->do_clear(ensure_end_ch0);

		if (pos_end < this->length()) {
			this->do_ensure_exclusive();
			std::move(this->data() + pos_end, this->data_end(), this->unsafe_data() + pos);
		}

		if (this->is_sso_mode())
			_my_sso_ptr()->length8 -= uint8_t(number);
		else
			_my_ref_ptr()->length32 -= uint32_t(number);

		this->do_ensure_end_ch0(ensure_end_ch0);
	}

	void do_clear(bool ensure_end_ch0) {
		if (this->is_sso_mode()) {
			auto* sso_ptr = _my_sso_ptr();
			sso_ptr->length8 = 0;
		}
		else {
			auto* ref_ptr = _my_ref_ptr();
			ref_ptr->length32 = 0;
		}

		this->do_ensure_end_ch0(ensure_end_ch0);
	}

	template <class RIGHT, class _ = std::enable_if_t<std::is_convertible_v<RIGHT, ks_basic_string_view<ELEM>>>>
	void do_self_add(RIGHT&& right, bool could_ref_right_data_directly, bool ensure_end_ch0) {
		const ks_basic_string_view<ELEM> right_view = __to_basic_string_view(right);
		bool will_ref_right_data_directly = false;
		if (could_ref_right_data_directly && !right_view.empty() && this->empty()) {
			if (std::is_base_of_v<ks_basic_xmutable_string_base<ELEM>, std::remove_cv_t<std::remove_reference_t<RIGHT>>> &&
				static_cast<const ks_basic_xmutable_string_base<ELEM>&>(right).is_ref_mode())
				will_ref_right_data_directly = true;
			else if (std::is_same_v<RIGHT, std::basic_string<ELEM, std::char_traits<ELEM>, ks_basic_string_allocator<ELEM>>&&>)
				will_ref_right_data_directly = true;
		}

		if (will_ref_right_data_directly) 
			*this = ks_basic_xmutable_string_base(std::forward<RIGHT>(right));
		else 
			this->do_append(right_view, false);

		this->do_ensure_end_ch0(ensure_end_ch0);

		//ensure right detached
		if (std::is_base_of_v<ks_basic_xmutable_string_base<ELEM>, std::remove_cv_t<std::remove_reference_t<RIGHT>>> &&
			std::is_rvalue_reference_v<RIGHT&&> && !std::is_const_v<std::remove_reference_t<RIGHT>>) {
			ks_basic_xmutable_string_base<ELEM>(std::forward<RIGHT>(right)).do_detach_void();
		}
	}

public:
	int compare(const ELEM* p) const { return this->view().compare(p); }
	int compare(const ELEM* p, size_t count) const { return this->view().compare(p, count); }
	int compare(const ks_basic_string_view<ELEM>& str_view) const { return this->view().compare(str_view); }

	bool contains(const ELEM* p) const { return this->view().contains(p); }
	bool contains(const ELEM* p, size_t count) const { return this->view().contains(p, count); }
	bool contains(const ks_basic_string_view<ELEM>& str_view) const { return this->view().contains(str_view); }
	bool contains(ELEM ch) const { return this->view().contains(ch); }

	bool starts_with(const ELEM* p) const { return this->view().starts_with(p); }
	bool starts_with(const ELEM* p, size_t count) const { return this->view().starts_with(p, count); }
	bool starts_with(const ks_basic_string_view<ELEM>& str_view) const { return this->view().starts_with(str_view); }
	bool starts_with(ELEM ch) const { return this->view().starts_with(ch); }

	bool ends_with(const ELEM* p) const { return this->view().ends_with(p); }
	bool ends_with(const ELEM* p, size_t count) const { return this->view().ends_with(p, count); }
	bool ends_with(const ks_basic_string_view<ELEM>& str_view) const { return this->view().ends_with(str_view); }
	bool ends_with(ELEM ch) const { return this->view().ends_with(ch); }

	size_t find(const ELEM* p, size_t pos = 0) const { return this->view().find(p, pos); }
	size_t find(const ELEM* p, size_t pos, size_t count) const { return this->view().find(p, pos, count); }
	size_t find(const ks_basic_string_view<ELEM>& str_view, size_t pos = 0) const { return this->view().find(str_view, pos); }
	size_t find(ELEM ch, size_t pos = 0) const { return this->view().find(ch, pos); }

	size_t rfind(const ELEM* p, size_t pos = -1) const { return this->view().rfind(p, pos); }
	size_t rfind(const ELEM* p, size_t pos, size_t count) const { return this->view().rfind(p, pos, count); }
	size_t rfind(const ks_basic_string_view<ELEM>& str_view, size_t pos = -1) const { return this->view().rfind(str_view, pos); }
	size_t rfind(ELEM ch, size_t pos = -1) const { return this->view().rfind(ch, pos); }

	size_t find_first_of(const ELEM* p, size_t pos = 0) const { return this->view().find_first_of(p, pos); }
	size_t find_first_of(const ELEM* p, size_t pos, size_t count) const { return this->view().find_first_of(p, pos, count); }
	size_t find_first_of(const ks_basic_string_view<ELEM>& str_view, size_t pos = 0) const { return this->view().find_first_of(str_view, pos); }
	size_t find_first_of(ELEM ch, size_t pos = 0) const { return this->view().find_first_of(ch, pos); }

	size_t find_last_of(const ELEM* p, size_t pos = -1) const { return this->view().find_last_of(p, pos); }
	size_t find_last_of(const ELEM* p, size_t pos, size_t count) const { return this->view().find_last_of(p, pos, count); }
	size_t find_last_of(const ks_basic_string_view<ELEM>& str_view, size_t pos = -1) const { return this->view().find_last_of(str_view, pos); }
	size_t find_last_of(ELEM ch, size_t pos = -1) const { return this->view().find_last_of(ch, pos); }

	size_t find_first_not_of(const ELEM* p, size_t pos = 0) const { return this->view().find_first_of(p, pos); }
	size_t find_first_not_of(const ELEM* p, size_t pos, size_t count) const { return this->view().find_first_of(p, pos, count); }
	size_t find_first_not_of(const ks_basic_string_view<ELEM>& str_view, size_t pos = 0) const { return this->view().find_first_of(str_view, pos); }
	size_t find_first_not_of(ELEM ch, size_t pos = 0) const { return this->view().find_first_of(ch, pos); }

	size_t find_last_not_of(const ELEM* p, size_t pos = -1) const { return this->view().find_last_of(p, pos); }
	size_t find_last_not_of(const ELEM* p, size_t pos, size_t count) const { return this->view().find_last_of(p, pos, count); }
	size_t find_last_not_of(const ks_basic_string_view<ELEM>& str_view, size_t pos = -1) const { return this->view().find_last_of(str_view, pos); }
	size_t find_last_not_of(ELEM ch, size_t pos = -1) const { return this->view().find_last_of(ch, pos); }

protected:
	ks_basic_xmutable_string_base do_slice(size_t from, size_t to) const {
		const size_t this_length = this->length();
		if (from > this_length)
			throw std::out_of_range("ks_basic_xmutable_string_base::slice(from, to) out-of-range exception");
		if (to > this_length)
			to = this_length;
		else if (to < from)
			to = from;
		return this->unsafe_substr(from, to - from);
	}

	ks_basic_xmutable_string_base do_substr(size_t pos, size_t count) const {
		const size_t this_length = this->length();
		if (pos > this_length)
			throw std::out_of_range("ks_basic_xmutable_string_base::substr(pos, count) out-of-range exception");
		if (count > this_length - pos)
			count = this_length - pos;
		return this->unsafe_substr(pos, count);
	}

protected:
	ks_basic_xmutable_string_base unsafe_substr(size_t pos, size_t count) const {
		ASSERT(this->view().unsafe_subview(pos, count + 1).is_subview_of(this->unsafe_whole_view()));
		if (count <= _SSO_BUFFER_SPACE - 1) {
			return ks_basic_xmutable_string_base(this->view().data() + (ptrdiff_t)pos, count);
		}
		else {
			ks_basic_xmutable_string_base slice = *this;
			ASSERT(slice.is_ref_mode());
			auto* slice_ref_ptr = slice._my_ref_ptr();
			slice_ref_ptr->p += (ptrdiff_t)pos;
			slice_ref_ptr->offset32 += (int32_t)(ptrdiff_t)pos;
			slice_ref_ptr->length32 = (uint32_t)count;
			return slice;
		}
	}

	ks_basic_string_view<ELEM> unsafe_whole_view() const {
		if (this->is_sso_mode()) {
			auto* sso_ptr = _my_sso_ptr();
			return ks_basic_string_view<ELEM>(sso_ptr->buffer, _SSO_BUFFER_SPACE);
		}
		else {
			auto* ref_ptr = _my_ref_ptr();
			ELEM* alloc_addr = ref_ptr->alloc_addr();
			return ks_basic_string_view<ELEM>(alloc_addr, ks_basic_string_allocator<ELEM>::_get_space32_value(alloc_addr));
		}
	}

protected:
	template <class STR_TYPE, class _ = std::enable_if_t<std::is_base_of_v<ks_basic_xmutable_string_base<ELEM>, STR_TYPE>>>
	std::vector<STR_TYPE> do_split(const ks_basic_string_view<ELEM>& sep, size_t n) const {
		const auto& this_view = this->view();
		std::vector<ks_basic_string_view<ELEM>> sub_view_seq = this_view.split(sep, n);

		std::vector<STR_TYPE> ret;
		ret.reserve(sub_view_seq.size());
		for (auto& sub_view : sub_view_seq) {
			ret.push_back(this->unsafe_substr(sub_view.data() - this_view.data(), sub_view.length()));
		}

		return ret;
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
			throw std::out_of_range("ks_basic_xmutable_string_base::at(pos) out-of-range exception");
		else
			return this->data()[pos];
	}

	const ELEM& operator[](size_t pos) const {
		ASSERT(pos < this->length());
		return this->data()[pos];
	}

public:
	const ELEM* data() const {
		return this->is_sso_mode()
			? _my_sso_ptr()->buffer
			: _my_ref_ptr()->p;
	}

	const ELEM* data_end() const {
		return this->is_sso_mode()
			? _my_sso_ptr()->buffer + _my_sso_ptr()->length8
			: _my_ref_ptr()->p + _my_ref_ptr()->length32;
	}

	size_t length() const {
		return this->is_sso_mode()
			? _my_sso_ptr()->length8
			: _my_ref_ptr()->length32;
	}

	size_t size() const { return this->length(); }
	bool empty() const { return this->length() == 0; }

	size_t capacity() const {
		return this->is_sso_mode()
			? _SSO_BUFFER_SPACE - 1
			: (ks_basic_string_allocator<ELEM>::_get_space32_value(_my_ref_ptr()->alloc_addr()) - 1) - _my_ref_ptr()->offset32;
	}

	bool is_exclusive() const {
		return !(this->is_ref_mode() && ks_basic_string_allocator<ELEM>::_get_refcount32_value(_my_ref_ptr()->alloc_addr()) > 1);
	}

	ks_basic_string_view<ELEM> view() const {
		return ks_basic_string_view<ELEM>(this->data(), this->length());
	}

protected:
	bool is_sso_mode() const { return _my_mode() == _SSO_MODE; }
	bool is_ref_mode() const { return _my_mode() == _REF_MODE; }
	bool is_detached_empty() const { return this->is_sso_mode() && this->empty(); }

	ELEM* unsafe_data() const { return (ELEM*)this->data(); }
	ELEM* unsafe_data_end() const { return (ELEM*)this->data_end(); }

private:
	static constexpr uint8_t _SSO_MODE = 0;
	static constexpr uint8_t _REF_MODE = 1;

	static constexpr size_t _MODE_BITS = 1;
	static constexpr size_t _FIX_DATA_SIZE = std::max(size_t(sizeof(ELEM) <= 2 ? 16 : 24), (sizeof(ELEM) * 2) / 8 * 8); //use 32 is good also, and std::basic_string uses just 32，but we use smaller size for mem-compact
	static constexpr size_t _SSO_BUFFER_SPACE = ((_FIX_DATA_SIZE - 2) / sizeof(ELEM)); //why sub 2? see also _SSO_STRUCT
	static constexpr size_t _STR_LENGTH_LIMIT = 0x7FFFFFFF; //due to _REF_STRUCT::offset32 is 31 bits actually, so the max len is defined as here
	static_assert(_SSO_BUFFER_SPACE != 0, "sso-buffer-space must not be 0");

	struct _SSO_STRUCT {
		uint8_t mode : _MODE_BITS;
		uint8_t length8; //due to gap exists, length8 need not share same byte with mode, for optimization
		ELEM buffer[_SSO_BUFFER_SPACE];
	};
	struct _REF_STRUCT {
		uint32_t mode : _MODE_BITS;
		uint32_t offset32 : (32 - _MODE_BITS);
		uint32_t length32;
		ELEM* p;
		ELEM* alloc_addr() const { return this->p - (size_t)(this->offset32); }
	};

	union _DATA_UNION {
		uint8_t mode : _MODE_BITS;
		_SSO_STRUCT sso_struct;
		_REF_STRUCT ref_struct;
	};

	static_assert(sizeof(_SSO_STRUCT) <= _FIX_DATA_SIZE, "the size of SSO_STRUCT is not perfect");
	static_assert(sizeof(_REF_STRUCT) <= _FIX_DATA_SIZE, "the size of REF_STRUCT it not perfect");
	static_assert(sizeof(_DATA_UNION) <= _FIX_DATA_SIZE, "the size of DATA_UNION is not perfect");

	_DATA_UNION m_data_union;

private:
	uint8_t _my_mode() const { return m_data_union.mode; }
	_SSO_STRUCT* _my_sso_ptr() { return &m_data_union.sso_struct; }
	_REF_STRUCT* _my_ref_ptr() { return &m_data_union.ref_struct; }
	const _SSO_STRUCT* _my_sso_ptr() const { return &m_data_union.sso_struct; }
	const _REF_STRUCT* _my_ref_ptr() const { return &m_data_union.ref_struct; }

public:
	bool operator==(const ks_basic_string_view<ELEM>& right) const { return this->view() == right; }
	bool operator!=(const ks_basic_string_view<ELEM>& right) const { return this->view() != right; }
	bool operator<(const ks_basic_string_view<ELEM>& right) const { return this->view() < right; }
	bool operator<=(const ks_basic_string_view<ELEM>& right) const { return this->view() <= right; }
	bool operator>(const ks_basic_string_view<ELEM>& right) const { return this->view() > right; }
	bool operator>=(const ks_basic_string_view<ELEM>& right) const { return this->view() >= right; }

protected: 
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ELEM* p) { return ks_basic_string_view<ELEM>::__to_basic_string_view(p); }
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ELEM* p, size_t count) { return ks_basic_string_view<ELEM>::__to_basic_string_view(p, count); }

	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ks_basic_string_view<ELEM>& str_view) { return ks_basic_string_view<ELEM>::__to_basic_string_view(str_view); }
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ks_basic_string_view<ELEM>& str_view, size_t offset, size_t count) { return ks_basic_string_view<ELEM>::__to_basic_string_view(str_view, offset, count); }

	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ks_basic_xmutable_string_base<ELEM>& str) { return ks_basic_string_view<ELEM>::__to_basic_string_view(str); }
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const ks_basic_xmutable_string_base<ELEM>& str, size_t offset, size_t count) { return ks_basic_string_view<ELEM>::__to_basic_string_view(str, offset, count); }

	template <class AllocType>
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str) { return ks_basic_string_view<ELEM>::__to_basic_string_view(str); }
	template <class AllocType>
	static constexpr inline ks_basic_string_view<ELEM> __to_basic_string_view(const std::basic_string<ELEM, std::char_traits<ELEM>, AllocType>& str, size_t offset, size_t count) { return ks_basic_string_view<ELEM>::__to_basic_string_view(str, offset, count); }

	friend class ks_basic_mutable_string<ELEM>;
	friend class ks_basic_immutable_string<ELEM>;
};


template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator==(const LEFT& left, const ks_basic_xmutable_string_base<ELEM>& right) { return ks_basic_string_view<ELEM>(left) == right.view(); }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator!=(const LEFT& left, const ks_basic_xmutable_string_base<ELEM>& right) { return ks_basic_string_view<ELEM>(left) != right.view(); }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator<(const LEFT& left, const ks_basic_xmutable_string_base<ELEM>& right) { return ks_basic_string_view<ELEM>(left) < right.view(); }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator<=(const LEFT& left, const ks_basic_xmutable_string_base<ELEM>& right) { return ks_basic_string_view<ELEM>(left) <= right.view(); }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator>(const LEFT& left, const ks_basic_xmutable_string_base<ELEM>& right) { return ks_basic_string_view<ELEM>(left) > right.view(); }
template <class LEFT, class ELEM, class _ = std::enable_if_t<std::is_convertible_v<LEFT, ks_basic_string_view<ELEM>>>>
inline bool operator>=(const LEFT& left, const ks_basic_xmutable_string_base<ELEM>& right) { return ks_basic_string_view<ELEM>(left) >= right.view(); }


namespace std {
	template <class ELEM>
	struct hash<ks_basic_xmutable_string_base<ELEM>> : hash<ks_basic_string_view<ELEM>> {
	};
}


template <class ELEM>
std::basic_ostream<ELEM, std::char_traits<ELEM>>& operator<<(std::basic_ostream<ELEM, std::char_traits<ELEM>>& strm, const ks_basic_xmutable_string_base<ELEM>& str) {
	return strm << str.view();
}
