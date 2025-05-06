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
#include <type_traits>


#ifndef __STD_TYPE_TRAITS_FIX14
#define __STD_TYPE_TRAITS_FIX14

#if __cplusplus < 201703L && !defined(_MSVC_LANG)
namespace std {
	template<bool __v>
	using bool_constant = integral_constant<bool, __v>;

	template <typename _Tp>
	constexpr bool is_void_v = is_void<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_null_pointer_v = is_null_pointer<_Tp>::value;

	template <typename _Tp, typename _Up>
	constexpr bool is_same_v = is_same<_Tp, _Up>::value;

	template <typename _From, typename _To>
	constexpr bool is_convertible_v = is_convertible<_From, _To>::value;

	template <typename _Tp>
	constexpr bool is_copy_assignable_v = is_copy_assignable<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_trivially_destructible_v = is_trivially_destructible<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_lvalue_reference_v = false;
	template <typename _Tp>
	constexpr bool is_lvalue_reference_v<_Tp&> = true;

	template <typename _Tp>
	constexpr bool is_rvalue_reference_v = is_rvalue_reference<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_trivial_v = is_trivial<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_standard_layout_v = is_standard_layout<_Tp>::value;

	template <typename _Base, typename _Derived>
	constexpr bool is_base_of_v = is_base_of<_Base, _Derived>::value;

	template <typename _Tp>
	constexpr bool is_const_v = is_const<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_signed_v = is_signed<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_integral_v = is_integral<_Tp>::value;

	template <typename _Tp>
	constexpr bool is_floating_point_v = is_floating_point<_Tp>::value;

	template <typename...>
	struct conjunction : true_type {};
	template <typename _Bn>
	struct conjunction<_Bn> : _Bn {};
	template <typename _Arg, class... _Args>
	struct conjunction<_Arg, _Args...> : conditional_t<!bool(_Arg::value), _Arg, conjunction<_Args...>> {};
	template <typename... _Bn>
	constexpr bool conjunction_v = conjunction<_Bn...>::value;

	template <typename...>
	struct disjunction : false_type {};
	template <typename _Bn>
	struct disjunction<_Bn> : _Bn {};
	template <typename _Arg, class... _Args>
	struct disjunction<_Arg, _Args...> : conditional_t<bool(_Arg::value), _Arg, disjunction<_Args...>> {};
	template <typename... _Bn>
	constexpr bool disjunction_v = disjunction<_Bn...>::value;
}
#endif

#if __cplusplus < 202002L
namespace std {
	//remove_cvref (like c++17)
	template <class T>
	struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> {};
	template <class T>
	using remove_cvref_t = typename remove_cvref<T>::type;
}
#endif

#endif //__STD_TYPE_TRAITS_FIX14


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifndef __KS_CHAR_TRAITS_DEF
#define __KS_CHAR_TRAITS_DEF
#include <string>

template <
	class ELEM, class UNDERLYING_ELEM, 
	class INT_TYPE = typename std::char_traits<UNDERLYING_ELEM>::int_type, 
	class STATE_TYPE = typename std::char_traits<UNDERLYING_ELEM>::state_type>
class __ks_underlying_char_traits {
	using __underlying_char_type = UNDERLYING_ELEM;
	using __underlying_std_char_traits = std::char_traits<UNDERLYING_ELEM>;
    static_assert(sizeof(UNDERLYING_ELEM) == sizeof(ELEM), "the size of ELEM and UNDERLYING_ELEM should be equal");

public:
    using char_type = ELEM;
	using int_type = INT_TYPE;
	using pos_type = typename __underlying_std_char_traits::pos_type;
    using off_type = typename __underlying_std_char_traits::off_type;
    using state_type = STATE_TYPE;
#if __cplusplus >= 202002L
	using comparison_category = typename __underlying_std_char_traits::comparison_category;
#endif

public:
	static constexpr bool eq(char_type _Left, char_type _Right) noexcept {
        return _Left == _Right;
    }
	static constexpr bool eq_int_type(int_type _Left, int_type _Right) noexcept {
		return _Left == _Right;
	}
	static constexpr bool lt(char_type _Left, char_type _Right) noexcept {
		return _Left < _Right;
	}

	static constexpr char_type to_char_type(int_type _Meta) noexcept {
        return (char_type)_Meta;
    }
    static constexpr int_type to_int_type(char_type _Ch) noexcept {
        return (int_type)_Ch;
    }

	static constexpr int_type not_eof(int_type _Meta) noexcept {
        return __underlying_std_char_traits::not_eof(_Meta);
    }
    static constexpr int_type eof() noexcept {
        return __underlying_std_char_traits::eof();
    }

	static constexpr size_t length(const char_type* _First) noexcept {
#if __cplusplus < 201703L
		const char_type* t = _First;
		if (t != nullptr) {
			while (*t)
				++t;
			return t - _First;
		}
		else {
			return 0;
		}
#else
		return __underlying_std_char_traits::length((const __underlying_char_type*)_First);
#endif
	}

	static int compare(const char_type* _First1, const char_type* _First2, size_t _Count) {
		return __underlying_std_char_traits::compare((const __underlying_char_type*)_First1, (const __underlying_char_type*)_First2, _Count);
	}
	static const char_type* find(const char_type* _First, size_t _Count, char_type _Ch) {
		return (const char_type*)__underlying_std_char_traits::find((const __underlying_char_type*)_First, _Count, _Ch);
	}

	static char_type* copy(char_type* dest, const char_type* src, std::size_t count) {
		return (char_type*)__underlying_std_char_traits::copy((__underlying_char_type*)dest, (const __underlying_char_type*)src, count);
	}
	static char_type* move(char_type* dest, const char_type* src, std::size_t count) {
		return (char_type*)__underlying_std_char_traits::move((__underlying_char_type*)dest, (const __underlying_char_type*)src, count);
	}

	static char_type* assign(char_type* _First, size_t _Count, char_type _Ch) {
		return (char_type*)__underlying_std_char_traits::assign((__underlying_char_type*)_First, _Count, _Ch);
	}
	static void assign(char_type& _Left, const char_type& _Right) {
		_Left = _Right;
	}
};

template <class ELEM> class ks_char_traits : public __ks_underlying_char_traits<ELEM, ELEM> {};

#if __cplusplus < 202002L
template <> class ks_char_traits<uint8_t> : public __ks_underlying_char_traits<uint8_t, unsigned char, uint8_t, std::mbstate_t> {};
#else
template <> class ks_char_traits<uint8_t> : public __ks_underlying_char_traits<uint8_t, char8_t> {};
#endif
template <> class ks_char_traits<uint16_t> : public __ks_underlying_char_traits<uint16_t, char16_t> {};
template <> class ks_char_traits<uint32_t> : public __ks_underlying_char_traits<uint32_t, char32_t> {};

#endif //__KS_CHAR_TRAITS_DEF
