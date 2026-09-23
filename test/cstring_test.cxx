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

#include <StormByte/cstring.hxx>
#include <StormByte/test_handlers.h>

#include <cstring>
#include <sstream>
#include <string>
#include <utility>

using namespace StormByte;

namespace {
	const char* View(const CString& text) {
		return static_cast<const char*>(text);
	}
}

// -------------------
// Construct
// -------------------

int test_default_is_null() {
	int result = 0;
	CString text;
	ASSERT_TRUE("test_default_is_null", View(text) == nullptr);
	ASSERT_EQUAL("test_default_is_null", 0u, text.Length());
	RETURN_TEST("test_default_is_null", result);
}

int test_construct_from_null() {
	int result = 0;
	CString text(static_cast<const char*>(nullptr));
	ASSERT_TRUE("test_construct_from_null", View(text) == nullptr);
	ASSERT_EQUAL("test_construct_from_null", 0u, text.Length());
	RETURN_TEST("test_construct_from_null", result);
}

int test_construct_from_empty() {
	int result = 0;
	CString text("");
	ASSERT_TRUE("test_construct_from_empty", View(text) != nullptr);
	ASSERT_EQUAL("test_construct_from_empty", 0, std::strcmp(View(text), ""));
	ASSERT_EQUAL("test_construct_from_empty", 0u, text.Length());
	RETURN_TEST("test_construct_from_empty", result);
}

int test_construct_copies_text() {
	int result = 0;
	const char raw[] = "hello";
	CString text(raw);
	ASSERT_TRUE("test_construct_copies_text", View(text) != raw);
	ASSERT_EQUAL("test_construct_copies_text", 0, std::strcmp(View(text), "hello"));
	ASSERT_EQUAL("test_construct_copies_text", 5u, text.Length());
	RETURN_TEST("test_construct_copies_text", result);
}

int test_construct_stops_at_embedded_nul() {
	int result = 0;
	const char raw[] = { 'a', 'b', '\0', 'c', '\0' };
	CString text(raw);
	ASSERT_TRUE("test_construct_stops_at_embedded_nul", View(text) != nullptr);
	ASSERT_EQUAL("test_construct_stops_at_embedded_nul", 2u, text.Length());
	ASSERT_EQUAL("test_construct_stops_at_embedded_nul", 0, std::strcmp(View(text), "ab"));
	RETURN_TEST("test_construct_stops_at_embedded_nul", result);
}

int test_construct_utf8() {
	int result = 0;
	CString text("cañón 日本語");
	ASSERT_TRUE("test_construct_utf8", View(text) != nullptr);
	ASSERT_EQUAL("test_construct_utf8", 0, std::strcmp(View(text), "cañón 日本語"));
	ASSERT_EQUAL("test_construct_utf8", std::strlen("cañón 日本語"), text.Length());
	RETURN_TEST("test_construct_utf8", result);
}

int test_construct_long() {
	int result = 0;
	const std::string raw(4096, 'X');
	CString text(raw.c_str());
	ASSERT_TRUE("test_construct_long", View(text) != nullptr);
	ASSERT_EQUAL("test_construct_long", raw.size(), text.Length());
	ASSERT_EQUAL("test_construct_long", 0, std::strcmp(View(text), raw.c_str()));
	RETURN_TEST("test_construct_long", result);
}

// -------------------
// Copy / move
// -------------------

int test_copy_is_independent() {
	int result = 0;
	CString original("alpha");
	CString copy(original);
	ASSERT_TRUE("test_copy_is_independent", View(copy) != View(original));
	ASSERT_EQUAL("test_copy_is_independent", 0, std::strcmp(View(copy), "alpha"));
	copy.Reset("beta");
	ASSERT_EQUAL("test_copy_is_independent", 0, std::strcmp(View(original), "alpha"));
	ASSERT_EQUAL("test_copy_is_independent", 0, std::strcmp(View(copy), "beta"));
	ASSERT_EQUAL("test_copy_is_independent", 5u, original.Length());
	ASSERT_EQUAL("test_copy_is_independent", 4u, copy.Length());
	RETURN_TEST("test_copy_is_independent", result);
}

