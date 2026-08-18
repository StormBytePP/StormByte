//==============================================================================
// FILE: test/serialization_test.cxx
//==============================================================================

#include <StormByte/serializable.hxx>
#include <StormByte/test_handlers.h>

#include <cstring>
#include <format>
#include <map>
#include <optional>
#include <random>
#include <string>
#include <vector>

using namespace StormByte;

// =============================================================================
// Helpers
// =============================================================================

namespace {

void CorruptByte(std::vector<std::byte>& buf, std::size_t index, std::byte value) {
	if (index < buf.size())
		buf[index] = value;
}

void FlipBit(std::vector<std::byte>& buf, std::size_t byte_index, unsigned bit) {
	if (byte_index >= buf.size() || bit > 7) return;
	auto& b = reinterpret_cast<unsigned char&>(buf[byte_index]);
	b ^= static_cast<unsigned char>(1u << bit);
}

std::vector<std::byte> Truncate(const std::vector<std::byte>& buf, std::size_t new_size) {
	if (new_size >= buf.size()) return buf;
	return {buf.begin(), buf.begin() + static_cast<std::ptrdiff_t>(new_size)};
}

std::vector<std::byte> MakeStringVectorBuffer() {
	std::vector<std::string> data = {"Hello", "StormByte", "World"};
	return Serializable<std::vector<std::string>>(data).Serialize();
}

std::vector<std::byte> MakeStringBuffer() {
	return Serializable<std::string>("StormByte serialization test").Serialize();
}

} // namespace

// =============================================================================
// Original tests
// =============================================================================

int test_serialize_int() {
	int data = 42;
	Serializable<int> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_int", 1);

	auto expected_data = Serializable<int>::Deserialize(buffer);
	if (!expected_data)
		RETURN_TEST("test_serialize_int", 1);
	
	ASSERT_EQUAL("test_serialize_int", data, expected_data.value());
	RETURN_TEST("test_serialize_int", 0);
}

int test_serialize_double() {
	double data = 777.777;
	Serializable<double> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_double", 1);
	
	auto expected_data = Serializable<double>::Deserialize(buffer);
	if (!expected_data)
		RETURN_TEST("test_serialize_double", 1);
	
	ASSERT_EQUAL("test_serialize_double", data, expected_data.value());
	RETURN_TEST("test_serialize_double", 0);
}

int test_serialize_string() {
	std::string data = "Hello, World!";
	Serializable<std::string> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_string", 1);
	
	auto expected_data = Serializable<std::string>::Deserialize(buffer);
	if (!expected_data)
		RETURN_TEST("test_serialize_string", 1);
	
	ASSERT_EQUAL("test_serialize_string", data, expected_data.value());
	RETURN_TEST("test_serialize_string", 0);
}

int test_serialize_size_t() {
	std::string data = "Hello, World!";
	std::size_t size = data.size();
	Serializable<std::size_t> serialization(size);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_size_t", 1);
	
	auto expected_data = Serializable<std::size_t>::Deserialize(buffer);
	if (!expected_data)
		RETURN_TEST("test_serialize_size_t", 1);

	ASSERT_EQUAL("test_serialize_size_t", data.size(), expected_data.value());
	RETURN_TEST("test_serialize_size_t", 0);
}

int test_serialize_string_vector() {
	std::vector<std::string> data = { "Hello", "World!" };
	Serializable<std::vector<std::string>> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_string_vector", 1);
	
	auto expected_data = Serializable<std::vector<std::string>>::Deserialize(buffer);
	if (!expected_data) {
		std::cerr << expected_data.error()->what() << std::endl;
		RETURN_TEST("test_serialize_string_vector", 1);
	}

	ASSERT_TRUE("test_serialize_string_vector", data == expected_data.value());
	RETURN_TEST("test_serialize_string_vector", 0);
}

int test_serialize_pair() {
	std::pair<int, double> data = { 42, 777.777 };
	Serializable<std::pair<int, double>> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_pair", 1);
	
	auto expected_data = Serializable<std::pair<int, double>>::Deserialize(buffer);
	if (!expected_data) {
		std::cerr << expected_data.error()->what() << std::endl;
		RETURN_TEST("test_serialize_pair", 1);
	}

	ASSERT_TRUE("test_serialize_pair", data == expected_data.value());
	RETURN_TEST("test_serialize_pair", 0);
}

