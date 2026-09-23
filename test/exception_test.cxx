/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte source in this repository.
 * They do not cover other StormByte modules or any third-party material
 * shipped with this repository (including everything under thirdparty/),
 * which remains under its own license.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#include <StormByte/exception.hxx>
#include <StormByte/test_handlers.h>

#include <string>
#include <string_view>
#include <utility>

using namespace StormByte;

// -------------------
// Construct
// -------------------

int test_plain_message_no_args() {
	int result = 0;
	Exception e("Key not found in Iterable::operator[]");
	ASSERT_EQUAL("test_plain_message_no_args", std::string("Key not found in Iterable::operator[]"), std::string(e.what()));
	RETURN_TEST("test_plain_message_no_args", result);
}

int test_construct_from_lvalue_string() {
	int result = 0;
	const std::string message("from lvalue");
	Exception e(message);
	ASSERT_EQUAL("test_construct_from_lvalue_string", std::string("from lvalue"), std::string(e.what()));
	ASSERT_EQUAL("test_construct_from_lvalue_string", std::string("from lvalue"), message);
	RETURN_TEST("test_construct_from_lvalue_string", result);
}

int test_formatted_message_with_args() {
	int result = 0;
	Exception e("value is {}", 42);
	ASSERT_EQUAL("test_formatted_message_with_args", std::string("value is 42"), std::string(e.what()));
	RETURN_TEST("test_formatted_message_with_args", result);
}

int test_zero_args_format_string_ctor_is_as_is() {
	int result = 0;
	constexpr std::string_view sv = "literal {{brace}} as-is";
	Exception e(sv);
	ASSERT_EQUAL("test_zero_args_format_string_ctor_is_as_is", std::string("literal {{brace}} as-is"), std::string(e.what()));
	RETURN_TEST("test_zero_args_format_string_ctor_is_as_is", result);
}

int test_component_prefixed_message() {
	int result = 0;
	Exception e(Component("MyComponent"), "failed with code {}", 7);
	ASSERT_EQUAL("test_component_prefixed_message", std::string("StormByte::MyComponent: failed with code 7"), std::string(e.what()));
	RETURN_TEST("test_component_prefixed_message", result);
}

int test_component_prefixed_message_no_args() {
	int result = 0;
	Exception e(Component("MyComponent"), "plain text");
	ASSERT_EQUAL("test_component_prefixed_message_no_args", std::string("StormByte::MyComponent: plain text"), std::string(e.what()));
	RETURN_TEST("test_component_prefixed_message_no_args", result);
}

// -------------------
// Copy / move
// -------------------

int test_copy_keeps_what() {
	int result = 0;
	Exception original("payload");
	Exception copy(original);
	ASSERT_EQUAL("test_copy_keeps_what", std::string("payload"), std::string(copy.what()));
	ASSERT_EQUAL("test_copy_keeps_what", std::string("payload"), std::string(original.what()));
	RETURN_TEST("test_copy_keeps_what", result);
}

int test_move_keeps_what() {
	int result = 0;
	Exception original("payload");
	Exception taken(std::move(original));
	ASSERT_EQUAL("test_move_keeps_what", std::string("payload"), std::string(taken.what()));
	RETURN_TEST("test_move_keeps_what", result);
}

// -------------------
// Derived
// -------------------

int test_derived_are_exceptions() {
	int result = 0;
	try {
		throw Base64Error("bad alphabet");
	} catch (const Exception& e) {
		ASSERT_EQUAL("test_derived_are_exceptions", std::string("bad alphabet"), std::string(e.what()));
	}
	try {
		throw OutOfBoundsError("index");
	} catch (const Exception& e) {
		ASSERT_EQUAL("test_derived_are_exceptions", std::string("index"), std::string(e.what()));
	}
	try {
		throw DeserializeError("wire");
	} catch (const Exception& e) {
		ASSERT_EQUAL("test_derived_are_exceptions", std::string("wire"), std::string(e.what()));
	}
	try {
		throw UTF8Error("bad utf8");
	} catch (const Exception& e) {
		ASSERT_EQUAL("test_derived_are_exceptions", std::string("bad utf8"), std::string(e.what()));
	}
	try {
		throw SystemError("path");
	} catch (const Exception& e) {
		ASSERT_EQUAL("test_derived_are_exceptions", std::string("path"), std::string(e.what()));
	}
	RETURN_TEST("test_derived_are_exceptions", result);
}

int main() {
	int result = 0;

	// -------------------
	// Construct
	// -------------------
	result += test_plain_message_no_args();
	result += test_construct_from_lvalue_string();
	result += test_formatted_message_with_args();
	result += test_zero_args_format_string_ctor_is_as_is();
	result += test_component_prefixed_message();
	result += test_component_prefixed_message_no_args();

	// -------------------
	// Copy / move
	// -------------------
	result += test_copy_keeps_what();
	result += test_move_keeps_what();

	// -------------------
	// Derived
	// -------------------
	result += test_derived_are_exceptions();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
