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
#include <string>
#include <utility>

using namespace StormByte;

// -------------------
// Construct
// -------------------

int test_default_is_null() {
	int result = 0;
	CString text;
	ASSERT_TRUE("test_default_is_null", text.Get() == nullptr);
	RETURN_TEST("test_default_is_null", result);
}

int test_construct_from_null() {
	int result = 0;
	CString text(static_cast<const char*>(nullptr));
	ASSERT_TRUE("test_construct_from_null", text.Get() == nullptr);
	RETURN_TEST("test_construct_from_null", result);
}

int test_construct_from_empty() {
	int result = 0;
	CString text("");
	ASSERT_TRUE("test_construct_from_empty", text.Get() != nullptr);
	ASSERT_EQUAL("test_construct_from_empty", 0, std::strcmp(text.Get(), ""));
	RETURN_TEST("test_construct_from_empty", result);
}

int test_construct_copies_text() {
	int result = 0;
	const char raw[] = "hello";
	CString text(raw);
	ASSERT_TRUE("test_construct_copies_text", text.Get() != raw);
	ASSERT_EQUAL("test_construct_copies_text", 0, std::strcmp(text.Get(), "hello"));
	RETURN_TEST("test_construct_copies_text", result);
}

int test_construct_stops_at_embedded_nul() {
	int result = 0;
	const char raw[] = { 'a', 'b', '\0', 'c', '\0' };
	CString text(raw);
	ASSERT_TRUE("test_construct_stops_at_embedded_nul", text.Get() != nullptr);
	ASSERT_EQUAL("test_construct_stops_at_embedded_nul", 2u, std::strlen(text.Get()));
	ASSERT_EQUAL("test_construct_stops_at_embedded_nul", 0, std::strcmp(text.Get(), "ab"));
	RETURN_TEST("test_construct_stops_at_embedded_nul", result);
}

int test_construct_utf8() {
	int result = 0;
	CString text("cañón 日本語");
	ASSERT_TRUE("test_construct_utf8", text.Get() != nullptr);
	ASSERT_EQUAL("test_construct_utf8", 0, std::strcmp(text.Get(), "cañón 日本語"));
	RETURN_TEST("test_construct_utf8", result);
}

int test_construct_long() {
	int result = 0;
	const std::string raw(4096, 'X');
	CString text(raw.c_str());
	ASSERT_TRUE("test_construct_long", text.Get() != nullptr);
	ASSERT_EQUAL("test_construct_long", raw.size(), std::strlen(text.Get()));
	ASSERT_EQUAL("test_construct_long", 0, std::strcmp(text.Get(), raw.c_str()));
	RETURN_TEST("test_construct_long", result);
}

// -------------------
// Copy / move
// -------------------

int test_copy_is_independent() {
	int result = 0;
	CString original("alpha");
	CString copy(original);
	ASSERT_TRUE("test_copy_is_independent", copy.Get() != original.Get());
	ASSERT_EQUAL("test_copy_is_independent", 0, std::strcmp(copy.Get(), "alpha"));
	copy.Reset("beta");
	ASSERT_EQUAL("test_copy_is_independent", 0, std::strcmp(original.Get(), "alpha"));
	ASSERT_EQUAL("test_copy_is_independent", 0, std::strcmp(copy.Get(), "beta"));
	RETURN_TEST("test_copy_is_independent", result);
}

int test_copy_null() {
	int result = 0;
	CString original;
	CString copy(original);
	ASSERT_TRUE("test_copy_null", original.Get() == nullptr);
	ASSERT_TRUE("test_copy_null", copy.Get() == nullptr);
	RETURN_TEST("test_copy_null", result);
}

int test_copy_assign_overwrites() {
	int result = 0;
	CString left("old");
	CString right("new");
	left = right;
	ASSERT_TRUE("test_copy_assign_overwrites", left.Get() != right.Get());
	ASSERT_EQUAL("test_copy_assign_overwrites", 0, std::strcmp(left.Get(), "new"));
	ASSERT_EQUAL("test_copy_assign_overwrites", 0, std::strcmp(right.Get(), "new"));
	RETURN_TEST("test_copy_assign_overwrites", result);
}

int test_copy_assign_self() {
	int result = 0;
	CString text("self");
	text = text;
	ASSERT_TRUE("test_copy_assign_self", text.Get() != nullptr);
	ASSERT_EQUAL("test_copy_assign_self", 0, std::strcmp(text.Get(), "self"));
	RETURN_TEST("test_copy_assign_self", result);
}