int test_serialize_map() {
	std::map<int, std::string> data = { { 1, "Hello" }, { 2, "World!" } };
	Serializable<std::map<int, std::string>> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_map", 1);
	
	auto expected_data = Serializable<std::map<int, std::string>>::Deserialize(buffer);
	if (!expected_data) {
		std::cerr << expected_data.error()->what() << std::endl;
		RETURN_TEST("test_serialize_map", 1);
	}

	ASSERT_TRUE("test_serialize_map", data == expected_data.value());
	RETURN_TEST("test_serialize_map", 0);
}

int test_serialize_int_truncated() {
	int data = 42;
	Serializable<int> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_int_truncated", 1);

	std::vector<std::byte> truncated_buffer(buffer.begin(), buffer.begin() + sizeof(int) / 2);
	auto expected_data = Serializable<int>::Deserialize(truncated_buffer);
	if (expected_data) {
		std::cerr << "Expected failure, but got value: " << expected_data.value() << std::endl;
		RETURN_TEST("test_serialize_int_truncated", 1);
	}

	RETURN_TEST("test_serialize_int_truncated", 0);
}

int test_serialize_string_vector_truncated() {
	std::vector<std::string> data = { "Hello", "World!" };
	Serializable<std::vector<std::string>> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_string_vector_truncated", 1);

	std::size_t truncated_length = sizeof(std::size_t) + 2;
	std::vector<std::byte> truncated_buffer(buffer.begin(), buffer.begin() + truncated_length);
	auto expected_data = Serializable<std::vector<std::string>>::Deserialize(truncated_buffer);
	if (expected_data) {
		std::cerr << "Expected failure, but got value" << std::endl;
		RETURN_TEST("test_serialize_string_vector_truncated", 1);
	}

	RETURN_TEST("test_serialize_string_vector_truncated", 0);
}

int test_serialize_pair_truncated() {
	std::pair<int, double> data = { 42, 777.777 };
	Serializable<std::pair<int, double>> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_pair_truncated", 1);

	std::size_t truncated_length = sizeof(int);
	std::vector<std::byte> truncated_buffer(buffer.begin(), buffer.begin() + truncated_length);
	auto expected_data = Serializable<std::pair<int, double>>::Deserialize(truncated_buffer);
	if (expected_data) {
		std::cerr << "Expected failure, but got value" << std::endl;
		RETURN_TEST("test_serialize_pair_truncated", 1);
	}

	RETURN_TEST("test_serialize_pair_truncated", 0);
}

int test_serialize_optional_notempty() {
	std::optional<int> data = 42;
	Serializable<std::optional<int>> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_optional_notempty", 1);

	auto expected_data = Serializable<std::optional<int>>::Deserialize(buffer);
	if (!expected_data) {
		std::cerr << expected_data.error()->what() << std::endl;
		RETURN_TEST("test_serialize_optional_notempty", 1);
	}

	ASSERT_EQUAL("test_serialize_optional_notempty", data.value(), expected_data.value().value());
	RETURN_TEST("test_serialize_optional_notempty", 0);
}

int test_serialize_optional_empty() {
	std::optional<int> data;
	Serializable<std::optional<int>> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_optional_empty", 1);

	auto expected_data = Serializable<std::optional<int>>::Deserialize(buffer);
	if (!expected_data) {
		std::cerr << expected_data.error()->what() << std::endl;
		RETURN_TEST("test_serialize_optional_empty", 1);
	}

	ASSERT_FALSE("test_serialize_optional_empty", expected_data.value().has_value());
	RETURN_TEST("test_serialize_optional_empty", 0);
}

int test_serialize_optional_string() {
	std::optional<std::string> data = "Hello, World!";
	Serializable<std::optional<std::string>> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.size() == 0)
		RETURN_TEST("test_serialize_optional_string", 1);

	auto expected_data = Serializable<std::optional<std::string>>::Deserialize(buffer);
	if (!expected_data) {
		std::cerr << expected_data.error()->what() << std::endl;
		RETURN_TEST("test_serialize_optional_string", 1);
	}

	ASSERT_EQUAL("test_serialize_optional_string", data.value(), expected_data.value().value());
	RETURN_TEST("test_serialize_optional_string", 0);
}

