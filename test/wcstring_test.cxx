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
#include <StormByte/wcstring.hxx>

#include <cwchar>
#include <sstream>
#include <string>
#include <utility>

using namespace StormByte;

namespace {
	const wchar_t* View(const WCString& text) {
		return static_cast<const wchar_t*>(text);
	}
}

// -------------------
// Construct
// -------------------

int test_default_is_null() {
	int result = 0;
	WCString text;
	ASSERT_TRUE("test_default_is_null", View(text) == nullptr);
	ASSERT_EQUAL("test_default_is_null", 0u, text.Length());
	RETURN_TEST("test_default_is_null", result);
}

int test_construct_from_null() {
	int result = 0;
	WCString text(static_cast<const wchar_t*>(nullptr));
	ASSERT_TRUE("test_construct_from_null", View(text) == nullptr);
	ASSERT_EQUAL("test_construct_from_null", 0u, text.Length());
	RETURN_TEST("test_construct_from_null", result);
}

int test_construct_from_empty() {
	int result = 0;
	WCString text(L"");
	ASSERT_TRUE("test_construct_from_empty", View(text) != nullptr);
	ASSERT_EQUAL("test_construct_from_empty", 0, std::wcscmp(View(text), L""));
	ASSERT_EQUAL("test_construct_from_empty", 0u, text.Length());
	RETURN_TEST("test_construct_from_empty", result);
}

int test_construct_copies_text() {
	int result = 0;
	const wchar_t raw[] = L"hello";
	WCString text(raw);
	ASSERT_TRUE("test_construct_copies_text", View(text) != raw);
	ASSERT_EQUAL("test_construct_copies_text", 0, std::wcscmp(View(text), L"hello"));
	ASSERT_EQUAL("test_construct_copies_text", 5u, text.Length());
	RETURN_TEST("test_construct_copies_text", result);
}

int test_construct_stops_at_embedded_nul() {
	int result = 0;
	const wchar_t raw[] = { L'a', L'b', L'\0', L'c', L'\0' };
	WCString text(raw);
	ASSERT_TRUE("test_construct_stops_at_embedded_nul", View(text) != nullptr);
	ASSERT_EQUAL("test_construct_stops_at_embedded_nul", 2u, text.Length());
	ASSERT_EQUAL("test_construct_stops_at_embedded_nul", 0, std::wcscmp(View(text), L"ab"));
	RETURN_TEST("test_construct_stops_at_embedded_nul", result);
}

int test_construct_unicode() {
	int result = 0;
	WCString text(L"cañón 日本語");
	ASSERT_TRUE("test_construct_unicode", View(text) != nullptr);
	ASSERT_EQUAL("test_construct_unicode", 0, std::wcscmp(View(text), L"cañón 日本語"));
	ASSERT_EQUAL("test_construct_unicode", std::wcslen(L"cañón 日本語"), text.Length());
	RETURN_TEST("test_construct_unicode", result);
}

int test_construct_long() {
	int result = 0;
	const std::wstring raw(4096, L'X');
	WCString text(raw.c_str());
	ASSERT_TRUE("test_construct_long", View(text) != nullptr);
	ASSERT_EQUAL("test_construct_long", raw.size(), text.Length());
	ASSERT_EQUAL("test_construct_long", 0, std::wcscmp(View(text), raw.c_str()));
	RETURN_TEST("test_construct_long", result);
}

// -------------------
// Copy / move
// -------------------

int test_copy_is_independent() {
	int result = 0;
	WCString original(L"alpha");
	WCString copy(original);
	ASSERT_TRUE("test_copy_is_independent", View(copy) != View(original));
	ASSERT_EQUAL("test_copy_is_independent", 0, std::wcscmp(View(copy), L"alpha"));
	copy.Reset(L"beta");
	ASSERT_EQUAL("test_copy_is_independent", 0, std::wcscmp(View(original), L"alpha"));
	ASSERT_EQUAL("test_copy_is_independent", 0, std::wcscmp(View(copy), L"beta"));
	ASSERT_EQUAL("test_copy_is_independent", 5u, original.Length());
	ASSERT_EQUAL("test_copy_is_independent", 4u, copy.Length());
	RETURN_TEST("test_copy_is_independent", result);
}

