# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Summary]

StormByte Base is the C++26 foundation of the StormByte suite.

Every other module links this library.
This repository is not Buffer, Config, Crypto, Database, Logger, Multimedia, Network or System — those live in their own repos and depend on Base.

Public headers under `StormByte/` cover exceptions, `Expected`, little-endian `Serializable`, strings, paths, UUID v4, bitmasks, clonable types, `ThreadLock`, and `StormByte::Type` concepts.

If you landed here from a release link and have not read the tree:

- What this module is, how to build it, and short examples: [README.md](https://github.com/StormBytePP/StormByte/blob/master/README.md)
- License: GNU Lesser General Public License version 3 or later, [LICENSE](https://github.com/StormBytePP/StormByte/blob/master/LICENSE)

## [Unreleased]

### Added

- **Type** object-semantics concepts — `TriviallyDefaultConstructible`, `TriviallyCopyConstructible`, `CopyAssignable`, `TriviallyCopyAssignable`, `TriviallyMoveConstructible`, `MoveAssignable`, `TriviallyMoveAssignable`, `Copyable`, `Movable`, rounding out the existing `CopyConstructible`/`MoveConstructible`/`TriviallyCopyable`/`TriviallyDestructible`/`Swappable` group.

### Changed

- **Template implementation split** — `Bitmask`, `Clonable`, `Iterable` (including its nested `Iterator`/`ConstIterator`), and `Serializable` now declare their members in the `.hxx` header and define them in a matching `.txx` file included at the bottom of the header. Public API and behavior are unchanged; `.txx` files are installed alongside the headers.
- **Serializable<T> explicit instantiation** — `bool`, all standard character/integer/floating-point types, and the four Codec-backed string types (`std::string`, `std::wstring`, `std::u16string`, `std::u32string`) are now explicitly instantiated inside the shared/dynamic library and declared `extern template` in the header, so consumers link against the library's code instead of re-instantiating it locally. `Serializable`'s container/pair/optional/trivial private helpers gained matching `requires` clauses (using the new `Type::TriviallyCopyable` concept) so the explicit instantiation only compiles the members that apply to each concrete `T`.

[Unreleased]: https://github.com/StormBytePP/StormByte/compare/1.1.0...HEAD

## [1.1.0] - 2026-09-13

### Added

- **Component** — wrapper for the module name on the component-prefixed `Exception` constructor. Call sites must now write `Exception(Component("Base64"), "…", args...)`.
- **Type** concepts — `Sized`, `SmartPointer`, `Swappable`, `DerivedFrom`, `EqualityComparable`, `ThreeWayComparable`, `Hashable`.
- **Type** range and pointer concepts — range/iterator category wrappers, range value/reference/difference aliases, `ExplicitlyConvertibleTo`, `NullablePointer`, `ByteInputRange`, and `ByteInputIterator` for public generic APIs.
- **Test handlers** — assertions evaluate operands once and report non-streamable values safely. New macros: `ASSERT_THROWS`, `ASSERT_NO_THROW`, `ASSERT_NEAR`, `ASSERT_CONTAINS`, `ASSERT_NOT_NULL`, covered by `TestHandlersTests`.
- **UTF8Error** — custom exception for invalid UTF-8 and wide-string Unicode input.

### Changed

- **Exception(component, fmt, args...)** — first argument is `Component`, not `std::string`. Source-breaking for every call that used the old two-string form.
- **Type** concepts — reorganized `type_traits.hxx` into focused container, wrapper, conversion, category, range, relation, and comparison groups without changing public contracts.

### Fixed

- **Exception(component, fmt, args...)** — `Exception(fmt, args...)` was chosen for every call with 2+ arguments, so the component string became the whole message and the format/args were discarded. `Component` makes the prefixed overload the only viable candidate.
- **Exception(format_string, Args...)** — the zero-argument path called `copy_str(fmt)`, which did not compile when that branch was selected (e.g. construction from `std::string_view`). It now copies `fmt.get()`, matching the documented “message as-is” behaviour.
- **Iterable::add** — `add(const value_type&)` requires `Type::CopyConstructible`; `add(value_type&&)` requires `Type::MoveConstructible`. clang-cl / MSVC no longer instantiate `push_back(const unique_ptr&)`.
- **Iterable::Iterator / ConstIterator** — `iterator_category` is taken from the underlying container iterator instead of being hardcoded as `random_access_iterator_tag`. `std::distance` / `std::advance` compile on `Iterable<std::list<…>>`, `map`, `set`, etc.
- **System::TempFileName** — builds the `mkstemp` template in a `std::string` sized to the prefix instead of a 256-byte buffer, so a long prefix no longer truncates the trailing `XXXXXX`.
- **Serializable<std::array>** — deserializes elements by index and rejects a serialized count that does not match the array extent.
- **Unexpected<Base>(Derived)** — no longer conflicts with `Unexpected<E>(error)` when `Base` and `Derived` are the same type.
- **String** — `ToLower` / `ToUpper` no longer pass negative signed values to the C character functions; negative byte sizes keep their sign instead of wrapping.
- **String UTF-8 conversion** — wide-string conversion is now locale-independent and rejects malformed Unicode input with `UTF8Error`.

[1.1.0]: https://github.com/StormBytePP/StormByte/compare/1.1.0...1.0.0

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