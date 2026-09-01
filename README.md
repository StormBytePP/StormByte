# StormByte
![Linux](https://img.shields.io/badge/Linux-Supported-1793D1?logo=linux&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-Supported-0078D6?logo=windows&logoColor=white)
![macOS](https://img.shields.io/badge/macOS-Supported-0078D6?logo=apple&logoColor=white)
![C++26](https://img.shields.io/badge/C%2B%2B-26-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.28+-064F8C?logo=cmake&logoColor=white)
![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)
[![CI](https://github.com/StormBytePP/StormByte/actions/workflows/ci.yml/badge.svg)](https://github.com/StormBytePP/StormByte/actions/workflows/ci.yml)

StormByte is a comprehensive, cross-platform C++ library aimed at easing system programming, configuration management, logging, and database handling tasks. This library provides a unified API that abstracts away the complexities and inconsistencies of different platforms (Windows, Linux, macOS).

This repository is **StormByte Base**: templates, exceptions, expected, serialization, strings, system helpers and type concepts. The other modules listed below live in their own repositories and depend on this one.

## Features

- **Configuration Management**: Provides an intuitive API for reading and writing configuration files ([StormByte-Config](https://github.com/StormBytePP/StormByte-Config)).
- **Database Handling**: Includes SQLite / PostgreSQL / MariaDB support while hiding vendor internals ([StormByte-Database](https://github.com/StormBytePP/StormByte-Database)).
- **Buffers**: Provides a variety of buffer types for managing byte data in both single-threaded and multi-threaded environments ([StormByte-Buffer](https://github.com/StormBytePP/StormByte-Buffer)).
- **Logging**: Offers various logging levels, customizable formats, and supports outputs to files, streams, or other destinations ([StormByte-Logger](https://github.com/StormBytePP/StormByte-Logger)).
- **Multimedia**: Includes a set of classes to work with multimedia files ([StormByte-Multimedia](https://github.com/StormBytePP/StormByte-Multimedia)).
- **Network**: Contains everything needed to handle network communication portable to Linux, Windows and macOS ([StormByte-Network](https://github.com/StormBytePP/StormByte-Network)).
- **System Operations**: Manages pipes, processes, and system variables seamlessly across different platforms ([StormByte-System](https://github.com/StormBytePP/StormByte-System)).
- **Serialization**: Little-endian binary encode/decode for trivial types, STL containers, pairs, optionals and text. Custom types extend via `StormByte::Detail::Codec<T>`.
- **Cryptographic functions**: Encrypting, decrypting, hashing, signing and verifying signatures ([StormByte-Crypto](https://github.com/StormBytePP/StormByte-Crypto)).
- **Type concepts**: C++26 concepts under `StormByte::Type` (`String`, `Container`, `Optional`, `Pair`, …) used by the rest of the suite.

## Table of Contents

- [Repository](#repository)
- [Installation](#installation)
- [Modules](#modules)
  - **Base**
    - [Exception Handling](#exception-handling)
    - [Expected and Error Handling](#expected-and-error-handling)
    - [Serialization](#serialization)
    - [String Utilities](#string-utilities)
    - [System Utilities](#system-utilities)
    - [Thread Synchronization](#thread-synchronization)
    - [Clonable Interface](#clonable-interface)
    - [Type Traits](#type-traits)
  - [Buffer](https://dev.stormbyte.org/StormByte-Buffer)
  - [Config](https://dev.stormbyte.org/StormByte-Config)
  - [Crypto](https://dev.stormbyte.org/StormByte-Crypto)
  - [Database](https://dev.stormbyte.org/StormByte-Database)
  - [Logger](https://dev.stormbyte.org/StormByte-Logger)
  - [Multimedia](https://dev.stormbyte.org/StormByte-Multimedia)
  - [Network](https://dev.stormbyte.org/StormByte-Network)
  - [System](https://dev.stormbyte.org/StormByte-System)
- [Contributing](#contributing)
- [License](#license)

## Repository

You can visit the code repository at [GitHub](https://github.com/StormBytePP/StormByte)

## Installation

### Prerequisites

Ensure you have the following installed:

- C++26 compatible compiler
- CMake 3.28 or higher

### Building

To build the library, follow these steps:

```sh
git clone https://github.com/StormBytePP/StormByte.git
cd StormByte
mkdir build
cd build
cmake ..
make
```

## Modules

StormByte Library is composed of several modules:

### Base

The Base component is the core of the library containing only templates, string helpers, serialization, system helpers and the base exception framework.

### Exception Handling

The `Exception` class provides a consistent, cross-platform mechanism for error handling in the StormByte library. It uses `const char*` for internal storage to avoid issues with `std::string` across DLL boundaries on Windows.

#### Features
- **Formatted Messages**: Supports `std::format` for constructing exception messages
- **Cross-Platform**: Works consistently across Windows, Linux and macOS
- **DLL-Safe**: Uses `const char*` internally to avoid std::string ABI issues
- **Inheritable**: Can be subclassed for domain-specific exceptions

#### Example Usage

```cpp
#include <StormByte/exception.hxx>
#include <iostream>

using namespace StormByte;

void process_data(int value) {
    if (value < 0) {
        throw Exception("Invalid value: {}", value);
    }
    if (value > 100) {
        throw Exception(std::string("Value too large: ") + std::to_string(value));
    }
}

int main() {
    try {
        process_data(-5);
    } catch (const Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    try {
        process_data(150);
    } catch (const Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
```

### Expected and Error Handling

StormByte provides `Expected<T, E>` as an alias for `std::expected` with enhanced support for reference types and shared error ownership. It also includes the `Unexpected` helper for creating error values and a custom error code system.

#### Features
- **Reference Support**: Automatically uses `std::reference_wrapper` for reference types
- **Shared Error Ownership**: Uses `std::shared_ptr<E>` for error types to allow safe copying
- **Formatted Errors**: `Unexpected` supports `std::format` for error messages
- **Custom Error Codes**: Integration with `std::error_code` via `Error::Code` and `Error::Category`

#### Example Usage

##### Basic Expected Usage

```cpp
#include <StormByte/expected.hxx>
#include <StormByte/exception.hxx>
#include <iostream>

using namespace StormByte;

Expected<int, Exception> divide(int a, int b) {
    if (b == 0) {
        return Unexpected<Exception>("Division by zero");
    }
    return a / b;
}

Expected<std::string, Exception> read_config(const std::string& key) {
    if (key.empty()) {
        return Unexpected<Exception>("Empty key: {}", key);
    }
    // Simulate reading config
    return "config_value";
}

int main() {
    auto result = divide(10, 2);
    if (result) {
        std::cout << "Result: " << result.value() << std::endl;
    } else {
        std::cerr << "Error: " << result.error()->what() << std::endl;
    }

    auto config = read_config("");
    if (!config) {
        std::cerr << "Config error: " << config.error()->what() << std::endl;
    }

    return 0;
}
```

##### Custom Error Codes

```cpp
#include <StormByte/error.hxx>
#include <iostream>

using namespace StormByte;

// Define custom error codes by extending Error::Code enum
// (currently empty, but can be extended by library users)

int main() {
    // Get the error category singleton
    const auto& cat = Error::category();
    std::cout << "Error category: " << cat.name() << std::endl;

    // Create error codes (when codes are defined)
    // std::error_code ec = make_error_code(Error::Code::SomeError);

    return 0;
}
```

### Serialization

The `Serializable` template class encodes and decodes a value into `std::vector<std::byte>`. Dispatch is by concept, in this order: optional, pair, container, trivially copyable, then `Detail::Codec<T>` for everything else.

The wire format is always **little-endian**. There is no BOM, no version tag and no runtime endian sniff. `Read` consumes a prefix and leaves leftover bytes to the caller.

#### Features
- **Automatic Type Detection**: Selects the path from `Type::Optional` / `Pair` / `Container` / trivial / `Codec`
- **Container Support**: `std::vector`, `std::map`, `std::set`, … (`uint64` count LE, then elements)
- **Pair and Optional Support**: Built-in. Optionals are `bool has_value` then the value; never a raw `optional` memcpy (padding on MSVC)
- **Text**: `std::string` is `uint64` byte count LE + raw bytes. `wstring` / `u16string` / `u32string` use the same layout with a **UTF-8** payload (host `wchar_t` width never appears on the wire)
- **Extensible**: Specialize `StormByte::Detail::Codec<T>` — do not specialize `Serializable<T>` itself
- **Type-Safe**: Uses `Expected<T, DeserializeError>` for deserialization
- **Span input**: `Deserialize(std::span<const std::byte>)` does not copy the input buffer
- **Alignment-Safe**: Trivial types use `std::memcpy` plus `Type::Detail::swap_endian` on big-endian hosts

#### Built-in Type Support

The library automatically handles:
- **Trivially copyable types**: `int`, `float`, `double`, `bool` (only bytes `0` and `1`), enums, small PODs
- **Containers**: anything matching `Type::Container` (has `begin` / `end` / `value_type` and is not a `Type::String`)
- **Pairs**: anything matching `Type::Pair` (`first` and `second`)
- **Optionals**: `std::optional<T>`
- **Strings**: `std::string`, `std::wstring`, `std::u16string`, `std::u32string` via `Detail::Codec` — not as a container of code units

#### Example Usage with Built-in Types

```cpp
#include <StormByte/serializable.hxx>
#include <iostream>
#include <string>
#include <vector>

using namespace StormByte;

int main() {
    // Serialize an integer (wire is little-endian: 42 → 2a 00 00 00)
    int number = 42;
    Serializable<int> intSer(number);
    std::vector<std::byte> intData = intSer.Serialize();

    auto deserializedInt = Serializable<int>::Deserialize(intData);
    if (deserializedInt) {
        std::cout << "Deserialized int: " << deserializedInt.value() << std::endl;
    }

    // Serialize a string (uint64 length LE + raw bytes)
    std::string text = "Hello, World!";
    Serializable<std::string> strSer(text);
    std::vector<std::byte> strData = strSer.Serialize();

    auto deserializedStr = Serializable<std::string>::Deserialize(strData);
    if (deserializedStr) {
        std::cout << "Deserialized string: " << deserializedStr.value() << std::endl;
    }

    // Serialize a vector
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    Serializable<std::vector<int>> vecSer(numbers);
    std::vector<std::byte> vecData = vecSer.Serialize();

    auto deserializedVec = Serializable<std::vector<int>>::Deserialize(vecData);
    if (deserializedVec) {
        std::cout << "Deserialized vector: ";
        for (int val : deserializedVec.value()) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    // Deserialize from a span (no extra copy of the input buffer)
    std::span<const std::byte> dataSpan(vecData.data(), vecData.size());
    auto spanResult = Serializable<std::vector<int>>::Deserialize(dataSpan);
    if (spanResult) {
        std::cout << "Deserialized from span" << std::endl;
    }

    return 0;
}
```

#### Extending Serialization for Custom Types

Specialize `StormByte::Detail::Codec<T>` with `Size`, `Write` and `Read`. `Serializable<T>` will pick it up on the complex path. The three members must agree: `Size(v) == Write(v).size()`, and `Read` consumes that prefix.

```cpp
#include <StormByte/helpers.hxx>
#include <StormByte/serializable.hxx>
#include <iostream>
#include <string>
#include <vector>

using namespace StormByte;

struct Point {
    int x;
    int y;
};

template<>
struct StormByte::Detail::Codec<Point> {
    static std::size_t Size(const Point& v) noexcept {
        return Serializable<int>::Size(v.x) + Serializable<int>::Size(v.y);
    }

    static std::vector<std::byte> Write(const Point& v) noexcept {
        std::vector<std::byte> buffer;
        append_vector(buffer, Serializable<int>(v.x).Serialize());
        append_vector(buffer, Serializable<int>(v.y).Serialize());
        return buffer;
    }

    static Expected<Point, DeserializeError> Read(std::span<const std::byte> data) noexcept {
        auto x = Serializable<int>::Deserialize(data);
        if (!x)
            return Unexpected(x.error());

        const std::size_t x_size = Serializable<int>::Size(x.value());
        if (x_size > data.size())
            return Unexpected<DeserializeError>("Insufficient data for Point.y");

        auto y = Serializable<int>::Deserialize(data.subspan(x_size));
        if (!y)
            return Unexpected(y.error());

        return Point{ x.value(), y.value() };
    }
};

int main() {
    Point p = {10, 20};
    Serializable<Point> serializer(p);

    std::vector<std::byte> data = serializer.Serialize();
    std::cout << "Serialized Point (" << p.x << ", " << p.y << ")" << std::endl;

    auto deserialized = Serializable<Point>::Deserialize(data);
    if (deserialized) {
        Point deserializedPoint = deserialized.value();
        std::cout << "Deserialized Point: ("
                  << deserializedPoint.x << ", "
                  << deserializedPoint.y << ")" << std::endl;
    } else {
        std::cerr << "Deserialization failed: "
                  << deserialized.error()->what() << std::endl;
    }

    return 0;
}
```

Wide text on the wire is UTF-8, so a `wstring` blob and a `u16string` blob of the same characters compare equal and survive Windows (`wchar_t` = 2) vs POSIX (`wchar_t` = 4):

```cpp
std::wstring wide = L"StormByte";
std::u16string u16 = u"StormByte";
auto a = Serializable<std::wstring>(wide).Serialize();
auto b = Serializable<std::u16string>(u16).Serialize();
assert(a == b);
```

**Note**: Specialize `Detail::Codec<T>` from other StormByte modules. Do not specialize `Serializable<T>` and do not reintroduce `SerializeComplex` / `DeserializeComplex`.

### String Utilities

The `String` namespace provides a comprehensive set of utilities for string manipulation, conversion, and formatting. These functions help with common string operations while maintaining cross-platform compatibility.

#### Features
- **Case Conversion**: Convert strings to uppercase or lowercase
- **String Splitting**: Split strings by delimiters or whitespace
- **Human-Readable Formatting**: Format numbers and byte sizes in human-readable formats
- **UTF-8 Encoding/Decoding**: Convert between wide strings and UTF-8
- **Byte Vector Conversion**: Convert between strings and byte vectors
- **Newline Sanitization**: Normalize line endings across platforms

#### Available Functions

##### Case Conversion
```cpp
#include <StormByte/string.hxx>
#include <iostream>

using namespace StormByte::String;

int main() {
    std::string text = "Hello World";

    std::string lower = ToLower(text);  // "hello world"
    std::string upper = ToUpper(text);  // "HELLO WORLD"

    std::cout << "Lower: " << lower << std::endl;
    std::cout << "Upper: " << upper << std::endl;

    return 0;
}
```

##### String Splitting
```cpp
#include <StormByte/string.hxx>
#include <iostream>
#include <queue>

using namespace StormByte::String;

int main() {
    // Split by delimiter into a queue
    std::string path = "path/to/file.txt";
    std::queue<std::string> parts = Explode(path, '/');

    while (!parts.empty()) {
        std::cout << parts.front() << std::endl;
        parts.pop();
    }

    // Split by whitespace into a vector
    std::string sentence = "Hello World from StormByte";
    std::vector<std::string> words = Split(sentence);

    for (const auto& word : words) {
        std::cout << word << std::endl;
    }

    return 0;
}
```

##### Human-Readable Formatting
```cpp
#include <StormByte/string.hxx>
#include <iostream>

using namespace StormByte::String;

int main() {
    // Format numbers with locale-specific thousand separators
    uint64_t largeNumber = 1234567890;
    std::string formatted = HumanReadable(largeNumber, Format::HumanReadableNumber);
    std::cout << "Number: " << formatted << std::endl;  // "1,234,567,890"

    // Format byte sizes in human-readable format
    uint64_t fileSize = 1536000;
    std::string readableSize = HumanReadable(fileSize, Format::HumanReadableBytes);
    std::cout << "Size: " << readableSize << std::endl;  // "1.46 MiB"

    return 0;
}
```

##### UTF-8 Encoding/Decoding
```cpp
#include <StormByte/string.hxx>
#include <iostream>

using namespace StormByte::String;

int main() {
    // Convert wide string to UTF-8
    std::wstring wide = L"Hello, 世界!";
    std::string utf8 = UTF8Encode(wide);

    std::cout << "UTF-8 encoded string" << std::endl;

    // Convert UTF-8 back to wide string
    std::wstring recovered = UTF8Decode(utf8);

    return 0;
}
```

##### Byte Vector Conversion
```cpp
#include <StormByte/string.hxx>
#include <iostream>
#include <vector>

using namespace StormByte::String;

int main() {
    // Convert string to byte vector
    std::string text = "Hello, World!";
    std::vector<std::byte> bytes = ToByteVector(text);

    std::cout << "Byte vector size: " << bytes.size() << std::endl;

    // Convert byte vector back to string
    std::string recovered = FromByteVector(bytes);
    std::cout << "Recovered string: " << recovered << std::endl;

    return 0;
}
```

### System Utilities

The `System` namespace provides cross-platform utilities for common system operations such as temporary file management, path operations, and timing functions.

#### Features
- **Temporary File Management**: Safely create temporary files with custom prefixes
- **Path Operations**: Get current working directory and executable path
- **Sleep Functions**: Cross-platform sleep with any `std::chrono::duration`

#### Available Functions

##### Temporary File Creation
```cpp
#include <StormByte/system.hxx>
#include <iostream>
#include <fstream>

using namespace StormByte::System;

int main() {
    // Create a temporary file with default prefix
    std::filesystem::path tempFile = TempFileName();
    std::cout << "Temp file: " << tempFile << std::endl;

    // Create a temporary file with custom prefix
    std::filesystem::path customTemp = TempFileName("myapp");
    std::cout << "Custom temp file: " << customTemp << std::endl;

    // Use the temporary file
    std::ofstream file(customTemp);
    file << "Temporary data" << std::endl;
    file.close();

    // Clean up
    std::filesystem::remove(customTemp);

    return 0;
}
```

##### Current Path
```cpp
#include <StormByte/system.hxx>
#include <iostream>

using namespace StormByte::System;

int main() {
    // Get the current working directory
    std::filesystem::path current = CurrentPath();
    std::cout << "Current directory: " << current << std::endl;

    return 0;
}
```

##### Sleep Function
```cpp
#include <StormByte/system.hxx>
#include <iostream>
#include <chrono>

using namespace StormByte::System;
using namespace std::chrono_literals;

int main() {
    std::cout << "Sleeping for 2 seconds..." << std::endl;

    // Sleep for 2 seconds
    Sleep(2s);

    std::cout << "Awake!" << std::endl;

    // Sleep for 500 milliseconds
    Sleep(500ms);

    // Sleep for 1 minute
    Sleep(std::chrono::minutes(1));

    return 0;
}
```

### UUID

The `StormByte` library provides a simple helper to generate RFC4122-compliant version 4 UUIDs.

- **Function**: `StormByte::GenerateUUIDv4()`
- **Returns**: a 36-character, lowercase UUID string in the form
    `xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx` (where `y` is one of `8`, `9`, `a`, or `b`).
- **Exception safety**: marked `noexcept` in the public header.
- **Randomness**: implementation prefers the OS CSPRNG and falls back to a PRNG if necessary.

#### Example
```cpp
#include <StormByte/uuid.hxx>
#include <iostream>

int main() {
        // Generate a new UUID v4 string
        std::string uuid = StormByte::GenerateUUIDv4();
        std::cout << "Generated UUID v4: " << uuid << std::endl;
        return 0;
}
```

### Thread Synchronization

The `ThreadLock` class provides a lightweight, reentrant lock with thread ownership tracking. Unlike standard mutexes, it allows the owning thread to call `Lock()` multiple times without blocking itself.

#### Features
- **Reentrant for Owner**: The owning thread can lock multiple times without deadlock
- **Thread Ownership**: Tracks which thread owns the lock
- **Simple API**: Just `Lock()` and `Unlock()` methods
- **Non-copyable, Non-movable**: Ensures proper resource management

#### Example Usage

```cpp
#include <StormByte/thread_lock.hxx>
#include <iostream>
#include <thread>
#include <vector>

using namespace StormByte;

class SharedResource {
private:
    ThreadLock lock;
    int value = 0;

public:
    void increment() {
        lock.Lock();
        ++value;
        // Reentrant: same thread can call nested_operation
        nested_operation();
        lock.Unlock();
    }

    void nested_operation() {
        lock.Lock();  // Same thread, doesn't block
        value *= 2;
        lock.Unlock();
    }

    int get_value() {
        lock.Lock();
        int v = value;
        lock.Unlock();
        return v;
    }
};

int main() {
    SharedResource resource;
    std::vector<std::thread> threads;

    // Spawn multiple threads
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&resource]() {
            for (int j = 0; j < 100; ++j) {
                resource.increment();
            }
        });
    }

    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final value: " << resource.get_value() << std::endl;
    return 0;
}
```

### Clonable Interface

The `Clonable` template class provides a CRTP (Curiously Recurring Template Pattern) base for types that need cloning support with smart pointers. It works with both `std::shared_ptr` and `std::unique_ptr`.

#### Features
- **Smart Pointer Agnostic**: Works with `std::shared_ptr` and `std::unique_ptr`
- **Type-Safe Cloning**: Virtual `Clone()` method returns the correct smart pointer type
- **Factory Method**: `MakePointer` for creating instances with the correct pointer type
- **CRTP Pattern**: Compile-time polymorphism for efficient cloning

#### Example Usage

```cpp
#include <StormByte/clonable.hxx>
#include <iostream>
#include <memory>

using namespace StormByte;

// Base class using shared_ptr
class Shape : public Clonable<Shape, std::shared_ptr<Shape>> {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
    virtual std::shared_ptr<Shape> Clone() const override = 0;
};

class Circle : public Shape {
private:
    double radius;

public:
    Circle(double r) : radius(r) {}

    void draw() const override {
        std::cout << "Circle with radius: " << radius << std::endl;
    }

    std::shared_ptr<Shape> Clone() const override {
        return MakePointer<Circle>(radius);
    }
};

class Rectangle : public Shape {
private:
    double width, height;

public:
    Rectangle(double w, double h) : width(w), height(h) {}

    void draw() const override {
        std::cout << "Rectangle " << width << "x" << height << std::endl;
    }

    std::shared_ptr<Shape> Clone() const override {
        return MakePointer<Rectangle>(width, height);
    }
};

int main() {
    auto circle = Shape::MakePointer<Circle>(5.0);
    auto rect = Shape::MakePointer<Rectangle>(10.0, 20.0);

    circle->draw();
    rect->draw();

    // Clone shapes
    auto circle_copy = circle->Clone();
    auto rect_copy = rect->Clone();

    circle_copy->draw();
    rect_copy->draw();

    return 0;
}
```

### Type Traits

StormByte exposes compile-time type inspection as **concepts** under `StormByte::Type`. There are no `is_string<T>::value` / `void_t` traits anymore.

- **`Type::String`**: `std::string`, `std::wstring`, `std::u16string` or `std::u32string`
- **`Type::Container`**: has `begin()`, `end()` and `value_type`, and is not a `Type::String`
- **`Type::Optional`**: exactly `std::optional<U>`
- **`Type::Pair`**: has accessible `first` and `second` (broader than `std::pair`)
- **`Type::Reference`**: lvalue or rvalue reference
- **`Type::Enum` / `Type::ScopedEnum` / `Type::ToUnderlying`**: enumerations
- **`Type::Variant` / `Type::VariantHasType<T, U>`**: `std::variant` alternatives

`Type::Detail::swap_endian` reverses the object representation of a trivially copyable value (`std::byteswap` for integrals, `std::bit_cast` otherwise). Serializable is the one that decides *when* to call it (host ≠ little-endian).

#### Example Usage

```cpp
#include <StormByte/type_traits.hxx>
#include <iostream>
#include <vector>
#include <optional>
#include <string>

using namespace StormByte;

template<typename T>
void process() {
    if constexpr (Type::String<T>) {
        std::cout << "String type" << std::endl;
    } else if constexpr (Type::Container<T>) {
        std::cout << "Container type" << std::endl;
    } else if constexpr (Type::Optional<T>) {
        std::cout << "Optional type" << std::endl;
    } else if constexpr (Type::Pair<T>) {
        std::cout << "Pair type" << std::endl;
    } else {
        std::cout << "Other type" << std::endl;
    }
}

int main() {
    process<std::string>();                    // String type
    process<std::u16string>();                 // String type (not a container)
    process<std::vector<int>>();               // Container type
    process<std::optional<int>>();             // Optional type
    process<std::pair<int, double>>();         // Pair type
    process<int>();                            // Other type

    return 0;
}
```

### Bitmask

The `Bitmask` utility provides a type-safe, convenient way to work with flag enums. It uses a CRTP pattern so derived bitmask types preserve their concrete type when performing bitwise operations (so `a | b` returns the derived bitmask type, not the base class).

Key points:

- Uses `enum class` underlying values safely via `Type::ToUnderlying`.
- Operators `|`, `&`, `^`, and `~` return the derived bitmask type (CRTP).
- Provides helpers `Add`, `Remove`, `Has`, `Any`, `None`, and `Value()`.

#### Example

```cpp
#include <StormByte/bitmask.hxx>
#include <iostream>

using namespace StormByte;

enum class MyFlags : uint8_t {
    FlagA = 0x01,
    FlagB = 0x02,
    FlagC = 0x04
};

class MyBitmask: public Bitmask<MyBitmask, MyFlags> {
public:
    using Bitmask<MyBitmask, MyFlags>::Bitmask;
};

int main() {
    MyBitmask a(MyFlags::FlagA);
    MyBitmask b(MyFlags::FlagB);

    MyBitmask c = a | b; // returns MyBitmask

    if (c.Value() == (MyFlags::FlagA | MyFlags::FlagB)) {
        std::cout << "Flags A and B set\n";
    }

    c |= MyBitmask(MyFlags::FlagC);
    c.Remove(MyFlags::FlagB);

    std::cout << "Final value: " << static_cast<int>(Type::ToUnderlying(c.Value())) << std::endl;
    return 0;
}
```

## Contributing

Contributions are welcome! Please fork the repository and submit pull requests for any enhancements or bug fixes.

## License

This project is licensed under the LGPLv3 License - see the [LICENSE](LICENSE) file for details.