int test_copy_null() {
	int result = 0;
	WCString original;
	WCString copy(original);
	ASSERT_TRUE("test_copy_null", View(original) == nullptr);
	ASSERT_TRUE("test_copy_null", View(copy) == nullptr);
	ASSERT_EQUAL("test_copy_null", 0u, copy.Length());
	RETURN_TEST("test_copy_null", result);
}

int test_copy_assign_overwrites() {
	int result = 0;
	WCString left(L"old");
	WCString right(L"new");
	left = right;
	ASSERT_TRUE("test_copy_assign_overwrites", View(left) != View(right));
	ASSERT_EQUAL("test_copy_assign_overwrites", 0, std::wcscmp(View(left), L"new"));
	ASSERT_EQUAL("test_copy_assign_overwrites", 0, std::wcscmp(View(right), L"new"));
	RETURN_TEST("test_copy_assign_overwrites", result);
}

int test_copy_assign_self() {
	int result = 0;
	WCString text(L"self");
	text = text;
	ASSERT_TRUE("test_copy_assign_self", View(text) != nullptr);
	ASSERT_EQUAL("test_copy_assign_self", 0, std::wcscmp(View(text), L"self"));
	RETURN_TEST("test_copy_assign_self", result);
}

int test_move_leaves_source_null() {
	int result = 0;
	WCString original(L"payload");
	const wchar_t* raw = View(original);
	WCString taken(std::move(original));
	ASSERT_TRUE("test_move_leaves_source_null", View(original) == nullptr);
	ASSERT_EQUAL("test_move_leaves_source_null", 0u, original.Length());
	ASSERT_TRUE("test_move_leaves_source_null", View(taken) == raw);
	ASSERT_EQUAL("test_move_leaves_source_null", 0, std::wcscmp(View(taken), L"payload"));
	RETURN_TEST("test_move_leaves_source_null", result);
}

int test_move_assign_leaves_source_null() {
	int result = 0;
	WCString left(L"old");
	WCString right(L"fresh");
	const wchar_t* raw = View(right);
	left = std::move(right);
	ASSERT_TRUE("test_move_assign_leaves_source_null", View(right) == nullptr);
	ASSERT_TRUE("test_move_assign_leaves_source_null", View(left) == raw);
	ASSERT_EQUAL("test_move_assign_leaves_source_null", 0, std::wcscmp(View(left), L"fresh"));
	RETURN_TEST("test_move_assign_leaves_source_null", result);
}

int test_move_assign_self() {
	int result = 0;
	WCString text(L"self-move");
	text = std::move(text);
	ASSERT_TRUE("test_move_assign_self", View(text) != nullptr);
	ASSERT_EQUAL("test_move_assign_self", 0, std::wcscmp(View(text), L"self-move"));
	RETURN_TEST("test_move_assign_self", result);
}

int test_move_from_null() {
	int result = 0;
	WCString original;
	WCString taken(std::move(original));
	ASSERT_TRUE("test_move_from_null", View(original) == nullptr);
	ASSERT_TRUE("test_move_from_null", View(taken) == nullptr);
	RETURN_TEST("test_move_from_null", result);
}

// -------------------
// Reset
// -------------------

int test_reset_replaces() {
	int result = 0;
	WCString text(L"first");
	text.Reset(L"second");
	ASSERT_EQUAL("test_reset_replaces", 0, std::wcscmp(View(text), L"second"));
	ASSERT_EQUAL("test_reset_replaces", 6u, text.Length());
	text.Reset();
	ASSERT_TRUE("test_reset_replaces", View(text) == nullptr);
	ASSERT_EQUAL("test_reset_replaces", 0u, text.Length());
	text.Reset(L"third");
	ASSERT_EQUAL("test_reset_replaces", 0, std::wcscmp(View(text), L"third"));
	RETURN_TEST("test_reset_replaces", result);
}

