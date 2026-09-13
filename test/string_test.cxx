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

#include <StormByte/string.hxx>
#include <StormByte/system.hxx>
#include <StormByte/test_handlers.h>
#include <clocale>
#include <string>
using namespace StormByte::String;
int test_simple_explode() {
	int result = 0;
	try {
		std::string str = "Hello, World!";
		std::queue<std::string> parts = Explode(str, ',');
		ASSERT_EQUAL("test_simple_explode", 2, parts.size());
		ASSERT_EQUAL("test_simple_explode", "Hello", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_simple_explode", " World!", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_simple_explode", true, parts.empty());
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_simple_explode", result);
}
int test_explode_consecutive_delimiters() {
	int result = 0;
	std::queue<std::string> parts = Explode("a,,c", ',');
	ASSERT_EQUAL("test_explode_consecutive_delimiters", 3u, parts.size());
	ASSERT_EQUAL("test_explode_consecutive_delimiters", "a", parts.front());
	parts.pop();
	ASSERT_EQUAL("test_explode_consecutive_delimiters", "", parts.front());
	parts.pop();
	ASSERT_EQUAL("test_explode_consecutive_delimiters", "c", parts.front());
	RETURN_TEST("test_explode_consecutive_delimiters", result);
}
int test_path_explode() {
	int result = 0;
	try {
		std::string str = "path/to/items";
		std::queue<std::string> parts = Explode(str, '/');
		ASSERT_EQUAL("test_path_explode", 3, parts.size());
		ASSERT_EQUAL("test_path_explode", "path", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_path_explode", "to", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_path_explode", "items", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_path_explode", true, parts.empty());
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_path_explode", result);
}
int test_explode_one_item() {
	int result = 0;
	try {
		std::string str = "Hello";
		std::queue<std::string> parts = Explode(str, '/');
		ASSERT_EQUAL("test_explode_one_item", 1, parts.size());
		ASSERT_EQUAL("test_explode_one_item", "Hello", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_explode_one_item", true, parts.empty());
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_explode_one_item", result);
}
int test_temp_path() {
	int result = 0;
	try {
		const std::filesystem::path path = StormByte::System::TempFileName("something");
		const bool exists = std::filesystem::exists(path);
		std::remove(path.string().c_str());
		ASSERT_TRUE("test_temp_path", exists);
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_temp_path", result);
}
int test_human_readable_byte_size() {
    int result = 0;
    const std::string locale = "en_US.UTF-8"; // Can't be a constexpr or gcc complains
    try {
        // Explicitly specify the type of T
        std::string size = HumanReadable<uint64_t>(1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 KiB", size);
        size = HumanReadable<uint64_t>(1024ULL * 1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 MiB", size);
        size = HumanReadable<uint64_t>(1024ULL * 1024 * 1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 GiB", size);
        size = HumanReadable<uint64_t>(1024ULL * 1024 * 1024 * 1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 TiB", size);
        size = HumanReadable<uint64_t>(1024ULL * 1024 * 1024 * 1024 * 1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 PiB", size);
		size = HumanReadable<double>(1027.65, Format::HumanReadableBytes, locale);
		ASSERT_EQUAL("test_human_readable_byte_size", "1 KiB", size);
		size = HumanReadable<double>(1154.65, Format::HumanReadableBytes, locale);
		ASSERT_EQUAL("test_human_readable_byte_size", "1.13 KiB", size);
    } catch (const StormByte::Exception& ex) {
        std::cerr << ex.what() << std::endl;
        result++;
    }
    RETURN_TEST("test_human_readable_byte_size", result);
}
int test_human_readable_negative_bytes() {
	int result = 0;
	const std::string size = HumanReadable<int>(-1024, Format::HumanReadableBytes, "C");
	ASSERT_EQUAL("test_human_readable_negative_bytes", "-1 KiB", size);
	RETURN_TEST("test_human_readable_negative_bytes", result);
}
int test_human_readable_number() {
	int result = 0;
	try {
		std::string number = HumanReadable<int>(1024, Format::HumanReadableNumber, "en_US.UTF-8");
		ASSERT_EQUAL("test_human_readable_number", "1,024", number);
		number = HumanReadable<int>(1024 * 1024, Format::HumanReadableNumber, "en_US.UTF-8");
		ASSERT_EQUAL("test_human_readable_number", "1,048,576", number);
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_human_readable_number", result);
}
int test_buffer_to_string() {
	const std::string test_string = "test_buffer_to_string";
	std::vector<std::byte> buffer = StormByte::String::ToByteVector(test_string);
	std::string str = StormByte::String::FromByteVector(buffer);
	ASSERT_EQUAL("test_buffer_to_string", test_string, str);
	RETURN_TEST("test_buffer_to_string", 0);
}
int test_case_conversion_high_bytes() {
	int result = 0;
	const std::string input = "A\xC0\xFFz";
	ASSERT_EQUAL("test_case_conversion_high_bytes", std::string("a\xC0\xFFz"), ToLower(input));
	ASSERT_EQUAL("test_case_conversion_high_bytes", std::string("A\xC0\xFFZ"), ToUpper(input));
	RETURN_TEST("test_case_conversion_high_bytes", result);
}
int test_utf8_conversion_is_locale_independent() {
	int result = 0;
	const char* current_locale = std::setlocale(LC_ALL, nullptr);
	const std::string saved_locale = current_locale == nullptr ? "C" : current_locale;
	std::setlocale(LC_ALL, "C");
	try {
		const std::wstring input = L"caf\u00e9 \U0001F600";
		const std::string encoded = UTF8Encode(input);
		ASSERT_EQUAL("test_utf8_conversion_is_locale_independent", "caf\xC3\xA9 \xF0\x9F\x98\x80", encoded);
		ASSERT_EQUAL("test_utf8_conversion_is_locale_independent", input, UTF8Decode(encoded));
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	std::setlocale(LC_ALL, saved_locale.c_str());
	RETURN_TEST("test_utf8_conversion_is_locale_independent", result);
}
int test_utf8_conversion_rejects_invalid_input() {
	int result = 0;
	const std::vector<std::string> invalid_utf8 = {
		std::string("\x80", 1),
		std::string("\xC0\x80", 2),
		std::string("\xE0\x80\x80", 3),
		std::string("\xF0\x80\x80\x80", 4),
		std::string("\xED\xA0\x80", 3),
		std::string("\xF4\x90\x80\x80", 4),
		std::string("\xF0\x28\x8C\x28", 4),
		std::string("\xC2", 1),
		std::string("\xE2\x82", 2),
		std::string("\xF0\x9F\x98", 3)
	};
	for (const std::string& input : invalid_utf8) {
		bool threw = false;
		try {
			UTF8Decode(input);
		} catch (const StormByte::UTF8Error&) {
			threw = true;
		}
		ASSERT_TRUE("test_utf8_conversion_rejects_invalid_input", threw);
	}
	bool threw = false;
	try {
		UTF8Encode(std::wstring(1, static_cast<wchar_t>(0xD800)));
	} catch (const StormByte::UTF8Error&) {
		threw = true;
	}
	ASSERT_TRUE("test_utf8_conversion_rejects_invalid_input", threw);
	RETURN_TEST("test_utf8_conversion_rejects_invalid_input", result);
}
int test_utf8_conversion_boundaries() {
	int result = 0;
	const std::vector<std::pair<std::wstring, std::string>> cases = {
		{std::wstring(1, static_cast<wchar_t>(0x7F)), std::string("\x7F", 1)},
		{std::wstring(1, static_cast<wchar_t>(0x80)), std::string("\xC2\x80", 2)},
		{std::wstring(1, static_cast<wchar_t>(0x7FF)), std::string("\xDF\xBF", 2)},
		{std::wstring(1, static_cast<wchar_t>(0x800)), std::string("\xE0\xA0\x80", 3)},
		{std::wstring(1, static_cast<wchar_t>(0xFFFF)), std::string("\xEF\xBF\xBF", 3)}
	};
	for (const auto& [wide, utf8] : cases) {
		ASSERT_EQUAL("test_utf8_conversion_boundaries", utf8, UTF8Encode(wide));
		ASSERT_EQUAL("test_utf8_conversion_boundaries", wide, UTF8Decode(utf8));
	}
	if constexpr (sizeof(wchar_t) == 2) {
		const std::wstring pair = {static_cast<wchar_t>(0xD800), static_cast<wchar_t>(0xDC00)};
		ASSERT_EQUAL("test_utf8_conversion_boundaries", std::string("\xF0\x90\x80\x80", 4), UTF8Encode(pair));
		ASSERT_EQUAL("test_utf8_conversion_boundaries", pair, UTF8Decode("\xF0\x90\x80\x80"));
	} else {
		const std::vector<std::pair<std::wstring, std::string>> supplementary = {
			{std::wstring(1, static_cast<wchar_t>(0x10000)), std::string("\xF0\x90\x80\x80", 4)},
			{std::wstring(1, static_cast<wchar_t>(0x10FFFF)), std::string("\xF4\x8F\xBF\xBF", 4)}
		};
		for (const auto& [wide, utf8] : supplementary) {
			ASSERT_EQUAL("test_utf8_conversion_boundaries", utf8, UTF8Encode(wide));
			ASSERT_EQUAL("test_utf8_conversion_boundaries", wide, UTF8Decode(utf8));
		}
	}
	RETURN_TEST("test_utf8_conversion_boundaries", result);
}
int main() {
    int result = 0;
    try {
		result += test_simple_explode();
		result += test_explode_consecutive_delimiters();
		result += test_path_explode();
		result += test_explode_one_item();
		result += test_temp_path();
		result += test_human_readable_byte_size();
		result += test_human_readable_negative_bytes();
		result += test_human_readable_number();
		result += test_buffer_to_string();
		result += test_case_conversion_high_bytes();
		result += test_utf8_conversion_is_locale_independent();
		result += test_utf8_conversion_rejects_invalid_input();
		result += test_utf8_conversion_boundaries();
    } catch (const StormByte::Exception& ex) {
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