int test_move_leaves_source_null() {
	int result = 0;
	CString original("payload");
	const char* raw = original.Get();
	CString taken(std::move(original));
	ASSERT_TRUE("test_move_leaves_source_null", original.Get() == nullptr);
	ASSERT_TRUE("test_move_leaves_source_null", taken.Get() == raw);
	ASSERT_EQUAL("test_move_leaves_source_null", 0, std::strcmp(taken.Get(), "payload"));
	RETURN_TEST("test_move_leaves_source_null", result);
}

int test_move_assign_leaves_source_null() {
	int result = 0;
	CString left("old");
	CString right("fresh");
	const char* raw = right.Get();
	left = std::move(right);
	ASSERT_TRUE("test_move_assign_leaves_source_null", right.Get() == nullptr);
	ASSERT_TRUE("test_move_assign_leaves_source_null", left.Get() == raw);
	ASSERT_EQUAL("test_move_assign_leaves_source_null", 0, std::strcmp(left.Get(), "fresh"));
	RETURN_TEST("test_move_assign_leaves_source_null", result);
}

int test_move_assign_self() {
	int result = 0;
	CString text("self-move");
	text = std::move(text);
	ASSERT_TRUE("test_move_assign_self", text.Get() != nullptr);
	ASSERT_EQUAL("test_move_assign_self", 0, std::strcmp(text.Get(), "self-move"));
	RETURN_TEST("test_move_assign_self", result);
}

int test_move_from_null() {
	int result = 0;
	CString original;
	CString taken(std::move(original));
	ASSERT_TRUE("test_move_from_null", original.Get() == nullptr);
	ASSERT_TRUE("test_move_from_null", taken.Get() == nullptr);
	RETURN_TEST("test_move_from_null", result);
}

// -------------------
// Reset / Release
// -------------------

int test_reset_replaces() {
	int result = 0;
	CString text("first");
	text.Reset("second");
	ASSERT_EQUAL("test_reset_replaces", 0, std::strcmp(text.Get(), "second"));
	text.Reset();
	ASSERT_TRUE("test_reset_replaces", text.Get() == nullptr);
	text.Reset("third");
	ASSERT_EQUAL("test_reset_replaces", 0, std::strcmp(text.Get(), "third"));
	RETURN_TEST("test_reset_replaces", result);
}

int test_reset_from_null_pointer() {
	int result = 0;
	CString text("keep");
	text.Reset(nullptr);
	ASSERT_TRUE("test_reset_from_null_pointer", text.Get() == nullptr);
	RETURN_TEST("test_reset_from_null_pointer", result);
}

int test_release_transfers_and_requires_delete() {
	int result = 0;
	CString text("owned");
	const char* raw = text.Release();
	ASSERT_TRUE("test_release_transfers_and_requires_delete", text.Get() == nullptr);
	ASSERT_TRUE("test_release_transfers_and_requires_delete", raw != nullptr);
	ASSERT_EQUAL("test_release_transfers_and_requires_delete", 0, std::strcmp(raw, "owned"));
	delete[] raw;
	RETURN_TEST("test_release_transfers_and_requires_delete", result);
}

int test_release_null() {
	int result = 0;
	CString text;
	const char* raw = text.Release();
	ASSERT_TRUE("test_release_null", raw == nullptr);
	ASSERT_TRUE("test_release_null", text.Get() == nullptr);
	RETURN_TEST("test_release_null", result);
}

int test_reset_after_release() {
	int result = 0;
	CString text("gone");
	const char* raw = text.Release();
	delete[] raw;
	text.Reset("back");
	ASSERT_EQUAL("test_reset_after_release", 0, std::strcmp(text.Get(), "back"));
	RETURN_TEST("test_reset_after_release", result);
}

int test_assign_after_release() {
	int result = 0;
	CString left("left");
	CString right("right");
	const char* raw = left.Release();
	delete[] raw;
	left = right;
	ASSERT_EQUAL("test_assign_after_release", 0, std::strcmp(left.Get(), "right"));
	ASSERT_TRUE("test_assign_after_release", left.Get() != right.Get());
	RETURN_TEST("test_assign_after_release", result);
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
	// Reset / Release
	// -------------------
	result += test_reset_replaces();
	result += test_reset_from_null_pointer();
	result += test_release_transfers_and_requires_delete();
	result += test_release_null();
	result += test_reset_after_release();
	result += test_assign_after_release();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
