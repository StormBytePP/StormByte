#include <StormByte/base64.hxx>
#include <StormByte/exception.hxx>
#include <StormByte/test_handlers.h>

#include <string>
#include <vector>
#include <cstring>

using namespace StormByte;

// Helper to create a byte vector from a string literal
static std::vector<std::byte> ToBytes(const std::string& str) {
	std::vector<std::byte> result;
	result.reserve(str.size());
	for (char c : str) {
		result.push_back(static_cast<std::byte>(c));
	}
	return result;
}

int test_base64_empty() {
	int result = 0;
	try {
		// Empty input
		std::string encoded = Base64Encode(std::vector<std::byte>{});
		ASSERT_EQUAL("test_base64_empty", "", encoded);

		std::vector<std::byte> decoded = Base64Decode("");
		ASSERT_EQUAL("test_base64_empty", 0u, decoded.size());
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_empty", result);
}

int test_base64_simple() {
	int result = 0;
	try {
		const std::string original = "Hello";
		auto bytes = ToBytes(original);

		std::string encoded = Base64Encode(bytes);
		ASSERT_EQUAL("test_base64_simple", "SGVsbG8=", encoded);

		auto decoded = Base64Decode(encoded);
		std::string recovered(reinterpret_cast<const char*>(decoded.data()), decoded.size());
		ASSERT_EQUAL("test_base64_simple", original, recovered);
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_simple", result);
}

int test_base64_padding_cases() {
	int result = 0;
	try {
		// 1 byte → two padding characters
		{
			auto bytes = ToBytes("A");
			std::string encoded = Base64Encode(bytes);
			ASSERT_EQUAL("test_base64_padding_cases", "QQ==", encoded);
			auto decoded = Base64Decode(encoded);
			ASSERT_EQUAL("test_base64_padding_cases", 1u, decoded.size());
			ASSERT_EQUAL("test_base64_padding_cases",
				static_cast<unsigned char>('A'),
				static_cast<unsigned char>(decoded[0]));
		}

		// 2 bytes → one padding character
		{
			auto bytes = ToBytes("AB");
			std::string encoded = Base64Encode(bytes);
			ASSERT_EQUAL("test_base64_padding_cases", "QUI=", encoded);
			auto decoded = Base64Decode(encoded);
			ASSERT_EQUAL("test_base64_padding_cases", 2u, decoded.size());
		}

		// 3 bytes → no padding
		{
			auto bytes = ToBytes("ABC");
			std::string encoded = Base64Encode(bytes);
			ASSERT_EQUAL("test_base64_padding_cases", "QUJD", encoded);
			auto decoded = Base64Decode(encoded);
			ASSERT_EQUAL("test_base64_padding_cases", 3u, decoded.size());
		}
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_padding_cases", result);
}

int test_base64_binary_data() {
	int result = 0;
	try {
		// Binary data with all possible byte values (0x00 - 0xFF)
		std::vector<std::byte> original(256);
		for (std::size_t i = 0; i < 256; ++i) {
			original[i] = static_cast<std::byte>(i);
		}

		std::string encoded = Base64Encode(original);
		auto decoded = Base64Decode(encoded);

		ASSERT_EQUAL("test_base64_binary_data", original.size(), decoded.size());
		ASSERT_TRUE("test_base64_binary_data", original == decoded);
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_binary_data", result);
}

int test_base64_span_overload() {
	int result = 0;
	try {
		const std::string original = "SpanTest";
		auto bytes = ToBytes(original);

		// Use the span overload
		std::string encoded = Base64Encode(std::span<const std::byte>(bytes));
		ASSERT_EQUAL("test_base64_span_overload", "U3BhblRlc3Q=", encoded);

		auto decoded = Base64Decode(encoded);
		std::string recovered(reinterpret_cast<const char*>(decoded.data()), decoded.size());
		ASSERT_EQUAL("test_base64_span_overload", original, recovered);
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_span_overload", result);
}

int test_base64_whitespace_in_input() {
	int result = 0;
	try {
		// Base64 with whitespace should still decode correctly
		std::string encoded_with_ws = "SGVs\nbG8=\t";
		auto decoded = Base64Decode(encoded_with_ws);

		std::string recovered(reinterpret_cast<const char*>(decoded.data()), decoded.size());
		ASSERT_EQUAL("test_base64_whitespace_in_input", "Hello", recovered);
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_whitespace_in_input", result);
}

int test_base64_invalid_character() {
	int result = 0;
	try {
		bool thrown = false;
		try {
			Base64Decode("SGVsbG8!"); // '!' is invalid
		} catch (const Base64Error&) {
			thrown = true;
		}
		ASSERT_TRUE("test_base64_invalid_character", thrown);
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_invalid_character", result);
}

int test_base64_long_string() {
	int result = 0;
	try {
		// Generate a longer string
		std::string original(1000, 'X');
		auto bytes = ToBytes(original);

		std::string encoded = Base64Encode(bytes);
		auto decoded = Base64Decode(encoded);

		std::string recovered(reinterpret_cast<const char*>(decoded.data()), decoded.size());
		ASSERT_EQUAL("test_base64_long_string", original, recovered);
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_long_string", result);
}

int test_base64_roundtrip_various_sizes() {
	int result = 0;
	try {
		for (std::size_t len = 0; len <= 16; ++len) {
			std::string original(len, static_cast<char>('A' + (len % 26)));
			auto bytes = ToBytes(original);

			std::string encoded = Base64Encode(bytes);
			auto decoded = Base64Decode(encoded);

			std::string recovered(reinterpret_cast<const char*>(decoded.data()), decoded.size());
			ASSERT_EQUAL("test_base64_roundtrip_various_sizes", original, recovered);
		}
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_base64_roundtrip_various_sizes", result);
}

int main() {
	int result = 0;
	try {
		result += test_base64_empty();
		result += test_base64_simple();
		result += test_base64_padding_cases();
		result += test_base64_binary_data();
		result += test_base64_span_overload();
		result += test_base64_whitespace_in_input();
		result += test_base64_invalid_character();
		result += test_base64_long_string();
		result += test_base64_roundtrip_various_sizes();
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}

	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}
