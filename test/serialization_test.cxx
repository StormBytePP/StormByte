/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * or later, as published by the Free Software Foundation.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 */

//==============================================================================
// FILE: test/serialization_test.cxx
//==============================================================================
#include <StormByte/helpers.hxx>
#include <StormByte/serializable.hxx>
#include <StormByte/test_handlers.h>
#include <cstring>
#include <map>
#include <optional>
#include <random>
#include <span>
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
}
// =============================================================================
// Custom type via Detail::Codec (the supported extension point)
// =============================================================================
namespace {
	struct Tag {
		int a;
		std::string b;
		bool operator==(const Tag& other) const noexcept {
			return a == other.a && b == other.b;
		}
	};
}
template<>
struct StormByte::Detail::Codec<Tag> {
	static std::size_t Size(const Tag& v) noexcept {
		return Serializable<int>::Size(v.a) + Serializable<std::string>::Size(v.b);
	}
	static std::vector<std::byte> Write(const Tag& v) noexcept {
		std::vector<std::byte> buf;
		append_vector(buf, Serializable<int>(v.a).Serialize());
		append_vector(buf, Serializable<std::string>(v.b).Serialize());
		return buf;
	}
	static Expected<Tag, DeserializeError> Read(std::span<const std::byte> data) noexcept {
		auto expected_a = Serializable<int>::Deserialize(data);
		if (!expected_a)
			return Unexpected(expected_a.error());
		const std::size_t a_size = Serializable<int>::Size(expected_a.value());
		if (a_size > data.size())
			return Unexpected<DeserializeError>("Insufficient data for Tag::b");
		auto expected_b = Serializable<std::string>::Deserialize(data.subspan(a_size));
		if (!expected_b)
			return Unexpected(expected_b.error());
		return Tag{ expected_a.value(), expected_b.value() };
	}
};
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
	const std::string data(10 * 1024 * 1024, 'A');
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
// Wire contract: little-endian, no BOM
// =============================================================================
int test_wire_int_is_little_endian() {
	const int data = 0x01020304;
	auto buffer = Serializable<int>(data).Serialize();
	if (buffer.size() != sizeof(int)) {
		std::cerr << "test_wire_int_is_little_endian: unexpected size " << buffer.size() << "\n";
		RETURN_TEST("test_wire_int_is_little_endian", 1);
	}
	const unsigned char b0 = static_cast<unsigned char>(buffer[0]);
	const unsigned char b1 = static_cast<unsigned char>(buffer[1]);
	const unsigned char b2 = static_cast<unsigned char>(buffer[2]);
	const unsigned char b3 = static_cast<unsigned char>(buffer[3]);
	if (b0 != 0x04 || b1 != 0x03 || b2 != 0x02 || b3 != 0x01) {
		std::cerr << "test_wire_int_is_little_endian: got "
			<< static_cast<int>(b0) << " " << static_cast<int>(b1) << " "
			<< static_cast<int>(b2) << " " << static_cast<int>(b3) << "\n";
		RETURN_TEST("test_wire_int_is_little_endian", 1);
	}
	RETURN_TEST("test_wire_int_is_little_endian", 0);
}
int test_wire_string_length_is_uint64_le() {
	const std::string data = "AB";
	auto buffer = Serializable<std::string>(data).Serialize();
	if (buffer.size() != sizeof(std::uint64_t) + data.size()) {
		std::cerr << "test_wire_string_length_is_uint64_le: unexpected size\n";
		RETURN_TEST("test_wire_string_length_is_uint64_le", 1);
	}
	if (static_cast<unsigned char>(buffer[0]) != 2 ||
			static_cast<unsigned char>(buffer[1]) != 0 ||
			static_cast<unsigned char>(buffer[7]) != 0 ||
			static_cast<char>(buffer[8]) != 'A' ||
			static_cast<char>(buffer[9]) != 'B') {
		std::cerr << "test_wire_string_length_is_uint64_le: layout mismatch\n";
		RETURN_TEST("test_wire_string_length_is_uint64_le", 1);
	}
	RETURN_TEST("test_wire_string_length_is_uint64_le", 0);
}
// =============================================================================
// Corruption / robustness
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
	for (std::size_t i = 0; i < std::min<std::size_t>(16, clean.size()); ++i) {
		if (i + sizeof(std::uint64_t) > clean.size()) break;
		auto buf = clean;
		std::uint64_t huge = static_cast<std::uint64_t>(-1);
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
	if (clean.size() >= sizeof(std::uint64_t)) {
		auto buf = clean;
		std::uint64_t huge = static_cast<std::uint64_t>(-1);
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
			(void)result;
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
// std::u16string / std::wstring (UTF-8 on the wire)
// =============================================================================
int test_serialize_u16string() {
	std::u16string data = u"Hello, StormByte!";
	Serializable<std::u16string> serialization(data);
	std::vector<std::byte> buffer = serialization.Serialize();
	if (buffer.empty())
		RETURN_TEST("test_serialize_u16string", 1);
	auto expected = Serializable<std::u16string>::Deserialize(buffer);
	if (!expected) {
		std::cerr << expected.error()->what() << std::endl;
		RETURN_TEST("test_serialize_u16string", 1);
	}
	ASSERT_TRUE("test_serialize_u16string", data == expected.value());
	RETURN_TEST("test_serialize_u16string", 0);
}
int test_serialize_u16string_empty() {
	std::u16string data;
	auto buffer = Serializable<std::u16string>(data).Serialize();
	auto expected = Serializable<std::u16string>::Deserialize(buffer);
	if (!expected) {
		std::cerr << expected.error()->what() << std::endl;
		RETURN_TEST("test_serialize_u16string_empty", 1);
	}
	ASSERT_TRUE("test_serialize_u16string_empty", expected.value().empty());
	RETURN_TEST("test_serialize_u16string_empty", 0);
}
int test_serialize_u16string_truncated() {
	std::u16string data = u"TruncationTest";
	auto buffer = Serializable<std::u16string>(data).Serialize();
	for (std::size_t len = 0; len < buffer.size(); ++len) {
		auto truncated = std::vector<std::byte>(buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(len));
		auto result = Serializable<std::u16string>::Deserialize(truncated);
		if (result) {
			std::cerr << "test_serialize_u16string_truncated: size " << len << " accepted\n";
			RETURN_TEST("test_serialize_u16string_truncated", 1);
		}
	}
	RETURN_TEST("test_serialize_u16string_truncated", 0);
}
int test_serialize_u16string_huge_size() {
	auto clean = Serializable<std::u16string>(u"safe").Serialize();
	if (clean.size() < sizeof(std::uint64_t))
		RETURN_TEST("test_serialize_u16string_huge_size", 1);
	auto buf = clean;
	std::uint64_t huge = static_cast<std::uint64_t>(-1);
	std::memcpy(buf.data(), &huge, sizeof(huge));
	auto result = Serializable<std::u16string>::Deserialize(buf);
	if (result) {
		std::cerr << "test_serialize_u16string_huge_size: huge size was accepted\n";
		RETURN_TEST("test_serialize_u16string_huge_size", 1);
	}
	RETURN_TEST("test_serialize_u16string_huge_size", 0);
}
int test_serialize_u16string_non_bmp() {
	// U+1F4A9 PILE OF POO — needs a surrogate pair in UTF-16.
	std::u16string data = u"\U0001F4A9";
	auto buffer = Serializable<std::u16string>(data).Serialize();
	auto expected = Serializable<std::u16string>::Deserialize(buffer);
	if (!expected) {
		std::cerr << expected.error()->what() << std::endl;
		RETURN_TEST("test_serialize_u16string_non_bmp", 1);
	}
	ASSERT_TRUE("test_serialize_u16string_non_bmp", data == expected.value());
	RETURN_TEST("test_serialize_u16string_non_bmp", 0);
}
int test_serialize_wstring() {
	std::wstring data = L"Hello, StormByte!";
	auto buffer = Serializable<std::wstring>(data).Serialize();
	if (buffer.empty())
		RETURN_TEST("test_serialize_wstring", 1);
	auto expected = Serializable<std::wstring>::Deserialize(buffer);
	if (!expected) {
		std::cerr << expected.error()->what() << std::endl;
		RETURN_TEST("test_serialize_wstring", 1);
	}
	ASSERT_TRUE("test_serialize_wstring", data == expected.value());
	RETURN_TEST("test_serialize_wstring", 0);
}
int test_serialize_wstring_non_bmp() {
	std::wstring data = L"\U0001F4A9";
	auto buffer = Serializable<std::wstring>(data).Serialize();
	auto expected = Serializable<std::wstring>::Deserialize(buffer);
	if (!expected) {
		std::cerr << expected.error()->what() << std::endl;
		RETURN_TEST("test_serialize_wstring_non_bmp", 1);
	}
	ASSERT_TRUE("test_serialize_wstring_non_bmp", data == expected.value());
	RETURN_TEST("test_serialize_wstring_non_bmp", 0);
}
int test_wide_and_u16_share_utf8_wire() {
	const std::wstring wide = L"StormByte";
	const std::u16string u16 = u"StormByte";
	auto a = Serializable<std::wstring>(wide).Serialize();
	auto b = Serializable<std::u16string>(u16).Serialize();
	ASSERT_TRUE("test_wide_and_u16_share_utf8_wire", a == b);
	RETURN_TEST("test_wide_and_u16_share_utf8_wire", 0);
}
// =============================================================================
// Extra monster tests
// =============================================================================
int test_base_cross_type_vector_as_string() {
	auto vec_buf = MakeStringVectorBuffer();
	auto as_string = Serializable<std::string>::Deserialize(vec_buf);
	(void)as_string;
	RETURN_TEST("test_base_cross_type_vector_as_string", 0);
}
int test_base_idempotent_roundtrip_vector() {
	std::vector<std::string> original = {"a", "b", "StormByte"};
	auto buf1 = Serializable<std::vector<std::string>>(original).Serialize();
	auto d1 = Serializable<std::vector<std::string>>::Deserialize(buf1);
	if (!d1) {
		std::cerr << d1.error()->what() << std::endl;
		RETURN_TEST("test_base_idempotent_roundtrip_vector", 1);
	}
	auto buf2 = Serializable<std::vector<std::string>>(d1.value()).Serialize();
	ASSERT_TRUE("test_base_idempotent_roundtrip_vector", original == d1.value());
	ASSERT_TRUE("test_base_idempotent_roundtrip_vector", buf1 == buf2);
	RETURN_TEST("test_base_idempotent_roundtrip_vector", 0);
}
int test_base_idempotent_roundtrip_pair() {
	std::pair<int, std::string> original{42, "answer"};
	auto buf1 = Serializable<std::pair<int, std::string>>(original).Serialize();
	auto d1 = Serializable<std::pair<int, std::string>>::Deserialize(buf1);
	if (!d1) {
		std::cerr << d1.error()->what() << std::endl;
		RETURN_TEST("test_base_idempotent_roundtrip_pair", 1);
	}
	auto buf2 = Serializable<std::pair<int, std::string>>(d1.value()).Serialize();
	ASSERT_TRUE("test_base_idempotent_roundtrip_pair", original == d1.value());
	ASSERT_TRUE("test_base_idempotent_roundtrip_pair", buf1 == buf2);
	RETURN_TEST("test_base_idempotent_roundtrip_pair", 0);
}
int test_base_trailing_garbage() {
	auto clean = MakeStringBuffer();
	auto dirty = clean;
	dirty.push_back(std::byte{0xDE});
	dirty.push_back(std::byte{0xAD});
	auto result = Serializable<std::string>::Deserialize(dirty);
	if (!result) {
		std::cerr << "test_base_trailing_garbage: trailing bytes should be ignored by Read\n";
		RETURN_TEST("test_base_trailing_garbage", 1);
	}
	ASSERT_EQUAL("test_base_trailing_garbage", std::string("StormByte serialization test"), result.value());
	RETURN_TEST("test_base_trailing_garbage", 0);
}
int test_base_double_corruption() {
	auto clean = MakeStringVectorBuffer();
	if (clean.size() < 8)
		RETURN_TEST("test_base_double_corruption", 0);
	auto buf = clean;
	for (std::size_t i = 0; i < 4; ++i)
		buf[i] = std::byte{0xFF};
	for (std::size_t i = 0; i < 4; ++i)
		buf[buf.size() - 1 - i] = std::byte{0xAA};
	auto result = Serializable<std::vector<std::string>>::Deserialize(buf);
	(void)result;
	RETURN_TEST("test_base_double_corruption", 0);
}
int test_base_nested_vector_of_pairs() {
	std::vector<std::pair<int, std::string>> data = {
		{1, "one"}, {2, "two"}, {3, "three"}
	};
	auto buf = Serializable<std::vector<std::pair<int, std::string>>>(data).Serialize();
	auto expected = Serializable<std::vector<std::pair<int, std::string>>>::Deserialize(buf);
	if (!expected) {
		std::cerr << expected.error()->what() << std::endl;
		RETURN_TEST("test_base_nested_vector_of_pairs", 1);
	}
	ASSERT_TRUE("test_base_nested_vector_of_pairs", data == expected.value());
	RETURN_TEST("test_base_nested_vector_of_pairs", 0);
}
int test_base_bool_rejects_invalid_byte() {
	std::vector<std::byte> buf = { std::byte{17} };
	bool threw = false;
	bool accepted = false;
	try {
		auto result = Serializable<bool>::Deserialize(buf);
		if (result)
			accepted = true;
	} catch (...) {
		threw = true;
	}
	if (threw) {
		std::cerr << "test_base_bool_rejects_invalid_byte: threw instead of returning error\n";
		RETURN_TEST("test_base_bool_rejects_invalid_byte", 1);
	}
	if (accepted) {
		std::cerr << "test_base_bool_rejects_invalid_byte: invalid byte 17 was accepted\n";
		RETURN_TEST("test_base_bool_rejects_invalid_byte", 1);
	}
	RETURN_TEST("test_base_bool_rejects_invalid_byte", 0);
}
int test_base_bool_accepts_0_and_1() {
	auto z = Serializable<bool>::Deserialize(std::vector<std::byte>{ std::byte{0} });
	auto o = Serializable<bool>::Deserialize(std::vector<std::byte>{ std::byte{1} });
	if (!z || z.value() != false) {
		std::cerr << "test_base_bool_accepts_0_and_1: 0 not decoded as false\n";
		RETURN_TEST("test_base_bool_accepts_0_and_1", 1);
	}
	if (!o || o.value() != true) {
		std::cerr << "test_base_bool_accepts_0_and_1: 1 not decoded as true\n";
		RETURN_TEST("test_base_bool_accepts_0_and_1", 1);
	}
	RETURN_TEST("test_base_bool_accepts_0_and_1", 0);
}
int test_base_optional_flag_rejects_invalid_bool() {
	std::vector<std::byte> buf = { std::byte{17} };
	bool threw = false;
	bool accepted = false;
	try {
		auto result = Serializable<std::optional<int>>::Deserialize(buf);
		if (result)
			accepted = true;
	} catch (...) {
		threw = true;
	}
	if (threw || accepted) {
		std::cerr << "test_base_optional_flag_rejects_invalid_bool: invalid flag not rejected cleanly\n";
		RETURN_TEST("test_base_optional_flag_rejects_invalid_bool", 1);
	}
	RETURN_TEST("test_base_optional_flag_rejects_invalid_bool", 0);
}
int test_codec_custom_type() {
	const Tag original{ 7, "codec" };
	auto buffer = Serializable<Tag>(original).Serialize();
	if (buffer.empty())
		RETURN_TEST("test_codec_custom_type", 1);
	auto expected = Serializable<Tag>::Deserialize(buffer);
	if (!expected) {
		std::cerr << expected.error()->what() << std::endl;
		RETURN_TEST("test_codec_custom_type", 1);
	}
	ASSERT_TRUE("test_codec_custom_type", original == expected.value());
	ASSERT_EQUAL("test_codec_custom_type", Serializable<Tag>::Size(original), buffer.size());
	RETURN_TEST("test_codec_custom_type", 0);
}
int test_codec_custom_type_truncated() {
	const Tag original{ 7, "codec" };
	auto buffer = Serializable<Tag>(original).Serialize();
	for (std::size_t len = 0; len < buffer.size(); ++len) {
		auto truncated = Truncate(buffer, len);
		auto result = Serializable<Tag>::Deserialize(truncated);
		if (result) {
			std::cerr << "test_codec_custom_type_truncated: size " << len << " accepted\n";
			RETURN_TEST("test_codec_custom_type_truncated", 1);
		}
	}
	RETURN_TEST("test_codec_custom_type_truncated", 0);
}
// =============================================================================
// main
// =============================================================================
int main() {
	int result = 0;
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
	result += test_wire_int_is_little_endian();
	result += test_wire_string_length_is_uint64_le();
	result += test_base_corruption_empty_buffer();
	result += test_base_corruption_string_truncated_all();
	result += test_base_corruption_vector_truncated_all();
	result += test_base_corruption_huge_container_size();
	result += test_base_corruption_huge_string_size();
	result += test_base_corruption_no_crash_bit_flip();
	result += test_base_corruption_no_crash_byte_overwrite();
	result += test_base_corruption_random_stress();
	result += test_serialize_u16string();
	result += test_serialize_u16string_empty();
	result += test_serialize_u16string_truncated();
	result += test_serialize_u16string_huge_size();
	result += test_serialize_u16string_non_bmp();
	result += test_serialize_wstring();
	result += test_serialize_wstring_non_bmp();
	result += test_wide_and_u16_share_utf8_wire();
	result += test_base_cross_type_vector_as_string();
	result += test_base_idempotent_roundtrip_vector();
	result += test_base_idempotent_roundtrip_pair();
	result += test_base_trailing_garbage();
	result += test_base_double_corruption();
	result += test_base_nested_vector_of_pairs();
	result += test_base_bool_rejects_invalid_byte();
	result += test_base_bool_accepts_0_and_1();
	result += test_base_optional_flag_rejects_invalid_bool();
	result += test_codec_custom_type();
	result += test_codec_custom_type_truncated();
	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}
