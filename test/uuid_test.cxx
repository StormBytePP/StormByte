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

#include <StormByte/test_handlers.h>
#include <StormByte/uuid.hxx>

#include <set>
#include <string>

using namespace StormByte;

namespace {
	bool IsHex(char c) {
		return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
	}

	bool IsVariant(char c) {
		return c == '8' || c == '9' || c == 'a' || c == 'b';
	}
}

// -------------------
// Generate
// -------------------

int test_generate_not_null() {
	int result = 0;
	const auto uuid = GenerateUUIDv4();
	ASSERT_TRUE("test_generate_not_null", static_cast<bool>(uuid));
	ASSERT_EQUAL("test_generate_not_null", 36u, uuid.Length());
	RETURN_TEST("test_generate_not_null", result);
}

int test_generate_implicit_string() {
	int result = 0;
	const std::string text = GenerateUUIDv4();
	ASSERT_EQUAL("test_generate_implicit_string", 36u, text.size());
	RETURN_TEST("test_generate_implicit_string", result);
}

// -------------------
// Format
// -------------------

int test_format_hyphens() {
	int result = 0;
	const auto uuid = GenerateUUIDv4();
	ASSERT_EQUAL("test_format_hyphens", '-', uuid[8]);
	ASSERT_EQUAL("test_format_hyphens", '-', uuid[13]);
	ASSERT_EQUAL("test_format_hyphens", '-', uuid[18]);
	ASSERT_EQUAL("test_format_hyphens", '-', uuid[23]);
	RETURN_TEST("test_format_hyphens", result);
}

int test_format_version_is_4() {
	int result = 0;
	const auto uuid = GenerateUUIDv4();
	ASSERT_EQUAL("test_format_version_is_4", '4', uuid[14]);
	RETURN_TEST("test_format_version_is_4", result);
}

int test_format_variant_rfc4122() {
	int result = 0;
	const auto uuid = GenerateUUIDv4();
	ASSERT_TRUE("test_format_variant_rfc4122", IsVariant(uuid[19]));
	RETURN_TEST("test_format_variant_rfc4122", result);
}

int test_format_hex_lowercase() {
	int result = 0;
	const auto uuid = GenerateUUIDv4();
	for (std::size_t i = 0; i < uuid.Length(); ++i) {
		if (i == 8 || i == 13 || i == 18 || i == 23)
			continue;
		ASSERT_TRUE("test_format_hex_lowercase", IsHex(uuid[i]));
		ASSERT_TRUE("test_format_hex_lowercase", uuid[i] < 'A' || uuid[i] > 'F');
	}
	RETURN_TEST("test_format_hex_lowercase", result);
}

int test_format_rejects_wrong_length_shape() {
	int result = 0;
	const auto uuid = GenerateUUIDv4();
	ASSERT_FALSE("test_format_rejects_wrong_length_shape", uuid.Length() != 36);
	ASSERT_FALSE("test_format_rejects_wrong_length_shape", uuid[8] != '-');
	ASSERT_FALSE("test_format_rejects_wrong_length_shape", uuid[14] != '4');
	ASSERT_FALSE("test_format_rejects_wrong_length_shape", !IsVariant(uuid[19]));
	RETURN_TEST("test_format_rejects_wrong_length_shape", result);
}

// -------------------
// Uniqueness
// -------------------

int test_generate_distinct() {
	int result = 0;
	std::set<std::string> seen;
	for (int i = 0; i < 1000; ++i) {
		const auto uuid = GenerateUUIDv4();
		ASSERT_TRUE("test_generate_distinct", !seen.contains(uuid));
		seen.insert(uuid);
	}
	ASSERT_EQUAL("test_generate_distinct", 1000u, seen.size());
	RETURN_TEST("test_generate_distinct", result);
}

int main() {
	int result = 0;

	// -------------------
	// Generate
	// -------------------
	result += test_generate_not_null();
	result += test_generate_implicit_string();

	// -------------------
	// Format
	// -------------------
	result += test_format_hyphens();
	result += test_format_version_is_4();
	result += test_format_variant_rfc4122();
	result += test_format_hex_lowercase();
	result += test_format_rejects_wrong_length_shape();

	// -------------------
	// Uniqueness
	// -------------------
	result += test_generate_distinct();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
