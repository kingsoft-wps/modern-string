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
#include "ks_string_view.h"

#include "ks_basic_mutable_string.inl"
#include "ks_basic_immutable_string.inl"

using ks_mutable_string = ks_basic_mutable_string<char>;
using ks_immutable_string = ks_basic_immutable_string<char>;

using ks_mutable_wstring = ks_basic_mutable_string<WCHAR>;
using ks_immutable_wstring = ks_basic_immutable_string<WCHAR>;


#include "ks_string_util.h"