int test_copy_null() {
	int result = 0;
	CString original;
	CString copy(original);
	ASSERT_TRUE("test_copy_null", View(original) == nullptr);
	ASSERT_TRUE("test_copy_null", View(copy) == nullptr);
	ASSERT_EQUAL("test_copy_null", 0u, copy.Length());
	RETURN_TEST("test_copy_null", result);
}

int test_copy_assign_overwrites() {
	int result = 0;
	CString left("old");
	CString right("new");
	left = right;
	ASSERT_TRUE("test_copy_assign_overwrites", View(left) != View(right));
	ASSERT_EQUAL("test_copy_assign_overwrites", 0, std::strcmp(View(left), "new"));
	ASSERT_EQUAL("test_copy_assign_overwrites", 0, std::strcmp(View(right), "new"));
	RETURN_TEST("test_copy_assign_overwrites", result);
}

int test_copy_assign_self() {
	int result = 0;
	CString text("self");
	text = text;
	ASSERT_TRUE("test_copy_assign_self", View(text) != nullptr);
	ASSERT_EQUAL("test_copy_assign_self", 0, std::strcmp(View(text), "self"));
	RETURN_TEST("test_copy_assign_self", result);
}

int test_move_leaves_source_null() {
	int result = 0;
	CString original("payload");
	const char* raw = View(original);
	CString taken(std::move(original));
	ASSERT_TRUE("test_move_leaves_source_null", View(original) == nullptr);
	ASSERT_EQUAL("test_move_leaves_source_null", 0u, original.Length());
	ASSERT_TRUE("test_move_leaves_source_null", View(taken) == raw);
	ASSERT_EQUAL("test_move_leaves_source_null", 0, std::strcmp(View(taken), "payload"));
	RETURN_TEST("test_move_leaves_source_null", result);
}

int test_move_assign_leaves_source_null() {
	int result = 0;
	CString left("old");
	CString right("fresh");
	const char* raw = View(right);
	left = std::move(right);
	ASSERT_TRUE("test_move_assign_leaves_source_null", View(right) == nullptr);
	ASSERT_TRUE("test_move_assign_leaves_source_null", View(left) == raw);
	ASSERT_EQUAL("test_move_assign_leaves_source_null", 0, std::strcmp(View(left), "fresh"));
	RETURN_TEST("test_move_assign_leaves_source_null", result);
}

int test_move_assign_self() {
	int result = 0;
	CString text("self-move");
	text = std::move(text);
	ASSERT_TRUE("test_move_assign_self", View(text) != nullptr);
	ASSERT_EQUAL("test_move_assign_self", 0, std::strcmp(View(text), "self-move"));
	RETURN_TEST("test_move_assign_self", result);
}

int test_move_from_null() {
	int result = 0;
	CString original;
	CString taken(std::move(original));
	ASSERT_TRUE("test_move_from_null", View(original) == nullptr);
	ASSERT_TRUE("test_move_from_null", View(taken) == nullptr);
	RETURN_TEST("test_move_from_null", result);
}

// -------------------
// Reset
// -------------------

int test_reset_replaces() {
	int result = 0;
	CString text("first");
	text.Reset("second");
	ASSERT_EQUAL("test_reset_replaces", 0, std::strcmp(View(text), "second"));
	ASSERT_EQUAL("test_reset_replaces", 6u, text.Length());
	text.Reset();
	ASSERT_TRUE("test_reset_replaces", View(text) == nullptr);
	ASSERT_EQUAL("test_reset_replaces", 0u, text.Length());
	text.Reset("third");
	ASSERT_EQUAL("test_reset_replaces", 0, std::strcmp(View(text), "third"));
	RETURN_TEST("test_reset_replaces", result);
}

int test_reset_from_null_pointer() {
	int result = 0;
	CString text("keep");
	text.Reset(nullptr);
	ASSERT_TRUE("test_reset_from_null_pointer", View(text) == nullptr);
	ASSERT_EQUAL("test_reset_from_null_pointer", 0u, text.Length());
	RETURN_TEST("test_reset_from_null_pointer", result);
}

