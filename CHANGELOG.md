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

- **Type** concepts — `Sized`, `SmartPointer`, `Swappable`, `DerivedFrom`, `EqualityComparable`, `ThreeWayComparable`, `Hashable`
- **Component** — wraps a module name for the component-prefixed `Exception` constructor; see Changed.
- **Test handlers** — existing assertions now evaluate operands once and safely report non-streamable values; added `ASSERT_THROWS`, `ASSERT_NO_THROW`, `ASSERT_NEAR`, `ASSERT_CONTAINS` and `ASSERT_NOT_NULL`, with dedicated `TestHandlersTests` coverage.

### Changed

- **Exception(component, fmt, args...)** — `component` is now a `Component` wrapper instead of a plain `std::string`: `Exception(Component("Base64"), "...", args...)`. Source-breaking for every call site using the old two-string form.

### Fixed

- **Iterable::add** — `add(const value_type&)` is constrained with `Type::CopyConstructible`; `add(value_type&&)` with `Type::MoveConstructible`. clang-cl / MSVC no longer instantiate `push_back(const unique_ptr&)` when the container holds move-only values.
- **Iterable::Iterator / ConstIterator** — `iterator_category` is now derived from the underlying container iterator instead of being hardcoded to `random_access_iterator_tag`. Fixes a compile error in `std::distance` / `std::advance` and similar algorithms on `Iterable<std::list<...>>`, `Iterable<std::map<...>>`, `Iterable<std::set<...>>`, etc.
- **Exception(component, fmt, args...)** — this overload was silently hijacked by `Exception(fmt, args...)` for every call with 2+ arguments: the component string was used as the whole message and the real format string/args were discarded (e.g. `Base64Error("Base64", "Invalid character '{}' in input", c)` produced just `"Base64"`). Wrapping `component` in `Component` makes the component overload the only viable candidate.
- **Exception(format_string, Args...)** zero-argument path — fixed `copy_str(fmt)` not compiling whenever that branch was actually selected by overload resolution (e.g. constructing from a `std::string_view`); it now copies `fmt.get()` as-is, per the documented "message as-is" behavior.
- **System::TempFileName** — builds the `mkstemp` template in a `std::string` sized to the prefix instead of a fixed 256-byte buffer, so a long `prefix` no longer truncates the required trailing `XXXXXX` and silently fails.
- **Serializable<std::array>** — deserialization now assigns fixed-size array elements by index and rejects serialized element counts that do not exactly match the array extent.
- **Unexpected<Base>(Derived)** — no longer conflicts with the ordinary `Unexpected<E>(error)` overload when `Base` and `Derived` are the same type.
- **String case conversion and byte sizes** — `ToLower`/`ToUpper` no longer invoke the C character functions with signed negative values, and negative byte sizes retain their sign instead of wrapping to a huge unsigned value.

[Unreleased]: https://github.com/StormBytePP/StormByte/compare/1.0.0...HEAD

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