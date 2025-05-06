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

#include <memory>
#include <atomic>


template <class ELEM>
class MODERN_STRING_INLINE_API ks_basic_string_allocator {
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = ELEM;
    using reference = ELEM&;
    using const_reference = const ELEM&;
    using pointer = ELEM*;
    using const_pointer = const ELEM*;

    constexpr ks_basic_string_allocator() {}
    constexpr ks_basic_string_allocator(const ks_basic_string_allocator&) {}

    template <class ELEM2> 
    constexpr ks_basic_string_allocator(const ks_basic_string_allocator<ELEM2>&) {}

    template <class ELEM2>
    struct rebind { using other = ks_basic_string_allocator<ELEM2>; };

    static ELEM* address(ELEM& _Val) { return std::addressof(_Val); }
    static const ELEM* address(const ELEM& _Val) { return std::addressof(_Val); }

    static ELEM* allocate(size_t _Count) {
        if (_Count > 0x7FFFFFFFu)
            throw std::bad_array_new_length();
        _Count = ((_Count * sizeof(ELEM) + 3) & ~size_t(0x03)) / sizeof(ELEM);
        size_t alloc_size = __header_size() + ((_Count * sizeof(ELEM) + 3) & ~size_t(0x03));
        ASSERT(alloc_size % 4 == 0);
        uintptr_t addr = (uintptr_t)malloc(alloc_size);
        if (addr == 0)
            throw std::bad_alloc();
        ASSERT(addr % 4 == 0);
        addr += __header_size();
        *(uint32_t*)__get_space32_p((ELEM*)(addr)) = uint32_t(_Count);
        *(uint32_t*)__get_refcount32_p((ELEM*)(addr)) = 0;
        return (ELEM*)(addr);
    }

    static ELEM* allocate(size_t _Count, const void*) {
        return allocate(_Count);
    }

    static void deallocate(ELEM* _Ptr) {
        ASSERT(_Ptr != nullptr);
        ASSERT(*(uint32_t*)__get_refcount32_p(_Ptr) == 0);
        free((void*)(uintptr_t(_Ptr) - __header_size()));
    }

    static void deallocate(ELEM* _Ptr, size_t _Count) {
        ASSERT(_Ptr != nullptr);
        ASSERT(_get_space32_value(_Ptr) == ((_Count * sizeof(ELEM) + 3) & ~size_t(0x03)) / sizeof(ELEM));
        deallocate(_Ptr);
    }

    template <class _Objty, class... _Types> 
    static void construct(_Objty* _Ptr, _Types&&... _Args) {
        ::new (const_cast<void*>(static_cast<const volatile void*>(_Ptr))) _Objty(std::forward<_Types>(_Args)...); 
    }

    template <class _Uty> 
    static void destroy(_Uty* _Ptr) {
        _Ptr->~_Uty(); 
    }

    static constexpr size_t max_size() {
        return (size_t(-1) - __header_size()) / sizeof(ELEM);
    }

    template <class _Uty> 
    bool operator ==(const ks_basic_string_allocator<_Uty>&) const { return true; }
    template <class _Uty> 
    bool operator !=(const ks_basic_string_allocator<_Uty>&) const { return false; }

public:
    static ELEM* _atomic_alloc(size_t _Count) {
        ELEM* _Ptr = allocate(_Count);
        _atomic_initref(_Ptr);
        return _Ptr;
    }

    static void _atomic_initref(ELEM* _Ptr) {
        ASSERT(_Ptr != nullptr);
        ASSERT(_get_refcount32_value(_Ptr) == 0);
        *(volatile uint32_t*)__get_refcount32_p((ELEM*)(_Ptr)) = 1;
    }

    static void _atomic_addref(ELEM* _Ptr) {
        ASSERT(_Ptr != nullptr);
        ASSERT(_get_refcount32_value(_Ptr) >= 1);
        ++(*(std::atomic<uint32_t>*)__get_refcount32_p(_Ptr));
    }

    static void _atomic_release(ELEM* _Ptr) {
        ASSERT(_Ptr != nullptr);
        ASSERT(_get_refcount32_value(_Ptr) >= 1);
        if (--(*(std::atomic<uint32_t>*)__get_refcount32_p(_Ptr)) == 0) {
            deallocate(_Ptr);
        }
    }

    static constexpr uint32_t _get_space32_value(ELEM* p) {
        return *(uint32_t*)__get_space32_p(p);
    }

    static constexpr uint32_t _get_refcount32_value(ELEM* p) {
        return *(volatile uint32_t*)__get_refcount32_p(p);
    }

private:
    static constexpr size_t __header_size() {
        static_assert(alignof(ELEM) < 8 ? true : alignof(ELEM) % 4 == 0, "the asign of larger ELEM type must be multi of 4");
        return alignof(ELEM) < 8 ? 8 : alignof(ELEM);
    }

    static constexpr void* __get_space32_p(ELEM* p) {
        ASSERT(p != nullptr);
        ASSERT(uintptr_t(p) % 4 == 0);
        return (void*)(uint32_t*)(uintptr_t(p) - 4);
    }

    static constexpr void* __get_refcount32_p(ELEM* p) {
        ASSERT(p != nullptr);
        ASSERT(uintptr_t(p) % 4 == 0);
        return (void*)(uint32_t*)(uintptr_t(p) - 8);
    }
};
