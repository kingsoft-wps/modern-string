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

#include <type_traits>


#ifndef __STD_TYPE_TRAITS_FIX14
#define __STD_TYPE_TRAITS_FIX14

#if defined(_MSVC_LANG) ? false : __cplusplus < 201703L
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

namespace std {
	//remove_cvref (like c++17)
	template <class T>
	struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> {};
	template <class T>
	using remove_cvref_t = typename remove_cvref<T>::type;
}

#endif //__STD_TYPE_TRAITS_FIX14


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

