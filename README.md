# StormByte

StormByte is a comprehensive, cross-platform C++ library aimed at easing system programming, configuration management, logging, and database handling tasks. This library provides a unified API that abstracts away the complexities and inconsistencies of different platforms (Windows, Linux).

## Features

- **Configuration Management**: Provides an intuitive API for reading and writing configuration files.
- **Database Handling**: Includes SQLite support for embedded database management while hiding SQLite3 internals conveniently.
- **Buffers**: Provides a variety of buffer types for managing byte data in both single-threaded and multi-threaded environments. This includes lightweight non-thread-safe buffers, thread-safe shared buffers, and robust producer/consumer models that track buffer status.
- **Logging**: Offers various logging levels, customizable formats, and supports outputs to files, streams, or other destinations.
- **Multimedia**: Includes a set of classes to work with multimedia files.
- **Network**: Contains everything needed to handle network communication portable to Linux and Windows.
- **System Operations**: Manages pipes, processes, and system variables seamlessly across different platforms.
- **Serialization**: Provides a flexible serialization/deserialization framework that works with trivial types, standard containers, pairs, optionals, and can be extended for custom types through template specialization.
- **Cryptographic functions**: Cryptographic functions for encrypting, decrypting, hashing, signing and verify signatures.

## Table of Contents

