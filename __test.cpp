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

#include "ks_string.h"
#include "ks_string_util.h"
#include <iostream>


int main() {
#ifdef _WIN32
    std::cout.imbue(std::locale("zh_CN"));
    std::wcout.imbue(std::locale("zh_CN"));
#endif

    ks_mutable_string ms1("a");
    ks_mutable_string ms2("bbbbbabbbbbabbbbbabbbbbabbbbb");
    ks_immutable_string ims1("x");
    ks_immutable_string ims2("yyyyyxyyyyyxyyyyyxyyyyyxyyyyy");
    ks_mutable_string ms3(ims1);
    ks_mutable_string ms4(ims2);
    ks_immutable_string ims3(ms1);
    ks_immutable_string ims4(ms2);
    
    std::cout << "ms1: " << ms1 << "\n";
    std::cout << "ms2: " << ms2 << "\n";
    std::cout << "ms3: " << ms3 << "\n";
    std::cout << "ms4: " << ms4 << "\n";
    std::cout << "ims1: " << ims1 << "\n";
    std::cout << "ims2: " << ims2 << "\n";
    std::cout << "ims3: " << ims3 << "\n";
    std::cout << "ims4: " << ims4 << "\n";

    auto ms5 = ms1 + ms2;
    auto ms6 = ms2 + ms2;
    auto ims5 = ims1 + ims2;
    auto ims6 = ims2 + ims2;
    auto ms7 = ms1 + ims2;
    auto ms8 = ms2 + ims2;
    auto ims7 = ims1 + ms2;
    auto ims8 = ims2 + ms2;
    std::cout << "ms5: " << ms5 << "\n";
    std::cout << "ms6: " << ms6 << "\n";
    std::cout << "ms7: " << ms7 << "\n";
    std::cout << "ms8: " << ms8 << "\n";
    std::cout << "ims5: " << ims5 << "\n";
    std::cout << "ims6: " << ims6 << "\n";
    std::cout << "ims7: " << ims7 << "\n";
    std::cout << "ims8: " << ims8 << "\n";

    size_t pos1 = ms2.find(ms1);
    size_t pos2 = ms2.find(ims1);
    size_t pos3 = ims2.find(ims1);
    size_t pos4 = ims2.find(ms1);
    size_t pos5 = ms2.find_last_of(ms1);
    size_t pos6 = ms2.rfind(ims1);
    size_t pos7 = ims2.rfind(ims1);
    size_t pos8 = ims2.rfind(ms1);
    std::cout << "pos1: " << pos1 << "\n";
    std::cout << "pos2: " << pos2 << "\n";
    std::cout << "pos3: " << pos3 << "\n";
    std::cout << "pos4: " << pos4 << "\n";
    std::cout << "pos5: " << pos5 << "\n";
    std::cout << "pos6: " << pos6 << "\n";
    std::cout << "pos7: " << pos7 << "\n";
    std::cout << "pos8: " << pos8 << "\n";

    ms1.substitute('a', 'x');
    std::cout << "ms1.substitute(a=>x): " << ms1 << "\n";
    ms2.substitute_n('b', 'c', 2);
    std::cout << "ms2.substitute(b=>c, 2): " << ms2 << "\n";
    ms2.substitute("b", "c");
    std::cout << "ms2.substitute(b=>c): " << ms2 << "\n";
    ms2.substitute("c", "dd");
    std::cout << "ms2.substitute(c=>dd): " << ms2 << "\n";
    ms2.substitute("dd", "e");
    std::cout << "ms2.substitute(dd=>e): " << ms2 << "\n";

    ms2.assign(ims4);
    std::cout << "ms2.assign(ims4): " << ms2 << "\n";
    ms2.assign(ms4);
    std::cout << "ms2.assign(ms4): " << ms2 << "\n";

    ks_mutable_string ms9 = ms2.slice(1, 4);
    ks_immutable_string ims9 = ims2.slice(100, 400);
    std::cout << "ms9: " << ms9 << "\n";
    std::cout << "ims9: " << ims9 << "\n";

    ms9.reserve(60);
    std::cout << "ms9.reserve(60): " << ms9 << "\n";
    ms9.resize(2);
    std::cout << "ms9.resize(2): " << ms9 << "\n";

    size_t h1 = std::hash<ks_mutable_string>{}(ms1);
    size_t h2 = std::hash<ks_immutable_string>{}(ims1);
    std::cout << "h1: " << h1 << "\n";
    std::cout << "h2: " << h2 << "\n";

    std::vector<ks_immutable_string> ims2_subs = ims2.split("x");
    std::cout << "ims2.split(x): [ ";
    for (auto& sub : ims2_subs) {
        if (&sub != &ims2_subs[0])
            std::cout << ", ";
        std::cout << sub;
    }
    std::cout << " ]\n";

    ks_immutable_string ims10 = ks_string_util::join(ims2_subs.begin(), ims2_subs.end(), "z", "\"", "\"");
    ks_immutable_string ims10a = ks_string_util::join(ims2_subs.begin(), ims2_subs.end(), "");
    std::cout << "ims10(join): " << ims10 << "\n";
    std::cout << "ims10a(concat): " << ims10a << "\n";

    std::cout << "parse-int 100: " << ks_string_util::parse_int("100") << "\n";
    std::cout << "parse-double 100.2: " << ks_string_util::parse_double("100.2") << "\n";
    std::cout << "parse-bool true: " << ks_string_util::parse_bool("true") << "\n";

    std::cout << "to-string 200: " << ks_string_util::to_string(200) << "\n";
    std::cout << "to-string true: " << ks_string_util::to_string(true) << "\n";

#ifdef _WIN32
    std::wcout << "convert utf8: " << ks_string_util::wstring_from_u8_chars(ks_string_util::wstring_to_std_u8_string((WCHAR*)u"大家好呀呀").c_str(), -1) << "\n";
    std::wcout << "convert utf32: " << ks_string_util::wstring_from_u32_chars(ks_string_util::wstring_to_std_u32_string((WCHAR*)u"大家好呀呀").c_str(), -1) << "\n";
    std::wcout << "convert ansi: " << ks_string_util::wstring_from_native_chars(ks_string_util::wstring_to_std_native_string((WCHAR*)u"大家好呀呀").c_str(), -1) << "\n";
    std::wcout << "convert wide: " << ks_string_util::wstring_from_native_wide_chars(ks_string_util::wstring_to_std_native_wide_string((WCHAR*)u"大家好呀呀").c_str(), -1) << "\n";
#else
    std::cout << "convert utf8: " << ks_string_util::wstring_to_std_native_string(ks_string_util::wstring_from_u8_chars(ks_string_util::wstring_to_std_u8_string((WCHAR*)u"大家好呀呀").c_str(), -1)) << "\n";
    std::cout << "convert utf32: " << ks_string_util::wstring_to_std_native_string(ks_string_util::wstring_from_u32_chars(ks_string_util::wstring_to_std_u32_string((WCHAR*)u"大家好呀呀").c_str(), -1)) << "\n";
    std::cout << "convert ansi: " << ks_string_util::wstring_to_std_native_string(ks_string_util::wstring_from_native_chars(ks_string_util::wstring_to_std_string((WCHAR*)u"大家好呀呀").c_str(), -1)) << "\n";
    std::cout << "convert wide: " << ks_string_util::wstring_to_std_native_string(ks_string_util::wstring_from_native_wide_chars(ks_string_util::wstring_to_std_native_wide_string((WCHAR*)u"大家好呀呀").c_str(), -1)) << "\n";
#endif

    //ks_mutable_string ms10;
    //std::cout << "please input ms10: ";
    //std::cin >> ms10;
    //std::cout << "ms10: " << ms10 << "\n";

    std::cout << "Hello World!\n";
    return 0;
}
