# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Summary]

StormByte Base is the C++26 foundation of the StormByte suite.

Every other module links this library.
This repository is not Buffer, Config, Crypto, Database, Logger, Multimedia, Network or System — those live in their own repos and depend on Base.

Public headers under `StormByte/` cover exceptions, `Expected`, little-endian `Serializable`, strings, paths, UUID v4, bitmasks, clonable types, `ThreadLock`, `Size`, and `StormByte::Type` concepts.

If you landed here from a release link and have not read the tree:

- What this module is, how to build it, and short examples: [README.md](https://github.com/StormBytePP/StormByte/blob/master/README.md)
- Since 2.0.0 license changed: original source in this repository is dual-licensed, LGPL v3 or later **or** a commercial license from the copyright holder. The grant does not cover other StormByte modules or `thirdparty/`. [LICENSE](https://github.com/StormBytePP/StormByte/blob/master/LICENSE)

## [Unreleased]

### Fixed

- **`FindStormByte`.** `Buffer` pulls `Logger`, `String` and `System`. `Logger` and `System` pull `String`. `Database` pulls `Logger`. `Multimedia` pulls `Buffer`, same as `Crypto` and `Network`. `Config` and `String` still link only the core.

[Unreleased]: https://github.com/StormBytePP/StormByte/compare/2.0.0...HEAD

## [2.0.0] - 2026-09-25

### Added

- **BinaryData** — owned contiguous sequence of `std::byte`, safe to use across a DLL boundary. Same kind of API as `std::vector<std::byte>` (iterators, `<algorithm>`, `std::ranges`, `std::span`, insert / erase / assign / `append` / `emplace_back`). Lengths use `StormByte::Size`. Storage lives on Base's heap; the public type is not `std::vector<std::byte>`. `at()` throws `OutOfBoundsError`. Construct from `span`, pointer+`Size`, range, initializer list, `string_view`, and a caller-owned `std::vector<std::byte>` (lvalue copies and leaves the vector; rvalue copies onto Base's heap then clears the vector — looks like a move, not a heap steal). Convert out with implicit `span` and `explicit operator std::vector<std::byte>` (`const&` copies and leaves `*this`; `&&` copies onto the caller CRT then clears `*this`). `append(BinaryData&&)` is a real same-heap move. `Type::Container` / `Type::Sized` / `Type::HasPushBack` / `Type::ByteInputRange` match; `Type::String` does not. `Serializable<BinaryData>` uses the container path (same wire as `std::vector<std::byte>`: `uint64` LE count + payload). Covered by `BinaryDataTests` and `SerializationTests`.
- **CString** — owned NUL-terminated buffer, safe to use across a DLL boundary. Not a `std::string`. Copy, move, `Reset`, `swap`, `Length`, observer `operator[]` (indices `[0, Length()]`; `Length()` is the trailing NUL; null or past `Length()` is undefined and `assert`s when assertions are on), explicit `operator bool` (non-null pointer; `""` is valid empty text), explicit `const char*` (same lifetime as `std::string::c_str()`), explicit `std::string_view` (empty view when null; `[0, Length())`; same lifetime), implicit `std::string`, `operator<<`, content `==` / `!=` / `<=>`, and `std::hash`. Covered by `CStringTests`.
- **WCString** — wide counterpart of `CString` (`wchar_t` / `std::wstring` / `std::wstring_view` / `std::wostream`). Same comparison, hash, subscript and lifetime rules as `std::wstring::c_str()`. Covered by `WCStringTests`.
- **Error** — `Domain`, `Category`, `Code` (`Success`, `Unknown`) and `Fault`. Modules specialize `Domain` for their enums; `make_error_code` lives next to the enum so ADL feeds `std::error_code`. Category singletons stay in the module `.cxx`. `Fault` holds the code and a `CString`. Not thrown. Covered by `ErrorTests`.
- **ClonableTests** — clone / move coverage for `shared_ptr` and `unique_ptr`.
- **Size** — `uint64_t` byte count, same width on every host and safe across a DLL. `Size{100}` is valid; a negative integer is undefined and `assert`s when assertions are on. `Value()` and explicit `operator uint64_t` only (no `size_t` conversion). `+` / `-` / `+=` / `-=` with `assert` on wrap or underflow. IEC (`B`, `KiB`, `MiB`, `GiB`, `TiB`, `PiB`, `EiB`) and SI (`KB`, `MB`, `GB`, `TB`, `PB`, `EB`) units. `4 * MiB`, `4.2 * KiB` (nearest byte) and `GiB * 2` yield `Size`. `4 * s` / `s * 4` scale an existing `Size` by a positive integer. `s / 4` and `s % 4` return `uint64_t` (how many pieces fit, leftover bytes). No `Size * Size`. Explicit instantiations of the templates live in the StormByte DLL. `operator CString` (IEC text, built in the DLL) and inline `operator std::string` (caller heap via that `CString`). Covered by `SizeTests`.

### Changed

- **License change** — original source in this repository is dual-licensed: GNU LGPL v3 or later, or a commercial license from the copyright holder. The grant applies only to original StormByte source in this repository. It does not cover other StormByte modules or third-party material (including `thirdparty/`). No patent rights are granted.
- *Breaking*: public `std::vector<std::byte>` is gone. `Serializable<T>::Serialize()`, `Detail::Codec<T>::Write`, `Deserialize(const …&)` of a blob, `Base64Decode` and `Base64Encode` of owned bytes now use `BinaryData` (plus the existing `span` overloads). The wire layout is unchanged (`uint64` LE counts, little-endian trivials, UTF-8 wide strings). Helpers that concatenated blobs are `append_bytes` on `BinaryData`. A caller-owned `std::vector<std::byte>` is only for local conversion at the boundary (`explicit` construct / convert). Downstream modules that specialize `Codec` or store a serialize result must retarget `BinaryData`.
- **Visibility** — `STORMBYTE_PUBLIC` comes first on a function declaration (`STORMBYTE_PUBLIC CString Foo();`). clang-cl rejects `__declspec` after a reference return. `class STORMBYTE_PUBLIC` stays on the type. Exported templates are declared `extern template STORMBYTE_PUBLIC` / `extern template class STORMBYTE_PUBLIC` in the header and instantiated in the `.cxx` with `STORMBYTE_INSTANTIATE` (`dllexport` on Windows, empty on ELF so GCC does not warn `-Wattributes`). The `extern` line in the header is what ELF uses to export; do not put `STORMBYTE_INSTANTIATE` on that line.
- **Exception** — the message is stored in a `CString`. Copy, move, assign and the destructor are defaulted. The change is transparent: `what()` and the constructors are unchanged, consumers do not rebuild against a new layout contract, and the DLL boundary is the same (`const char*` owned by the exception).
- **Type::String** — also matches `StormByte::String::String` and `StormByte::String::WString` (forward-declared in Base). They stay out of `Type::Container`.
- **Type::Sized** — `size()` may be implicitly convertible to `std::size_t` (STL containers) **or** be `StormByte::Size` (suite octet lengths). `Size` still has no implicit conversion to an integer. `std::vector` still matches. Covered by `TypeTraitsTests`.
- **GenerateUUIDv4** — returns `CString` instead of `std::string`.
- **Base64** — both `Base64Encode` overloads return `CString`. `Base64Decode` returns `BinaryData`. `Base64Encode` takes `const BinaryData&` or `std::span<const std::byte>`. Input text is still `std::string_view`.
- **Tests** — suite section headers (`// -------------------`) match in the body and in `main`. UUID, Base64, Bitmask, Clonable, Exception, Expected, ThreadLock, TestHandlers, Iterable, Serialization, TypeTraits, Size and BinaryData cover the public surface (format, padding, invalid input, clone independence, non-owner unlock, bounds, wire, corruption, units, `*` / `/` / `%`, `<algorithm>` / ranges / iterators). `Type::Sized` covers a container whose `size()` returns `StormByte::Size`; `HasSubscript` covers a `Size` index. `Base64Decode` of an encode result uses an explicit `std::string_view`. `SerializationTests` cover `BinaryData` as the serialize blob, `Deserialize` from `BinaryData` and from `span`, the same wire as a local `std::vector<std::byte>`, truncation, a huge size field, bit-flip / byte-overwrite / random corruption, trailing garbage and `std::optional<BinaryData>`.

### Removed

- **String** (`StormByte::String` helpers, `string.hxx` / `string.cxx`) — leaves Base. Helpers move to a new StormByte-String library.
- **System** (`StormByte::System` in Base: `TempFileName`, `CurrentPath`, `ExecutablePath`, `Sleep`) — leaves Base. Absorbed by the existing StormByte-System module.
- **UTF8Error** / **SystemError** — leave Base with String and System. Derived exceptions that remain are `DeserializeError`, `OutOfBoundsError` and `Base64Error`.
- **CoreApiTests** — coverage lives in `ClonableTests` and `ErrorTests`.

[2.0.0]: https://github.com/StormBytePP/StormByte/compare/1.2.0...2.0.0

## [1.2.0] - 2026-09-17

### Added

- `String` helpers that only read the input now take `std::string_view` /
  `std::wstring_view`: `IsNumeric`, `ToLower`, `ToUpper`, `Explode`, `Split`,
  `UTF8Encode`, `UTF8Decode`, `SanitizeNewlines`, `ToByteVector`,
  `RemoveWhitespace`, `IsInteger`. `std::string`, `std::wstring` and
  literals convert to the views.
- `Base64Decode(std::string_view)` replaces `Base64Decode(const std::string&)`.

### Changed

- `SanitizeNewlines` no longer builds a throwaway copy and a `std::regex`;
  it walks the view and maps `\r\n` to `\n`.

[1.2.0]: https://github.com/StormBytePP/StormByte/compare/1.1.1...1.2.0

## [1.1.1] - 2026-09-15

### Added

- **Type** category concepts — `LvalueReference` and `RvalueReference` (`std::is_lvalue_reference` / `std::is_rvalue_reference`), next to the existing `Reference`.
- **SystemError** — `Exception` specialization for `StormByte::System` helpers.

### Changed

- **System::ExecutablePath** — throws `SystemError` instead of returning `"NOPATH"`. Paths longer than 256 bytes are resolved.

### Fixed

- **Type::CopyConstructible / CopyAssignable** — no longer wrap `std::is_copy_*` alone. They require a real `T{src}` / `dest = src` and recurse into `value_type` only when `T` is a `Type::Container` and not a `Type::View`. `std::span<std::unique_ptr<T>>` stays copyable; `std::vector<std::unique_ptr<T>>` does not.
- **Type::HasPushBack / HasPushFront / HasInsert** — accept `value_type&&` as well as `const value_type&`. `std::vector<std::unique_ptr<T>>` is now `HasPushBack`, so `Iterable::add` takes the `push_back` path.
- **Iterable::add** — one forwarding `add(T&&)` plus a by-value sink for braced-init (`add({"k", v})`). The sink writes to the container; it does not call `add` again (that recursed until a stack overflow). clang-cl / MSVC no longer instantiate `push_back(const unique_ptr&)`.
- **Iterable** container constructor — single `Iterable(C&&)` constrained to `Container`. An lvalue copy requires `Type::CopyConstructible` on `value_type`; a move requires `Type::MoveConstructible`.
- **Iterable** copy / move — defaulted copy and copy-assign require `Type::CopyConstructible` / `Type::CopyAssignable` on `Container`; move and move-assign require `Type::MoveConstructible` / `Type::MoveAssignable`. `Iterable<std::vector<std::unique_ptr<T>>>` is move-only.
- **Iterable::Iterator / ConstIterator** — arithmetic (`+=`, `-=`, `+`, difference) uses `std::advance` / `std::distance` instead of `m_it += n`, so it compiles on bidirectional containers (`list`, `map`, `set`). `reference` / `pointer` come from `std::iterator_traits` of the wrapped iterator, not `Container::reference` (`*it` on `std::set` is `const T&`).
- **System::TempFileName** — throws `SystemError` instead of `std::runtime_error`. The file is created and left on disk; the caller unlinks it. Windows still only uses the first three characters of `prefix`.
- **System::ExecutablePath** — grows the platform buffer instead of truncating at 256 bytes (`GetModuleFileNameW` / `readlink`). Failure is `SystemError`, not a fake path.
- **System::CurrentPath** — wraps `std::filesystem::filesystem_error` in `SystemError` instead of leaking a standard exception.
- **Iterable copy / copy-assign** — user-provided members with `if constexpr` on `Type::CopyConstructible` / `CopyAssignable`. clang-cl / MSVC `dllexport` of a derived class no longer instantiates `vector<unique_ptr<T>>` copy via `requires = default`.

[1.1.1]: https://github.com/StormBytePP/StormByte/compare/1.1.0...1.1.1

## [1.1.0] - 2026-09-13

### Added

- **Component** — wrapper for the module name on the component-prefixed `Exception` constructor. Call sites must now write `Exception(Component("Base64"), "…", args...)`.
- **Type** concepts — `Sized`, `SmartPointer`, `Swappable`, `DerivedFrom`, `EqualityComparable`, `ThreeWayComparable`, `Hashable`.
- **Type** range and pointer concepts — range/iterator category wrappers, range value/reference/difference aliases, `ExplicitlyConvertibleTo`, `NullablePointer`, `ByteInputRange`, and `ByteInputIterator` for public generic APIs.
- **Type** object-semantics concepts — `TriviallyDefaultConstructible`, `TriviallyCopyConstructible`, `CopyAssignable`, `TriviallyCopyAssignable`, `TriviallyMoveConstructible`, `MoveAssignable`, `TriviallyMoveAssignable`, `Copyable`, `Movable`, rounding out the existing `CopyConstructible`/`MoveConstructible`/`TriviallyCopyable`/`TriviallyDestructible`/`Swappable` group.
- **Test handlers** — assertions evaluate operands once and report non-streamable values safely. New macros: `ASSERT_THROWS`, `ASSERT_NO_THROW`, `ASSERT_NEAR`, `ASSERT_CONTAINS`, `ASSERT_NOT_NULL`, covered by `TestHandlersTests`.
- **UTF8Error** — custom exception for invalid UTF-8 and wide-string Unicode input.

### Changed

- **Exception(component, fmt, args...)** — first argument is `Component`, not `std::string`. Source-breaking for every call that used the old two-string form.
- **Type** concepts — reorganized `type_traits.hxx` into focused container, wrapper, conversion, category, range, relation, and comparison groups without changing public contracts.
- **Template implementation split** — `Bitmask`, `Clonable`, `Iterable` (including its nested `Iterator`/`ConstIterator`), and `Serializable` now declare their members in the `.hxx` header and define them in a matching `.txx` file included at the bottom of the header. Public API and behavior are unchanged; `.txx` files are installed alongside the headers.
- **Serializable<T> explicit instantiation** — `bool`, all standard character/integer/floating-point types, and the four Codec-backed string types (`std::string`, `std::wstring`, `std::u16string`, `std::u32string`) are now explicitly instantiated inside the shared/dynamic library and declared `extern template` in the header, so consumers link against the library's code instead of re-instantiating it locally.

### Removed

- **`StormByte::swap_endian`** — the transitional alias of `Type::Detail::swap_endian` in the root `StormByte` namespace. Call `StormByte::Type::Detail::swap_endian` directly instead.

### Fixed

- **Exception(component, fmt, args...)** — `Exception(fmt, args...)` was chosen for every call with 2+ arguments, so the component string became the whole message and the format/args were discarded. `Component` makes the prefixed overload the only viable candidate.
- **Exception(format_string, Args...)** — the zero-argument path called `copy_str(fmt)`, which did not compile when that branch was selected (e.g. construction from `std::string_view`). It now copies `fmt.get()`, matching the documented “message as-is” behaviour.
- **Iterable::add** — `add(const value_type&)` requires `Type::CopyConstructible`; `add(value_type&&)` requires `Type::MoveConstructible`. clang-cl / MSVC no longer instantiate `push_back(const unique_ptr&)`.
- **Iterable::Iterator / ConstIterator** — `iterator_category` is taken from the underlying container iterator instead of being hardcoded as `random_access_iterator_tag`. `std::distance` / `std::advance` compile on `Iterable<std::list<…>>`, `map`, `set`, etc.
- **System::TempFileName** — builds the `mkstemp` template in a `std::string` sized to the prefix instead of a 256-byte buffer, so a long prefix no longer truncates the trailing `XXXXXX`.
- **Serializable<std::array>** — deserializes elements by index and rejects a serialized count that does not match the array extent.
- **Serializable constrained helpers** — container/pair/optional/trivial private helpers are member templates (`template<typename U = T> requires Type::* <U>`) so `template class Serializable<bool>` / `char` no longer instantiate bodies that call `.size()` on a scalar (clang-cl / clang++).
- **Unexpected<Base>(Derived)** — no longer conflicts with `Unexpected<E>(error)` when `Base` and `Derived` are the same type.
- **String** — `ToLower` / `ToUpper` no longer pass negative signed values to the C character functions; negative byte sizes keep their sign instead of wrapping.
- **String UTF-8 conversion** — wide-string conversion is now locale-independent and rejects malformed Unicode input with `UTF8Error`.

[1.1.0]: https://github.com/StormBytePP/StormByte/compare/1.0.0...1.1.0

## [1.0.0] - 2026-09-05

Initial public release of StormByte Base.

### Added

- **Exception** hierarchy with DLL-safe `const char*` storage and `std::format` support
- **Expected<T, E>** on `std::expected` with reference support and shared error ownership (`Unexpected` helpers)
- **Serializable** template
  - Trivially copyable types, STL containers, `std::pair` and `std::optional`
  - `Detail::Codec` for `std::string`, `std::wstring`, `std::u16string` and `std::u32string`
  - Zero-copy input via `std::span<const std::byte>`
  - Little-endian on the wire
- **Base64** encode / decode (standard alphabet, whitespace-tolerant decoder)
- **Bitmask** CRTP for unsigned flag enums (`|`, `&`, `^`, `~`, `Add`, `Remove`, `Has`, `HasAny`, `HasNone`)
- **Clonable** for smart-pointer clone / move (`std::shared_ptr` / `std::unique_ptr`)
- **ThreadLock** — owner-tracked lock; the owner may reenter; `Unlock` from a non-owner is a no-op
- **String** utilities
  - Case conversion (`ToLower` / `ToUpper`)
  - Splitting (`Explode`, `Split`)
  - Human-readable number and byte-size formatting
  - UTF-8 ↔ wide string conversion
  - Byte vector ↔ string conversion
  - Whitespace removal, newline sanitization, integer detection
- **System** utilities
  - `TempFileName()`
  - `CurrentPath()` (process cwd)
  - `ExecutablePath()` (directory of the running executable)
  - `Sleep()` for any `std::chrono::duration`
- **UUID** generation (`GenerateUUIDv4()` — RFC 4122 version 4)
- **Type** concepts under `StormByte::Type` (`String`, `Container`, `Optional`, `Pair`, enums, …)
- Platform macros (`WINDOWS`, `LINUX`, `MACOS`, `UNIX`, `BIT32` / `BIT64`)
- Compiler macros: `CLANG` (including clang-cl), `GCC`, `MSVC` (MSVC only; not clang-cl)
- Visibility macros for shared builds
- Unit tests, including serialization robustness

### Changed

- Serialization size fields on the wire are `std::uint64_t` (8 bytes). 32-bit and 64-bit hosts interchange.
- `System::CurrentPath()` is the process cwd. The old meaning is `ExecutablePath()`.

### Fixed

- `Base64Decode` documentation matches the lenient decoder (whitespace ignored, stops at first `=`, padding accepted but not strictly checked).
- `CurrentPath()` documentation matches the implementation.

### Notes

- First stable release of StormByte Base.
- The binary serialization layout is stable.
- Needs a C++26 compiler and CMake ≥ 3.28.
- Foundation for Buffer, Config, Crypto, Database, Logger, Multimedia, Network and System.

[1.0.0]: https://github.com/StormBytePP/StormByte/releases/tag/1.0.0