- [Repository](#repository)
- [Installation](#installation)
- [Modules](#modules)
  - **Base**
    - [Serialization](#serialization)
    - [String Utilities](#string-utilities)
    - [System Utilities](#system-utilities)
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

- C++23 compatible compiler
- CMake 3.12 or higher

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

The Base component is the core of the library containing only templates, string helpers, and the base exception framework.


### Serialization

The `Serializable` template class provides a flexible and efficient way to serialize and deserialize data into byte vectors (`std::vector<std::byte>`). It automatically handles trivially copyable types, standard containers (like `std::vector`, `std::map`), pairs, optionals, and can be extended for custom types through template specialization.

#### Features
- **Automatic Type Detection**: Automatically selects the appropriate serialization method based on the type
- **Container Support**: Works seamlessly with STL containers
- **Pair and Optional Support**: Built-in support for `std::pair` and `std::optional`
- **Extensible**: Can be specialized for custom types
- **Type-Safe**: Uses `Expected<T, DeserializeError>` for safe deserialization with error handling

#### Built-in Type Support

The library automatically handles:
- **Trivially copyable types**: `int`, `float`, `double`, `char`, etc.
- **Containers**: `std::vector`, `std::list`, `std::map`, `std::set`, etc.
- **Pairs**: `std::pair<T1, T2>`
- **Optionals**: `std::optional<T>`
- **Strings**: `std::string` (as a container of characters)

#### Example Usage with Built-in Types

```cpp
#include <StormByte/serializable.hxx>
#include <iostream>
#include <string>
#include <vector>

using namespace StormByte;

int main() {
    // Serialize an integer
    int number = 42;
    Serializable<int> intSer(number);
    std::vector<std::byte> intData = intSer.Serialize();
    
    // Deserialize the integer
    auto deserializedInt = Serializable<int>::Deserialize(intData);
    if (deserializedInt) {
        std::cout << "Deserialized int: " << deserializedInt.value() << std::endl;
    }
    
    // Serialize a string
    std::string text = "Hello, World!";
    Serializable<std::string> strSer(text);
    std::vector<std::byte> strData = strSer.Serialize();
    
    // Deserialize the string
    auto deserializedStr = Serializable<std::string>::Deserialize(strData);
    if (deserializedStr) {
        std::cout << "Deserialized string: " << deserializedStr.value() << std::endl;
    }
    
    // Serialize a vector
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    Serializable<std::vector<int>> vecSer(numbers);
    std::vector<std::byte> vecData = vecSer.Serialize();
    
    // Deserialize the vector
    auto deserializedVec = Serializable<std::vector<int>>::Deserialize(vecData);
    if (deserializedVec) {
        std::cout << "Deserialized vector: ";
        for (int val : deserializedVec.value()) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
```

#### Extending Serialization for Custom Types

You can extend the serialization framework for your custom types by implementing the `SerializeComplex()` and `DeserializeComplex()` methods, or by providing template specializations. Here's an example:

```cpp
#include <StormByte/serializable.hxx>
#include <iostream>
#include <vector>

using namespace StormByte;

struct Point {
    int x;
    int y;
};

// Specialize the Serializable class for Point
namespace StormByte {
    template<>
    class Serializable<Point> {
    public:
        Serializable(const Point& data) : m_data(data) {}

        std::vector<std::byte> Serialize() const noexcept {
            std::vector<std::byte> buffer;
            
            // Serialize x coordinate
            Serializable<int> xSer(m_data.x);
            auto xData = xSer.Serialize();
            buffer.insert(buffer.end(), xData.begin(), xData.end());
            
            // Serialize y coordinate
            Serializable<int> ySer(m_data.y);
            auto yData = ySer.Serialize();
            buffer.insert(buffer.end(), yData.begin(), yData.end());
            
            return buffer;
        }

        static Expected<Point, DeserializeError> Deserialize(const std::vector<std::byte>& data) noexcept {
            std::size_t offset = 0;
            
            // Deserialize x coordinate
            if (offset + sizeof(int) > data.size())
                return Unexpected<DeserializeError>("Insufficient data for Point.x");
            
            std::vector<std::byte> xData(data.begin() + offset, data.begin() + offset + sizeof(int));
            auto x = Serializable<int>::Deserialize(xData);
            if (!x) return Unexpected(x.error());
            offset += sizeof(int);
            
            // Deserialize y coordinate
            if (offset + sizeof(int) > data.size())
                return Unexpected<DeserializeError>("Insufficient data for Point.y");
            
            std::vector<std::byte> yData(data.begin() + offset, data.begin() + offset + sizeof(int));
            auto y = Serializable<int>::Deserialize(yData);
            if (!y) return Unexpected(y.error());
            
            return Point{x.value(), y.value()};
        }

    private:
        const Point& m_data;
    };
}

int main() {
    Point p = {10, 20};
    Serializable<Point> serializer(p);

    // Serialize
    std::vector<std::byte> data = serializer.Serialize();
    std::cout << "Serialized Point (" << p.x << ", " << p.y << ")" << std::endl;

    // Deserialize
    auto deserialized = Serializable<Point>::Deserialize(data);
    if (deserialized) {
        Point deserializedPoint = deserialized.value();
        std::cout << "Deserialized Point: (" << deserializedPoint.x << ", " << deserializedPoint.y << ")" << std::endl;
    } else {
        std::cerr << "Deserialization failed: " << deserialized.error()->what() << std::endl;
    }

    return 0;
}
```

**Note**: By implementing template specializations in your library, you can extend serialization support to any custom type, making it seamlessly integrate with the built-in serialization framework.

### String Utilities

The `String` namespace provides a comprehensive set of utilities for string manipulation, conversion, and formatting. These functions help with common string operations while maintaining cross-platform compatibility.

#### Features
- **Case Conversion**: Convert strings to uppercase or lowercase
- **String Splitting**: Split strings by delimiters or whitespace
- **Fraction Parsing**: Parse and scale fraction strings
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

##### Fraction Parsing
```cpp
#include <StormByte/string.hxx>
#include <iostream>

using namespace StormByte::String;

int main() {
    // Parse a fraction string
    auto result = SplitFraction("3/4");
    if (result) {
        auto [numerator, denominator] = result.value();
        std::cout << "Fraction: " << numerator << "/" << denominator << std::endl;
    }
    
    // Parse and scale a fraction to a specific denominator
    auto scaled = SplitFraction("3/4", 16);
    if (scaled) {
        auto [num, denom] = scaled.value();
        std::cout << "Scaled: " << num << "/" << denom << std::endl;  // "12/16"
    }
    
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

### System Utilities

The `System` namespace provides cross-platform utilities for common system operations such as temporary file management, path operations, and timing functions.

#### Features
- **Temporary File Management**: Safely create temporary files with custom prefixes
- **Path Operations**: Get current working directory
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

## Contributing

Contributions are welcome! Please fork the repository and submit pull requests for any enhancements or bug fixes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