int test_view_invalid_after_reset() {
	int result = 0;
	CString text("alive");
	text.Reset("other");
	ASSERT_EQUAL("test_view_invalid_after_reset", 0, std::strcmp(View(text), "other"));
	ASSERT_EQUAL("test_view_invalid_after_reset", 5u, text.Length());
	RETURN_TEST("test_view_invalid_after_reset", result);
}

// -------------------
// Conversions
// -------------------

int test_string_conversion_copies() {
	int result = 0;
	CString text("bridge");
	const std::string copy = text;
	ASSERT_EQUAL("test_string_conversion_copies", std::string("bridge"), copy);
	text.Reset("changed");
	ASSERT_EQUAL("test_string_conversion_copies", std::string("bridge"), copy);
	RETURN_TEST("test_string_conversion_copies", result);
}

int test_string_conversion_from_null() {
	int result = 0;
	CString text;
	const std::string copy = text;
	ASSERT_TRUE("test_string_conversion_from_null", copy.empty());
	RETURN_TEST("test_string_conversion_from_null", result);
}

int test_string_conversion_from_empty() {
	int result = 0;
	CString text("");
	const std::string copy = text;
	ASSERT_TRUE("test_string_conversion_from_empty", copy.empty());
	RETURN_TEST("test_string_conversion_from_empty", result);
}

// -------------------
// Streams
// -------------------

int test_free_stream_operator() {
	int result = 0;
	CString text("streamed");
	std::ostringstream out;
	out << text;
	ASSERT_EQUAL("test_free_stream_operator", std::string("streamed"), out.str());
	RETURN_TEST("test_free_stream_operator", result);
}

int test_member_stream_operator() {
	int result = 0;
	CString text("member");
	std::ostringstream out;
	text.operator<<(out);
	ASSERT_EQUAL("test_member_stream_operator", std::string("member"), out.str());
	RETURN_TEST("test_member_stream_operator", result);
}

int test_stream_null_writes_nothing() {
	int result = 0;
	CString text;
	std::ostringstream out;
	out << "pre";
	out << text;
	out << "post";
	ASSERT_EQUAL("test_stream_null_writes_nothing", std::string("prepost"), out.str());
	RETURN_TEST("test_stream_null_writes_nothing", result);
}

int test_stream_empty_writes_nothing_extra() {
	int result = 0;
	CString text("");
	std::ostringstream out;
	out << '[' << text << ']';
	ASSERT_EQUAL("test_stream_empty_writes_nothing_extra", std::string("[]"), out.str());
	RETURN_TEST("test_stream_empty_writes_nothing_extra", result);
}

int main() {
	int result = 0;

	// -------------------
	// Construct
	// -------------------
	result += test_default_is_null();
	result += test_construct_from_null();
	result += test_construct_from_empty();
	result += test_construct_copies_text();
	result += test_construct_stops_at_embedded_nul();
	result += test_construct_utf8();
	result += test_construct_long();

	// -------------------
	// Copy / move
	// -------------------
	result += test_copy_is_independent();
	result += test_copy_null();
	result += test_copy_assign_overwrites();
	result += test_copy_assign_self();
	result += test_move_leaves_source_null();
	result += test_move_assign_leaves_source_null();
	result += test_move_assign_self();
	result += test_move_from_null();

	// -------------------
	// Reset
	// -------------------
	result += test_reset_replaces();
	result += test_reset_from_null_pointer();
	result += test_view_invalid_after_reset();

	// -------------------
	// Conversions
	// -------------------
	result += test_string_conversion_copies();
	result += test_string_conversion_from_null();
	result += test_string_conversion_from_empty();

	// -------------------
	// Streams
	// -------------------
	result += test_free_stream_operator();
	result += test_member_stream_operator();
	result += test_stream_null_writes_nothing();
	result += test_stream_empty_writes_nothing_extra();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
