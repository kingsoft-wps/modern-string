# modern-string

modern-string是一个高效C++字符串实现，其核心设计意图是支持低成本切片（slice）。同时为易于项目改造，最大程度保持与std::basic_string的接口兼容。

modern-string的核心设计思想是：
  1. 多个实例间共享内部字符串内存。
  2. 内部字符串数据永不改变。
  3. 字符串切片即为内部字符串的片段引用。
  4. 使用SSO和COW优化策略。

modern-string是基于C++14标准，但退化为C++11标准亦有可能。

modern-string提供的核心字符串类是ks_basic_mutable_string和ks_basic_immutable_string。

此外因为基于C++14的原因，还提供了ks_basic_string_view，以替代C++17才提供的std::basic_string_view。

按惯例，我们还定义了以下常用类型：
  1. ks_mutable_string
  2. ks_mutable_wstring
  3. ks_immutable_string
  4. ks_immutable_wstring
  5. ks_string_view
  6. ks_wstring_view

要特别说明的是，此库中的wstring实为utf-16编码的字符串，即使在Linux下也是如此。这与std::wstring是完全不同的，使用时务必注意区分。


## ks_basic_mutable_string 介绍

ks_basic_mutable_string与std::basic_string十分相似，关键区别在于：
  1. operator\[]和at方法的返回值类型恒为const_reference。
  2. iterator与const_iterator等价。
  3. 增加set_at方法。
  4. 增加slice方法。
  5. 增加trim、split等方法。
  6. 诸如substr、slice等方法返回值类型为immutable的。


## ks_basic_immutable_string 介绍

ks_basic_immutable_string是一个更纯粹的不可变字符串，可以类比为golang中的字符串。

其与ks_basic_mutable_string的关键区别在于：
  1. 不提供任何字符串修改方法。
  2. 不提供c_str方法。（这一点暗示了immutable字符串不保证0结尾）
  

## ks_string_util 介绍

ks_string_util是一个namespace，提供了诸多字符串操作方法，并且以后还可能会增加更多方法。

目前，ks_string_util提供的方法主要有：
  1. 编码转换：wstring_from_xxxx, wstring_to_xxxx
  2. 字符串解析：parse_xxxx
  3. 字符串化：to_string, to_wstring
  4. 字符串拼接：concat, join
  5. ... ...


## 版权和许可证
[Apache-2.0 license](LICENSE)