int test_serialize_deserialize_big_string() {
	const std::string fn_name = "test_serialize_deserialize_big_string";
	const std::string data(10 * 1024 * 1024, 'A'); // 10MB string
	Serializable<std::string> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	ASSERT_FALSE(fn_name, buffer.empty());
	auto expected_data = Serializable<std::string>::Deserialize(buffer);
	if (!expected_data) {
		std::cerr << expected_data.error()->what() << std::endl;
		RETURN_TEST(fn_name.c_str(), 1);
	}

	ASSERT_EQUAL(fn_name, data, expected_data.value());
	RETURN_TEST(fn_name.c_str(), 0);
}

int test_serialize_deserialize_with_span() {
	const std::string fn_name = "test_serialize_deserialize_with_span";
	int data = 123456;
	Serializable<int> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.empty())
		RETURN_TEST(fn_name.c_str(), 1);

	auto expected_data = Serializable<int>::Deserialize(std::span<const std::byte>(buffer.data(), buffer.size()));
	if (!expected_data) {
		std::cerr << expected_data.error()->what() << std::endl;
		RETURN_TEST(fn_name.c_str(), 1);
	}

	ASSERT_EQUAL(fn_name.c_str(), data, expected_data.value());
	RETURN_TEST(fn_name.c_str(), 0);
}

int test_serialize_deserialize_with_span_truncated() {
	const std::string fn_name = "test_serialize_deserialize_with_span_truncated";
	int data = 123456;
	Serializable<int> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.empty())
		RETURN_TEST(fn_name.c_str(), 1);

	std::size_t truncated_len = sizeof(int) / 2;
	std::span<const std::byte> truncated_span(buffer.data(), truncated_len);
	auto expected_data = Serializable<int>::Deserialize(truncated_span);
	if (expected_data) {
		std::cerr << "Expected failure, but got value: " << expected_data.value() << std::endl;
		RETURN_TEST(fn_name.c_str(), 1);
	}

	RETURN_TEST(fn_name.c_str(), 0);
}

// =============================================================================
// NEW: Corruption / robustness tests (base library)
// =============================================================================

int test_base_corruption_empty_buffer() {
	auto r1 = Serializable<int>::Deserialize(std::vector<std::byte>{});
	auto r2 = Serializable<std::string>::Deserialize(std::vector<std::byte>{});
	auto r3 = Serializable<std::vector<std::string>>::Deserialize(std::vector<std::byte>{});
	auto r4 = Serializable<std::pair<int, double>>::Deserialize(std::vector<std::byte>{});

	if (r1 || r2 || r3 || r4) {
		std::cerr << "test_base_corruption_empty_buffer: empty buffer was accepted\n";
		RETURN_TEST("test_base_corruption_empty_buffer", 1);
	}
	RETURN_TEST("test_base_corruption_empty_buffer", 0);
}

int test_base_corruption_string_truncated_all() {
	auto clean = MakeStringBuffer();
	for (std::size_t len = 0; len < clean.size(); ++len) {
		auto truncated = Truncate(clean, len);
		auto result = Serializable<std::string>::Deserialize(truncated);
		if (result) {
			std::cerr << "test_base_corruption_string_truncated_all: truncated size "
					<< len << " was accepted\n";
			RETURN_TEST("test_base_corruption_string_truncated_all", 1);
		}
	}
	RETURN_TEST("test_base_corruption_string_truncated_all", 0);
}

int test_base_corruption_vector_truncated_all() {
	auto clean = MakeStringVectorBuffer();
	for (std::size_t len = 0; len < clean.size(); ++len) {
		auto truncated = Truncate(clean, len);
		auto result = Serializable<std::vector<std::string>>::Deserialize(truncated);
		if (result) {
			std::cerr << "test_base_corruption_vector_truncated_all: truncated size "
					<< len << " was accepted\n";
			RETURN_TEST("test_base_corruption_vector_truncated_all", 1);
		}
	}
	RETURN_TEST("test_base_corruption_vector_truncated_all", 0);
}