int test_reset_from_null_pointer() {
	int result = 0;
	WCString text(L"keep");
	text.Reset(nullptr);
	ASSERT_TRUE("test_reset_from_null_pointer", View(text) == nullptr);
	ASSERT_EQUAL("test_reset_from_null_pointer", 0u, text.Length());
	RETURN_TEST("test_reset_from_null_pointer", result);
}

int test_view_after_reset() {
	int result = 0;
	WCString text(L"alive");
	text.Reset(L"other");
	ASSERT_EQUAL("test_view_after_reset", 0, std::wcscmp(View(text), L"other"));
	ASSERT_EQUAL("test_view_after_reset", 5u, text.Length());
	RETURN_TEST("test_view_after_reset", result);
}

// -------------------
// Conversions
// -------------------

int test_wstring_conversion_copies() {
	int result = 0;
	WCString text(L"bridge");
	const std::wstring copy = text;
	ASSERT_EQUAL("test_wstring_conversion_copies", std::wstring(L"bridge"), copy);
	text.Reset(L"changed");
	ASSERT_EQUAL("test_wstring_conversion_copies", std::wstring(L"bridge"), copy);
	RETURN_TEST("test_wstring_conversion_copies", result);
}

int test_wstring_conversion_from_null() {
	int result = 0;
	WCString text;
	const std::wstring copy = text;
	ASSERT_TRUE("test_wstring_conversion_from_null", copy.empty());
	RETURN_TEST("test_wstring_conversion_from_null", result);
}

int test_wstring_conversion_from_empty() {
	int result = 0;
	WCString text(L"");
	const std::wstring copy = text;
	ASSERT_TRUE("test_wstring_conversion_from_empty", copy.empty());
	RETURN_TEST("test_wstring_conversion_from_empty", result);
}

// -------------------
// Streams
// -------------------

int test_free_stream_operator() {
	int result = 0;
	WCString text(L"streamed");
	std::wostringstream out;
	out << text;
	ASSERT_TRUE("test_free_stream_operator", out.str() == L"streamed");
	RETURN_TEST("test_free_stream_operator", result);
}

int test_member_stream_operator() {
	int result = 0;
	WCString text(L"member");
	std::wostringstream out;
	text.operator<<(out);
	ASSERT_TRUE("test_member_stream_operator", out.str() == L"member");
	RETURN_TEST("test_member_stream_operator", result);
}

int test_stream_null_writes_nothing() {
	int result = 0;
	WCString text;
	std::wostringstream out;
	out << L"pre";
	out << text;
	out << L"post";
	ASSERT_TRUE("test_stream_null_writes_nothing", out.str() == L"prepost");
	RETURN_TEST("test_stream_null_writes_nothing", result);
}

int test_stream_empty_writes_nothing_extra() {
	int result = 0;
	WCString text(L"");
	std::wostringstream out;
	out << L'[' << text << L']';
	ASSERT_TRUE("test_stream_empty_writes_nothing_extra", out.str() == L"[]");
	RETURN_TEST("test_stream_empty_writes_nothing_extra", result);
}

int main() {
	int result = 0;

	result += test_default_is_null();
	result += test_construct_from_null();
	result += test_construct_from_empty();
	result += test_construct_copies_text();
	result += test_construct_stops_at_embedded_nul();
	result += test_construct_unicode();
	result += test_construct_long();

	result += test_copy_is_independent();
	result += test_copy_null();
	result += test_copy_assign_overwrites();
	result += test_copy_assign_self();
	result += test_move_leaves_source_null();
	result += test_move_assign_leaves_source_null();
	result += test_move_assign_self();
	result += test_move_from_null();

	result += test_reset_replaces();
	result += test_reset_from_null_pointer();
	result += test_view_after_reset();

	result += test_wstring_conversion_copies();
	result += test_wstring_conversion_from_null();
	result += test_wstring_conversion_from_empty();

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
