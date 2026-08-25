# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-08-20

Initial public release of **StormByte** (Base module): the foundational C++26 library that provides cross-platform utilities, exception handling, serialization, string helpers, system functions and other core building blocks used by the rest of the StormByte ecosystem.

### Added

- **Exception** hierarchy with DLL-safe `const char*` storage and `std::format` support
- **Expected<T, E>** wrapper around `std::expected` with reference support and shared error ownership (`Unexpected` helpers)
- **Serializable** template framework
  - Automatic handling of trivially copyable types, STL containers, `std::pair` and `std::optional`
  - Specializations for `std::string`, `std::wstring` and `std::u16string`
  - Zero-copy deserialization via `std::span<const std::byte>`
  - Endian-aware trivial serialization (little-endian on the wire)
- **Base64** encode / decode (standard alphabet, whitespace-tolerant decoder)
- **Bitmask** CRTP utility for type-safe flag enums (`|`, `&`, `^`, `~`, `Add`, `Remove`, `Has`, `HasAny`, `HasNone`…)
- **Clonable** CRTP interface for smart-pointer based cloning (`std::shared_ptr` / `std::unique_ptr`)
- **ThreadLock** – lightweight owner-tracked lock with reentrant semantics for the owning thread
- **String** utilities
  - Case conversion (`ToLower` / `ToUpper`)
  - Splitting (`Explode`, `Split`)
  - Human-readable number and byte-size formatting
  - UTF-8 ↔ wide string conversion
  - Byte vector ↔ string conversion
  - Whitespace removal, newline sanitization, integer detection
- **System** utilities
  - `TempFileName()`
  - `CurrentPath()` (current working directory)
  - `ExecutablePath()` (directory of the running executable)
  - `Sleep()` for any `std::chrono::duration`
- **UUID** generation (`GenerateUUIDv4()` – RFC 4122 version 4)
- Modern C++20/23 **type traits and concepts** (`Type::` namespace)
- Platform detection macros (`WINDOWS`, `LINUX`, `MACOS`, `BIT32`/`BIT64`, compiler detection)
- Visibility macros for shared library builds
- Comprehensive unit tests (including corruption / robustness tests for serialization)

### Changed

- Serialization protocol: all size fields on the wire are now consistently encoded as `std::uint64_t` (8 bytes fixed). This guarantees correct interchange between 32-bit and 64-bit platforms.
- `System::CurrentPath()` now returns the real current working directory. Previous behaviour moved to the new `ExecutablePath()`.

### Fixed

- Documentation of `Base64Decode` now accurately describes its lenient behaviour (whitespace ignored, stops at first `=`, padding accepted but not strictly validated).
- Documentation vs implementation mismatch of `CurrentPath()`.

### Notes

- This is the first stable release of the StormByte Base library.
- The binary serialization format is now considered stable.
- Requires a C++26 compliant compiler and CMake ≥ 3.12.
- The library is designed as the foundation for the rest of the StormByte modules (Buffer, Config, Crypto, Database, Logger, Multimedia, Network, System…).

[1.0.0]: https://github.com/StormBytePP/StormByte/releases/tag/1.0.0
