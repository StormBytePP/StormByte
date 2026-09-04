# StormByte

![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey)
![C++26](https://img.shields.io/badge/C%2B%2B-26-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.28+-064F8C?logo=cmake&logoColor=white)
![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)
[![CI](https://github.com/StormBytePP/StormByte/actions/workflows/ci.yml/badge.svg)](https://github.com/StormBytePP/StormByte/actions/workflows/ci.yml)
[![Sponsor](https://img.shields.io/badge/Sponsor-StormBytePP-ea4aaa?logo=githubsponsors)](https://github.com/sponsors/StormBytePP)

This repository is **StormByte Base**: the C++26 foundation of the StormByte suite.

It is the module every other StormByte library links. Public headers live under `StormByte/` and cover exceptions, `Expected`, little-endian serialization, strings, paths, UUID v4, bitmasks, clonable types, a reentrant `ThreadLock`, and the `StormByte::Type` concepts.

The suite is split on purpose. Buffer, Config, Crypto, Database, Logger, Multimedia, Network and System are **other repositories**. They depend on this one; this one does not implement them.

## What this module does

- **Exceptions** — `StormByte::Exception` with `std::format` messages and `const char*` storage (DLL-safe on Windows).
- **Expected** — `Expected<T, E>` on top of `std::expected`, references via `reference_wrapper`, errors as `shared_ptr<E>`, plus `Unexpected`.
- **Serialization** — `Serializable<T>` to `vector<byte>`, always little-endian, no BOM and no version tag. Optional / pair / container / trivial / `Detail::Codec<T>`.
- **Strings** — case, split, UTF-8 ↔ wide, human-readable numbers and byte sizes, newline sanitizing.
- **System** — temp files, cwd, executable directory, `Sleep` for `chrono` durations.
- **UUID** — RFC 4122 version 4 (`GenerateUUIDv4`).
- **Bitmask** — CRTP flags over `Type::UnsignedEnum`.
- **Clonable** — virtual `Clone` / `Move` into `shared_ptr` or `unique_ptr`.
- **ThreadLock** — owner-thread reentry; `Unlock` from a non-owner is a no-op.
- **Type concepts** — `StormByte::Type::*` (`String`, `Container`, `Optional`, `Pair`, enums, …). No `enable_if` / `void_t` next to them.
- **Platform / visibility** — `WINDOWS` / `LINUX` / `MACOS`, `BIT32` / `BIT64`, `CLANG` / `GCC` / `MSVC` (clang-cl is `CLANG`, not `MSVC`).

## The rest of the suite

| Module | Role |
| --- | --- |
| [Base](https://github.com/StormBytePP/StormByte) | This repository |
| [Buffer](https://github.com/StormBytePP/StormByte-Buffer) | FIFO, SharedFIFO, Ring, Producer/Consumer and multi-stage pipelines |
| [Config](https://github.com/StormBytePP/StormByte-Config) | Human-readable text and versioned binary documents (groups, lists, raw bytes) |
| [Crypto](https://github.com/StormBytePP/StormByte-Crypto) | Hash, compress, encrypt, sign and key agreement — Crypto++ never leaves the private tree |
| [Database](https://github.com/StormBytePP/StormByte-Database) | One API for SQLite, PostgreSQL and MariaDB: prepared statements and RAII transactions |
| [Logger](https://github.com/StormBytePP/StormByte-Logger) | Stream logger with levels, headers, human-readable sizes and redaction (`ThreadedLog`) |
| [Multimedia](https://github.com/StormBytePP/StormByte-Multimedia) | Decode, encode and containers without raw FFmpeg types; codecs enabled only if present |
| [Network](https://github.com/StormBytePP/StormByte-Network) | Framed packets, Client/Server, IPv4/IPv6 TCP and Buffer pipelines (compress/encrypt) |
| [System](https://github.com/StormBytePP/StormByte-System) | Processes, pipes and environment variables across Linux, Windows and macOS |

Docs sites (when published): [Base](https://dev.stormbyte.org/StormByte), [Buffer](https://dev.stormbyte.org/StormByte-Buffer), [Config](https://dev.stormbyte.org/StormByte-Config), [Crypto](https://dev.stormbyte.org/StormByte-Crypto), [Database](https://dev.stormbyte.org/StormByte-Database), [Logger](https://dev.stormbyte.org/StormByte-Logger), [Multimedia](https://dev.stormbyte.org/StormByte-Multimedia), [Network](https://dev.stormbyte.org/StormByte-Network), [System](https://dev.stormbyte.org/StormByte-System).

## Table of Contents

- [What this module does](#what-this-module-does)
- [The rest of the suite](#the-rest-of-the-suite)
- [Installation](#installation)
- [Usage](#usage)
  - [Exceptions](#exceptions)
  - [Expected](#expected)
  - [Serialization](#serialization)
  - [Strings](#strings)
  - [System](#system)
  - [UUID](#uuid)
  - [ThreadLock](#threadlock)
  - [Clonable](#clonable)
  - [Type concepts](#type-concepts)
  - [Bitmask](#bitmask)
- [Contributing](#contributing)
- [License](#license)

## Installation

Needs a C++26 compiler and CMake 3.28 or newer.

```sh
git clone https://github.com/StormBytePP/StormByte.git
cd StormByte
cmake -S . -B build
cmake --build build
```

## Usage

Headers are `#include <StormByte/….hxx>`. Namespace root is `StormByte`.

### Exceptions

```cpp
#include <StormByte/exception.hxx>
#include <iostream>

using namespace StormByte;

void process_data(int value) {
	if (value < 0)
		throw Exception("Invalid value: {}", value);
}

int main() {
	try {
		process_data(-5);
	} catch (const Exception& e) {
		std::cerr << e.what() << std::endl;
	}
}
```

### Expected

Errors are `shared_ptr<E>`. Read them with `result.error()->what()`.

```cpp
#include <StormByte/expected.hxx>
#include <StormByte/exception.hxx>
#include <iostream>

using namespace StormByte;

Expected<int, Exception> divide(int a, int b) {
	if (b == 0)
		return Unexpected<Exception>("Division by zero");
	return a / b;
}
```

`Error::Code` exists for `std::error_code` integration. The enum has no enumerators yet.

### Serialization

Wire is little-endian. `Deserialize` reads a prefix; leftover bytes stay with the caller. Custom types specialize `StormByte::Detail::Codec<T>` (`Size` / `Write` / `Read`), not `Serializable<T>`.

```cpp
#include <StormByte/serializable.hxx>
#include <iostream>
#include <string>
#include <vector>

using namespace StormByte;

int main() {
	int number = 42;
	auto blob = Serializable<int>(number).Serialize();
	auto back = Serializable<int>::Deserialize(blob);
	if (back)
		std::cout << back.value() << std::endl;

	std::string text = "Hello, World!";
	auto sblob = Serializable<std::string>(text).Serialize();
	auto sback = Serializable<std::string>::Deserialize(sblob);

	std::vector<int> numbers{1, 2, 3};
	auto vblob = Serializable<std::vector<int>>(numbers).Serialize();
	auto vback = Serializable<std::vector<int>>::Deserialize(
		std::span<const std::byte>(vblob.data(), vblob.size()));
}
```

`wstring` / `u16string` / `u32string` travel as `uint64` UTF-8 length + UTF-8 bytes. Host `wchar_t` width never appears on the wire.

### Strings

```cpp
#include <StormByte/string.hxx>
#include <iostream>
#include <queue>

using namespace StormByte::String;

int main() {
	auto parts = Explode("path/to/file.txt", '/');
	auto words = Split("Hello World from StormByte");
	auto n = HumanReadable(1234567890ull, Format::HumanReadableNumber);
	auto sz = HumanReadable(1536000ull, Format::HumanReadableBytes);
	auto utf8 = UTF8Encode(L"Hello, 世界!");
	auto wide = UTF8Decode(utf8);
}
```

### System

`CurrentPath()` is the process cwd. `ExecutablePath()` is the directory of the running binary (`NOPATH` if it cannot be resolved).

```cpp
#include <StormByte/system.hxx>
#include <chrono>

using namespace StormByte::System;
using namespace std::chrono_literals;

int main() {
	auto tmp = TempFileName("myapp");
	auto cwd = CurrentPath();
	auto exe = ExecutablePath();
	Sleep(500ms);
}
```

### UUID

```cpp
#include <StormByte/uuid.hxx>
#include <iostream>

int main() {
	std::cout << StormByte::GenerateUUIDv4() << std::endl;
}
```

### ThreadLock

The owner may `Lock()` again. Another thread blocks. `Unlock()` from a non-owner does nothing.

### Clonable

```cpp
#include <StormByte/clonable.hxx>
#include <memory>

using namespace StormByte;

class Shape : public Clonable<Shape, std::shared_ptr<Shape>> {
public:
	virtual std::shared_ptr<Shape> Clone() const override = 0;
	virtual std::shared_ptr<Shape> Move() override = 0;
};
```

### Type concepts

```cpp
#include <StormByte/type_traits.hxx>
#include <string>
#include <vector>
#include <optional>

using namespace StormByte;

static_assert(Type::String<std::string>);
static_assert(Type::Container<std::vector<int>>);
static_assert(Type::Optional<std::optional<int>>);
```

`Type::Detail::swap_endian` always reverses bytes. Serializable decides when to call it (host not little-endian).

### Bitmask

Needs an unsigned scoped enum. Operators return the derived CRTP type. Helpers are `Add`, `Remove`, `Has`, `HasAny`, `HasNone`, `Value` (not `Any` / `None`).

```cpp
#include <StormByte/bitmask.hxx>

using namespace StormByte;

enum class MyFlags : uint8_t { FlagA = 0x01, FlagB = 0x02 };

class MyBitmask : public Bitmask<MyBitmask, MyFlags> {
public:
	using Bitmask<MyBitmask, MyFlags>::Bitmask;
};
```

## Contributing

Issues only on this repository. Fork and open a pull request against `master`.

## License

GNU Lesser General Public License version 3 or later. See [LICENSE](LICENSE) and <https://www.gnu.org/licenses/lgpl-3.0.html>.
