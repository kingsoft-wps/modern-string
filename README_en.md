# modern-string

The modern-string is an efficient C++ string implementation, with the core design intention of supporting low-cost slicing. 
At the same time, in order to facilitate project transformation, it is necessary to maintain maximum compatibility with the interface of std:: basic_string.

The core design concept of model string is:
  1. Multiple instances share internal string memory.
  2. The internal string data never changes.
  3. String slicing refers to fragment references of internal strings.
  4. Use SSO and COW optimization strategies.

The modern-string is based on the C++14 standard, but it is also possible to degrade to the C++11 standard easily.

The core string classes provided by model string are ks-basic_mutable_string and ks-basic_immutable_string.

In addition, due to reasons based on C++14, ks_basic_string_view is also provided to replace the std:: basic_string_view only provided by C++17.

As a convention, we have also defined the following common types:
  1. ks_mutable_string
  2. ks_mutable_wstring
  3. ks_immutable_string
  4. ks_immutable_wstring
  5. ks_string_view
  6. ks_wstring_view

It should be noted that the wstring in this library is actually a UTF-16 encoded string, even in Linux. 
This is completely different from std:: wstring, so be sure to distinguish it when using it.


## how to use

Usually, reference modern-string as a static library, and #include <ks_string.h> in the source code file.


## about ks_basic_mutable_string

  1. The return-type of operator\[] and at methods are always const_reference.
  2. The iterator is equivalent to const_iterator.
  3. Provide set-at method.
  4. Provide slice method.
  5. Provide methods such as trim and split, and so on.
  6. The return-type of methods such as substr and slice are immutable.


## about ks_basic_immutable_string

the ks_basic_immutable_string is a purer immutable string that can be analogized to string in Golang.

The key difference between it and ks_basic_mutable_string is that:
  1. No string modification methods are provided.
  2.The c_str method is not provided. (This implies that immutable strings do not guarantee zero endings)
  

## about ks_string_util

The ks_string_util is a namespace that provides many string manipulation methods, and more methods may be added in the future.

At present, the methods provided by ks_string_util mainly include:
  1. Encoding conversion: wstring_from_xxxx, wstring_to_xxxx
  2. String parsing: parse_xxxx
  3. Stringization: to_string, to_wstring
  4. String concatenating: concat, join
  5. ... ...


## License
[Apache-2.0 license](LICENSE)
