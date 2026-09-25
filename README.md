# StormByte

![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey)
![C++26](https://img.shields.io/badge/C%2B%2B-26-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.28+-064F8C?logo=cmake&logoColor=white)
![License: LGPL v3 or commercial](https://img.shields.io/badge/License-LGPL_v3_or_commercial-blue.svg)
[![CI](https://github.com/StormBytePP/StormByte/actions/workflows/ci.yml/badge.svg)](https://github.com/StormBytePP/StormByte/actions/workflows/ci.yml)
[![Sponsor](https://img.shields.io/badge/Sponsor-StormBytePP-ea4aaa?logo=githubsponsors)](https://github.com/sponsors/StormBytePP)

This repository is **StormByte Base**: the C++26 foundation of the StormByte suite.

It is the module every other StormByte library links. Public headers live under `StormByte/` and cover exceptions, `Expected`, little-endian serialization, `CString` / `WCString`, `BinaryData`, `Size`, UUID v4, bitmasks, clonable types, a reentrant `ThreadLock`, and the `StormByte::Type` concepts.

The suite is split on purpose. Buffer, Config, Crypto, Database, Logger, Multimedia, Network, String and System are **other repositories**. They depend on this one; this one does not implement them.

## What this module does

- **Exceptions** — `StormByte::Exception` with `std::format` messages stored in `CString` (`what()` is a `const char*` owned by the exception).
- **Error** — `Domain`, `Category`, `Code` and `Fault` for `std::error_code`. `Fault` is not thrown; its text is a `CString`.
- **Expected** — `Expected<T, E>` on top of `std::expected`, references via `reference_wrapper`, errors as `shared_ptr<E>`, plus `Unexpected`.
- **Serialization** — `Serializable<T>` to `BinaryData`, always little-endian, no BOM and no version tag. Optional / pair / container / trivial / `Detail::Codec<T>`.
- **CString / WCString** — owned NUL-terminated narrow and wide buffers, safe to use across a DLL boundary. Not `std::string` / `std::wstring`. Construct from C string, `string_view` / `wstring_view` and `string` / `wstring` (copy onto Base's heap). Content equality, `<=>`, `swap` and `std::hash`.
- **BinaryData** — owned contiguous `std::byte` sequence, safe to use across a DLL boundary. Same kind of API as `std::vector<std::byte>`. Lengths use `Size`. `HexDump` prints offset + hex + ASCII; column count is `std::size_t`.
- **Size** — `uint64_t` byte count, same width on every host and safe across a DLL. IEC and SI units, `*` / `/` / `%`, IEC text as `CString`.
- **UUID** — RFC 4122 version 4 (`GenerateUUIDv4`).
- **Bitmask** — CRTP flags over `Type::UnsignedEnum`.
- **Clonable** — virtual `Clone` / `Move` into `shared_ptr` or `unique_ptr`.
- **ThreadLock** — owner-thread reentry; `Unlock` from a non-owner is a no-op.
- **Type concepts** — `StormByte::Type::*` (`String`, `Container`, `Optional`, `Pair`, enums, …). No `enable_if` / `void_t` next to them.
- **Platform / visibility** — `WINDOWS` / `LINUX` / `MACOS`, `BIT32` / `BIT64`, `CLANG` / `GCC` / `MSVC` (clang-cl is `CLANG`, not `MSVC`).

## The rest of the suite

| Module | Role | API |
| --- | --- | --- |
| **Base** | This repository | [/StormByte](https://dev.stormbyte.org/StormByte) |
| [Buffer](https://github.com/StormBytePP/StormByte-Buffer) | FIFO, SharedFIFO, Ring, Producer/Consumer and multi-stage pipelines | [/StormByte-Buffer](https://dev.stormbyte.org/StormByte-Buffer) |
| [Config](https://github.com/StormBytePP/StormByte-Config) | Human-readable text and versioned binary documents (groups, lists, raw bytes) | [/StormByte-Config](https://dev.stormbyte.org/StormByte-Config) |
| [Crypto](https://github.com/StormBytePP/StormByte-Crypto) | Hash, compress, encrypt, sign and key agreement — Crypto++ never leaves the private tree | [/StormByte-Crypto](https://dev.stormbyte.org/StormByte-Crypto) |
| [Database](https://github.com/StormBytePP/StormByte-Database) | One API over SQLite, PostgreSQL and MariaDB | [/StormByte-Database](https://dev.stormbyte.org/StormByte-Database) |
| [Logger](https://github.com/StormBytePP/StormByte-Logger) | Stream logger with levels, headers, human-readable sizes and redaction (`ThreadedLog`) | [/StormByte-Logger](https://dev.stormbyte.org/StormByte-Logger) |
| [Multimedia](https://github.com/StormBytePP/StormByte-Multimedia) | Decode, encode and containers without raw FFmpeg types; codecs enabled only if present | [/StormByte-Multimedia](https://dev.stormbyte.org/StormByte-Multimedia) |
| [Network](https://github.com/StormBytePP/StormByte-Network) | Framed packets, Client/Server, IPv4/IPv6 TCP and Buffer pipelines (compress/encrypt) | [/StormByte-Network](https://dev.stormbyte.org/StormByte-Network) |
| [String](https://github.com/StormBytePP/StormByte-String) | Suite text type and helpers (case, split, UTF-8, human-readable numbers and byte sizes) | [/StormByte-String](https://dev.stormbyte.org/StormByte-String) |
| [System](https://github.com/StormBytePP/StormByte-System) | Processes, pipes and environment variables across Linux, Windows and macOS | [/StormByte-System](https://dev.stormbyte.org/StormByte-System) |

## Table of Contents

- [What this module does](#what-this-module-does)
- [The rest of the suite](#the-rest-of-the-suite)
- [Installation](#installation)
- [Usage](#usage)
- [Exceptions](#exceptions)
- [Expected](#expected)
- [Error](#error)
- [CString / WCString](#cstring--wcstring)
- [BinaryData](#binarydata)
- [Size](#size)
- [Serialization](#serialization)
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

### Error

`Fault` wraps a `std::error_code`. Across a DLL use `Fault::what()` (`CString`), not `error_code::message()`.

```cpp
#include <StormByte/error.hxx>
#include <iostream>
#include <system_error>

using namespace StormByte;

int main() {
	const std::error_code code = Error::Code::Unknown;
	const Error::Fault fault{code};
	if (fault)
		std::cerr << fault.what() << std::endl;
}
```

A module adds its own enum, specializes `Error::Domain`, and puts `make_error_code` next to the enum so ADL fills `std::error_code`. The category singleton lives in that module’s `.cxx`.

### CString / WCString

Owned buffers. `operator bool` is true when the pointer is not null: `""` / `L""` are valid empty text; a default-constructed object is null.

Construct from `const char*` / `const wchar_t*` (null stays null), from `std::string_view` / `std::wstring_view`, and from `const std::string&` / `const std::wstring&`. Those last two **copy** onto Base's heap. They are not a heap steal. An empty `string` / view yields `""` / `L""`, not a null buffer.

`==` / `!=` / `<=>` compare text, not addresses. Two nulls are equal; null is not equal to `""` / `L""` and orders before any text. `swap` exchanges buffers. `std::hash` hashes the text (`0` when null), so the types work in `std::set` and `std::unordered_set`.

`explicit operator const char*` / `const wchar_t*` has the same lifetime as `std::string::c_str()` / `std::wstring::c_str()`. Implicit `std::string` / `std::wstring` and `operator<<` are inline (caller CRT).

```cpp
#include <StormByte/cstring.hxx>
#include <StormByte/wcstring.hxx>
#include <iostream>
#include <set>
#include <string>

using namespace StormByte;

int main() {
	CString text("hello");
	if (text)
		std::cout << text << " " << text.Length() << std::endl;

	CString from_std{std::string("hello")};
	if (text == from_std && text == "hello")
		std::cout << "same text" << std::endl;

	text.Reset();
	if (!text)
		std::cout << "null" << std::endl;

	CString empty("");
	if (empty && empty.Length() == 0 && text < empty)
		std::cout << "empty but valid" << std::endl;

	std::set<CString> ordered{CString("b"), CString("a")};

	WCString wide(L"wide");
	std::wcout << wide << std::endl;
}
```

### BinaryData

`BinaryData` is the suite’s owned raw-byte container. Use it wherever a module would otherwise put `std::vector<std::byte>` in a public signature.

`std::vector` is not a safe ABI type between two copies of a C++ runtime. A vector allocated in the application and grown, returned or destroyed inside a StormByte shared library (or the other way around) uses two heaps. On Windows that is a hard crash when CRTs differ; on Unix it fails when libc++ and libstdc++ mix.

`BinaryData` owns its storage on StormByte Base’s heap. Construction, growth and destruction always run in this library. Other suite modules can carry payloads, encoded blobs, file images or wire fragments without exporting `std::vector<std::byte>`.

It is not text (`CString`) and not a structured document. Lengths are `StormByte::Size`. Member names stay lowercase to match the STL.

For `<algorithm>` and `std::ranges` it supports everything `std::vector<std::byte>` supports on a contiguous sequence of bytes: copy / transform / sort / reverse / rotate / unique / remove / replace / partition / heap / set operations / binary search / permutations, plus iterators, `std::span` and insert / erase / assign / append / `operator+=` / emplace. `std::iota` is the exception that is *also* true of `std::vector<std::byte>`: `std::byte` is an enum class and has no `operator++`.

`at()` throws `OutOfBoundsError`. `operator[]` is unchecked, like `std::vector`.

Compare with another `BinaryData` or with `std::span<const std::byte>` (`==`, `!=`, `<=>`, both operand orders).

**Hex dump.** `HexDump()` and `HexDump(std::size_t columns)` return a `CString`. Each line is an 8-digit offset, a row of hex bytes, and the same bytes as ASCII (non-printable as `.`). `columns` is a **row width**, not a byte length — it is `std::size_t`, not `Size`. `0` prints every byte on one line. The default is 16 columns.

**`std::vector` and `std::span`.** You can build a `BinaryData` from a `span` or from a caller-owned `vector`. You can view the bytes as a `span` (implicit). You can copy them out to a `vector` (`explicit operator std::vector<std::byte>`). The rvalue overloads *look* like a move: the source is emptied after the copy. They are not a heap steal. Base cannot donate its pointer to a foreign `vector`, and it cannot adopt a caller `vector` pointer. Peak usage is two copies during the transfer.

`append(BinaryData&&)` / `operator+=(BinaryData&&)` is different: both sides live on Base’s heap, so that move is real when `*this` is empty.

`Serializable<BinaryData>` uses the container path. The wire is the same as `std::vector<std::byte>`: `uint64` little-endian count, then the payload.

```cpp
#include <StormByte/binary_data.hxx>
#include <StormByte/serializable.hxx>
#include <StormByte/size.hxx>
#include <algorithm>
#include <iostream>
#include <ranges>
#include <span>
#include <vector>

using namespace StormByte;

int main() {
	BinaryData payload{std::byte{0xDE}, std::byte{0xAD}, std::byte{0xBE}, std::byte{0xEF}};
	payload.push_back(std::byte{0x00});
	payload += payload.span().first(2);

	std::ranges::reverse(payload);
	std::sort(payload.begin(), payload.end());

	if (!payload.empty())
		payload.front() = std::byte{0x01};

	const Size n = payload.size();
	std::cout << static_cast<unsigned long long>(n.Value()) << std::endl;
	std::cout << payload.HexDump(8) << std::endl;

	std::vector<std::byte> caller = static_cast<std::vector<std::byte>>(payload);
	BinaryData back{std::move(caller)};

	BinaryData extra{std::byte{0xFF}};
	back += std::move(extra);

	auto blob = Serializable<BinaryData>(back).Serialize();
	auto loaded = Serializable<BinaryData>::Deserialize(blob);
	if (loaded)
		std::cout << (loaded.value() == back) << std::endl;
}
```

```cpp
#include <StormByte/binary_data.hxx>
#include <StormByte/size.hxx>
#include <algorithm>
#include <array>

using namespace StormByte;

BinaryData from_range() {
	const std::array<unsigned char, 4> raw{1, 2, 3, 4};
	BinaryData data(raw);
	data.insert(data.begin() + 1, std::byte{9});
	data.erase(data.begin() + 2);
	return data;
}
```

### Size

`Size` is a `uint64_t` byte count. It is the same width on 32-bit and 64-bit hosts and safe to return across a DLL. It is not a `std::size_t`.

`Size{100}` is valid. A negative integer is undefined and `assert`s when assertions are on. Read the count with `Value()` or `explicit operator uint64_t`. There is no `size_t` conversion; cast `Value()` yourself if a host `size_t` is required.

IEC units (`B`, `KiB`, `MiB`, `GiB`, `TiB`, `PiB`, `EiB`) and SI units (`KB`, `MB`, `GB`, `TB`, `PB`, `EB`) are `constexpr` objects. Scale a unit with `*`: `4 * MiB`, `4.2 * KiB` (nearest byte), `GiB * 2`. Scale an existing `Size` with a positive integer: `4 * s`, `s * 4`. `s / 4` and `s % 4` return `uint64_t` (how many pieces fit, leftover bytes). There is no `Size * Size` and no floating-point scale of a `Size`.

`operator CString` builds IEC text (`B`, `KiB`, …) in the StormByte DLL. `operator std::string` and `operator<<` are inline and copy that text into the caller’s heap.

```cpp
#include <StormByte/size.hxx>
#include <iostream>

using namespace StormByte;

int main() {
	const Size chunk{4 * MiB + 512 * KiB};
	const Size twice = 2 * chunk;
	const auto pieces = twice / 1024;
	const auto leftover = twice % 1024;

	std::cout << chunk << std::endl;
	std::cout << twice.Value() << " " << pieces << " " << leftover << std::endl;
}
```

### Serialization

Wire is little-endian. `Serialize()` returns `BinaryData`. `Deserialize` reads a prefix; leftover bytes stay with the caller. Custom types specialize `StormByte::Detail::Codec<T>` (`Size` / `Write` / `Read`), not `Serializable<T>`.

`BinaryData` is a `Type::Container` of `std::byte`. No `Codec` specialization is required; the container path writes the same layout as `std::vector<std::byte>`.

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
	auto vback = Serializable<std::vector<int>>::Deserialize(vblob.span());
}
```

`wstring` / `u16string` / `u32string` travel as `uint64` UTF-8 length + UTF-8 bytes. Host `wchar_t` width never appears on the wire.

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
#include <StormByte/binary_data.hxx>
#include <StormByte/type_traits.hxx>
#include <string>
#include <vector>
#include <optional>

using namespace StormByte;

static_assert(Type::String<std::string>);
static_assert(Type::Container<std::vector<int>>);
static_assert(Type::Container<BinaryData>);
static_assert(Type::Sized<BinaryData>);
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

Issues and pull requests belong on this repository. Fork and open a PR against `master`.

Read [CONTRIBUTING.md](CONTRIBUTING.md) before you send a patch (copyright assignment and review rules). Coding rules are in [CODING_STYLE.md](CODING_STYLE.md).

## License

Since 2.0.0, original source in this repository is dual-licensed: GNU Lesser General Public License v3 or later, or a commercial license from the copyright holder (David C. Manuelda <StormByte@gmail.com>).

The grant applies only to original StormByte source in this repository. It does not cover other StormByte modules or third-party material shipped here (including everything under `thirdparty/`), which remains under its own license. Neither license grants patent rights.

See [LICENSE](LICENSE) for the dual-license notice and [COPYING.LGPLv3](COPYING.LGPLv3) for the full GNU LGPL version 3 text. Also <https://www.gnu.org/licenses/lgpl-3.0.html>.

## Support

StormByte is developed in spare time. Sponsorship is optional and does not buy features, priority or support.

- [GitHub Sponsors](https://github.com/sponsors/StormBytePP)
- [PayPal](https://paypal.me/StormBytePP)