int test_base_corruption_huge_container_size() {
	auto clean = MakeStringVectorBuffer();
	int accepted = 0;

	// Overwrite early bytes with a huge size_t / uint64
	for (std::size_t i = 0; i < std::min<std::size_t>(16, clean.size()); ++i) {
		if (i + sizeof(std::size_t) > clean.size()) break;
		auto buf = clean;
		std::size_t huge = static_cast<std::size_t>(-1);
		std::memcpy(buf.data() + i, &huge, sizeof(huge));
		auto result = Serializable<std::vector<std::string>>::Deserialize(buf);
		if (result)
			++accepted;
	}

	if (accepted > 0) {
		std::cerr << "test_base_corruption_huge_container_size: " << accepted
				<< " buffers with huge size were accepted\n";
		RETURN_TEST("test_base_corruption_huge_container_size", 1);
	}
	RETURN_TEST("test_base_corruption_huge_container_size", 0);
}

int test_base_corruption_huge_string_size() {
	auto clean = MakeStringBuffer();
	int accepted = 0;

	// Only corrupt the full size field at the start
	if (clean.size() >= sizeof(std::size_t)) {
		auto buf = clean;
		std::size_t huge = static_cast<std::size_t>(-1);
		std::memcpy(buf.data(), &huge, sizeof(huge));
		auto result = Serializable<std::string>::Deserialize(buf);
		if (result)
			++accepted;
	}

	if (accepted > 0) {
		std::cerr << "test_base_corruption_huge_string_size: huge size field was accepted\n";
		RETURN_TEST("test_base_corruption_huge_string_size", 1);
	}
	RETURN_TEST("test_base_corruption_huge_string_size", 0);
}

int test_base_corruption_no_crash_bit_flip() {
	auto clean = MakeStringVectorBuffer();
	for (std::size_t i = 0; i < clean.size(); ++i) {
		for (unsigned bit = 0; bit < 8; ++bit) {
			auto buf = clean;
			FlipBit(buf, i, bit);
			auto result = Serializable<std::vector<std::string>>::Deserialize(buf);
			(void)result; // must not crash / terminate
		}
	}
	RETURN_TEST("test_base_corruption_no_crash_bit_flip", 0);
}

int test_base_corruption_no_crash_byte_overwrite() {
	auto clean = MakeStringBuffer();
	for (std::size_t i = 0; i < clean.size(); ++i) {
		for (int v = 0; v < 256; v += 31) {
			auto buf = clean;
			CorruptByte(buf, i, static_cast<std::byte>(v));
			auto result = Serializable<std::string>::Deserialize(buf);
			(void)result;
		}
	}
	RETURN_TEST("test_base_corruption_no_crash_byte_overwrite", 0);
}

int test_base_corruption_random_stress() {
	auto clean = MakeStringVectorBuffer();
	std::mt19937 rng(0xC0FFEE);
	std::uniform_int_distribution<std::size_t> pos_dist(0, clean.size() - 1);
	std::uniform_int_distribution<int> val_dist(0, 255);

	constexpr int ITERATIONS = 400;
	for (int i = 0; i < ITERATIONS; ++i) {
		auto buf = clean;
		int count = 1 + (i % 4);
		for (int c = 0; c < count; ++c)
			CorruptByte(buf, pos_dist(rng), static_cast<std::byte>(val_dist(rng)));
		auto result = Serializable<std::vector<std::string>>::Deserialize(buf);
		(void)result;
	}
	RETURN_TEST("test_base_corruption_random_stress", 0);
}

// =============================================================================
// main
// =============================================================================

int main() {
	int result = 0;

	// Original
	result += test_serialize_int();
	result += test_serialize_double();
	result += test_serialize_string();
	result += test_serialize_size_t();
	result += test_serialize_string_vector();
	result += test_serialize_pair();
	result += test_serialize_map();
	result += test_serialize_int_truncated();
	result += test_serialize_string_vector_truncated();
	result += test_serialize_pair_truncated();
	result += test_serialize_optional_notempty();
	result += test_serialize_optional_empty();
	result += test_serialize_optional_string();
	result += test_serialize_deserialize_big_string();
	result += test_serialize_deserialize_with_span();
	result += test_serialize_deserialize_with_span_truncated();

	// New robustness
	result += test_base_corruption_empty_buffer();
	result += test_base_corruption_string_truncated_all();
	result += test_base_corruption_vector_truncated_all();
	result += test_base_corruption_huge_container_size();
	result += test_base_corruption_huge_string_size();
	result += test_base_corruption_no_crash_bit_flip();
	result += test_base_corruption_no_crash_byte_overwrite();
	result += test_base_corruption_random_stress();

	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}