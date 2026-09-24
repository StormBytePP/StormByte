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

#include <StormByte/size.hxx>
#include <StormByte/test_handlers.h>

#include <string>

using namespace StormByte;

// -------------------
// Add
// -------------------

int test_add_and_sub() {
	int result = 0;
	const Size left{100};
	const Size right{40};
	ASSERT_EQUAL("test_add_and_sub", 140ull, (left + right).Value());
	ASSERT_EQUAL("test_add_and_sub", 60ull, (left - right).Value());
	Size acc{10};
	acc += Size{5};
	ASSERT_EQUAL("test_add_and_sub", 15ull, acc.Value());
	acc -= Size{3};
	ASSERT_EQUAL("test_add_and_sub", 12ull, acc.Value());
	RETURN_TEST("test_add_and_sub", result);
}

int test_add_mixed_integer() {
	int result = 0;
	const Size size{10};
	ASSERT_EQUAL("test_add_mixed_integer", 26ull, (size + 16).Value());
	ASSERT_EQUAL("test_add_mixed_integer", 26ull, (16 + size).Value());
	ASSERT_EQUAL("test_add_mixed_integer", 26ull, (size + 16u).Value());
	ASSERT_EQUAL("test_add_mixed_integer", 26ull, (16u + size).Value());
	ASSERT_EQUAL("test_add_mixed_integer", 26ull, (size + 16ull).Value());
	ASSERT_EQUAL("test_add_mixed_integer", 26ull, (16ull + size).Value());
	ASSERT_EQUAL("test_add_mixed_integer", 6ull, (size - 4).Value());
	ASSERT_EQUAL("test_add_mixed_integer", 6ull, (size - 4u).Value());
	ASSERT_EQUAL("test_add_mixed_integer", 0ull, (size - 10).Value());
	Size acc = 10;
	acc += 5;
	ASSERT_EQUAL("test_add_mixed_integer", 15ull, acc.Value());
	acc += 5u;
	ASSERT_EQUAL("test_add_mixed_integer", 20ull, acc.Value());
	acc -= 3;
	ASSERT_EQUAL("test_add_mixed_integer", 17ull, acc.Value());
	acc -= 1ull;
	ASSERT_EQUAL("test_add_mixed_integer", 16ull, acc.Value());
	RETURN_TEST("test_add_mixed_integer", result);
}

// -------------------
// Compare
// -------------------

int test_compare_equal_and_order() {
	int result = 0;
	const Size a{10};
	const Size b{10};
	const Size c{11};
	ASSERT_TRUE("test_compare_equal_and_order", a == b);
	ASSERT_TRUE("test_compare_equal_and_order", a != c);
	ASSERT_TRUE("test_compare_equal_and_order", a < c);
	ASSERT_TRUE("test_compare_equal_and_order", c > a);
	ASSERT_TRUE("test_compare_equal_and_order", a <= b);
	ASSERT_TRUE("test_compare_equal_and_order", c >= a);
	RETURN_TEST("test_compare_equal_and_order", result);
}

int test_compare_mixed_integer() {
	int result = 0;
	const Size size{5};
	ASSERT_TRUE("test_compare_mixed_integer", size == 5);
	ASSERT_TRUE("test_compare_mixed_integer", 5 == size);
	ASSERT_TRUE("test_compare_mixed_integer", size == 5u);
	ASSERT_TRUE("test_compare_mixed_integer", 5u == size);
	ASSERT_TRUE("test_compare_mixed_integer", size == 5ull);
	ASSERT_TRUE("test_compare_mixed_integer", 5ull == size);
	ASSERT_TRUE("test_compare_mixed_integer", size != 6);
	ASSERT_TRUE("test_compare_mixed_integer", 6 != size);
	ASSERT_TRUE("test_compare_mixed_integer", size != 6u);
	ASSERT_TRUE("test_compare_mixed_integer", 6u != size);
	ASSERT_TRUE("test_compare_mixed_integer", size < 6);
	ASSERT_TRUE("test_compare_mixed_integer", 4 < size);
	ASSERT_TRUE("test_compare_mixed_integer", size < 6u);
	ASSERT_TRUE("test_compare_mixed_integer", 4u < size);
	ASSERT_TRUE("test_compare_mixed_integer", size > 0);
	ASSERT_TRUE("test_compare_mixed_integer", 10 > size);
	ASSERT_TRUE("test_compare_mixed_integer", size > 0u);
	ASSERT_TRUE("test_compare_mixed_integer", 10u > size);
	ASSERT_TRUE("test_compare_mixed_integer", 7 > size);
	ASSERT_TRUE("test_compare_mixed_integer", size < 7);
	ASSERT_TRUE("test_compare_mixed_integer", size <= 5);
	ASSERT_TRUE("test_compare_mixed_integer", 5 <= size);
	ASSERT_TRUE("test_compare_mixed_integer", size <= 6);
	ASSERT_TRUE("test_compare_mixed_integer", 4 <= size);
	ASSERT_TRUE("test_compare_mixed_integer", size >= 5);
	ASSERT_TRUE("test_compare_mixed_integer", 5 >= size);
	ASSERT_TRUE("test_compare_mixed_integer", size >= 0);
	ASSERT_TRUE("test_compare_mixed_integer", 9 >= size);
	ASSERT_TRUE("test_compare_mixed_integer", size == 0 || size != 0);
	const Size empty;
	ASSERT_TRUE("test_compare_mixed_integer", empty == 0);
	ASSERT_TRUE("test_compare_mixed_integer", 0 == empty);
	ASSERT_TRUE("test_compare_mixed_integer", empty != 1);
	ASSERT_TRUE("test_compare_mixed_integer", 1 != empty);
	ASSERT_TRUE("test_compare_mixed_integer", empty < 1);
	ASSERT_TRUE("test_compare_mixed_integer", 1 > empty);
	RETURN_TEST("test_compare_mixed_integer", result);
}

// -------------------
// Construct
// -------------------

int test_construct_assign_integer() {
	int result = 0;
	Size size;
	size = 0;
	ASSERT_EQUAL("test_construct_assign_integer", 0ull, size.Value());
	size = 16;
	ASSERT_EQUAL("test_construct_assign_integer", 16ull, size.Value());
	size = 32u;
	ASSERT_EQUAL("test_construct_assign_integer", 32ull, size.Value());
	size = 64ull;
	ASSERT_EQUAL("test_construct_assign_integer", 64ull, size.Value());
	size = 0u;
	ASSERT_EQUAL("test_construct_assign_integer", 0ull, size.Value());
	RETURN_TEST("test_construct_assign_integer", result);
}

int test_construct_copy_move() {
	int result = 0;
	const Size original{64};
	const Size copied{original};
	Size moved{Size{32}};
	ASSERT_EQUAL("test_construct_copy_move", 64ull, copied.Value());
	ASSERT_EQUAL("test_construct_copy_move", 32ull, moved.Value());
	RETURN_TEST("test_construct_copy_move", result);
}

int test_construct_from_unit_expression() {
	int result = 0;
	const Size size{100 * GiB + 20 * KiB};
	ASSERT_EQUAL("test_construct_from_unit_expression",
		100ull * GiB.factor + 20ull * KiB.factor, size.Value());
	RETURN_TEST("test_construct_from_unit_expression", result);
}

int test_construct_implicit_from_int() {
	int result = 0;
	const Size from_zero = 0;
	ASSERT_EQUAL("test_construct_implicit_from_int", 0ull, from_zero.Value());
	const Size from_int = 100;
	ASSERT_EQUAL("test_construct_implicit_from_int", 100ull, from_int.Value());
	const Size from_unsigned = 100u;
	ASSERT_EQUAL("test_construct_implicit_from_int", 100ull, from_unsigned.Value());
	const Size from_ull = 100ull;
	ASSERT_EQUAL("test_construct_implicit_from_int", 100ull, from_ull.Value());
	const Size from_long = 100L;
	ASSERT_EQUAL("test_construct_implicit_from_int", 100ull, from_long.Value());
	RETURN_TEST("test_construct_implicit_from_int", result);
}

int test_construct_int_literal() {
	int result = 0;
	const Size size{100};
	ASSERT_EQUAL("test_construct_int_literal", 100ull, size.Value());
	RETURN_TEST("test_construct_int_literal", result);
}

int test_construct_unsigned_and_ull() {
	int result = 0;
	const Size from_unsigned{100u};
	const Size from_ull{100ull};
	ASSERT_EQUAL("test_construct_unsigned_and_ull", 100ull, from_unsigned.Value());
	ASSERT_EQUAL("test_construct_unsigned_and_ull", 100ull, from_ull.Value());
	RETURN_TEST("test_construct_unsigned_and_ull", result);
}

int test_construct_zero() {
	int result = 0;
	const Size empty;
	ASSERT_EQUAL("test_construct_zero", 0ull, empty.Value());
	RETURN_TEST("test_construct_zero", result);
}

// -------------------
// Convert
// -------------------

int test_convert_cstring_and_string() {
	int result = 0;
	const Size size{1024};
	const CString owned = static_cast<CString>(size);
	const std::string text = size;
	ASSERT_EQUAL("test_convert_cstring_and_string", "1 KiB", std::string(owned));
	ASSERT_EQUAL("test_convert_cstring_and_string", "1 KiB", text);
	RETURN_TEST("test_convert_cstring_and_string", result);
}

int test_convert_human_readable_bytes_and_fraction() {
	int result = 0;
	ASSERT_EQUAL("test_convert_human_readable_bytes_and_fraction", "0 B", std::string(Size{}));
	ASSERT_EQUAL("test_convert_human_readable_bytes_and_fraction", "512 B", std::string(Size{512}));
	ASSERT_EQUAL("test_convert_human_readable_bytes_and_fraction", "1 KiB", std::string(Size{1024}));
	ASSERT_EQUAL("test_convert_human_readable_bytes_and_fraction", "1.5 KiB", std::string(Size{1536}));
	ASSERT_EQUAL("test_convert_human_readable_bytes_and_fraction", "1 MiB", std::string(1 * MiB));
	RETURN_TEST("test_convert_human_readable_bytes_and_fraction", result);
}

int test_convert_size_t_and_ptrdiff() {
	int result = 0;
	const Size size{42};
	ASSERT_EQUAL("test_convert_size_t_and_ptrdiff", static_cast<std::size_t>(42), static_cast<std::size_t>(size));
	ASSERT_EQUAL("test_convert_size_t_and_ptrdiff", static_cast<std::ptrdiff_t>(42), static_cast<std::ptrdiff_t>(size));
	const Size empty;
	ASSERT_EQUAL("test_convert_size_t_and_ptrdiff", static_cast<std::size_t>(0), static_cast<std::size_t>(empty));
	ASSERT_EQUAL("test_convert_size_t_and_ptrdiff", static_cast<std::ptrdiff_t>(0), static_cast<std::ptrdiff_t>(empty));
	RETURN_TEST("test_convert_size_t_and_ptrdiff", result);
}

int test_convert_value_and_uint64() {
	int result = 0;
	const Size size{42};
	ASSERT_EQUAL("test_convert_value_and_uint64", 42ull, size.Value());
	ASSERT_EQUAL("test_convert_value_and_uint64", 42ull, static_cast<std::uint64_t>(size));
	RETURN_TEST("test_convert_value_and_uint64", result);
}

// -------------------
// Scale
// -------------------

int test_scale_size_integer() {
	int result = 0;
	const Size block{1 * KiB};
	ASSERT_EQUAL("test_scale_size_integer", 4096ull, (4 * block).Value());
	ASSERT_EQUAL("test_scale_size_integer", 4096ull, (block * 4).Value());
	ASSERT_EQUAL("test_scale_size_integer", 8192ull, (block * 8u).Value());
	ASSERT_EQUAL("test_scale_size_integer", 1024ull, (1 * block).Value());
	RETURN_TEST("test_scale_size_integer", result);
}

// -------------------
// Split
// -------------------

int test_split_div_and_mod() {
	int result = 0;
	const Size total{10};
	ASSERT_EQUAL("test_split_div_and_mod", 2ull, total / 4);
	ASSERT_EQUAL("test_split_div_and_mod", 2ull, total % 4);
	ASSERT_EQUAL("test_split_div_and_mod", 3ull, total / 3);
	ASSERT_EQUAL("test_split_div_and_mod", 1ull, total % 3);
	ASSERT_EQUAL("test_split_div_and_mod", 10ull, total / 1);
	ASSERT_EQUAL("test_split_div_and_mod", 0ull, total % 1);
	RETURN_TEST("test_split_div_and_mod", result);
}

int test_split_exact_and_unsigned() {
	int result = 0;
	const Size block{4 * KiB};
	ASSERT_EQUAL("test_split_exact_and_unsigned", 4ull, block / 1024);
	ASSERT_EQUAL("test_split_exact_and_unsigned", 0ull, block % 1024);
	ASSERT_EQUAL("test_split_exact_and_unsigned", 4ull, block / 1024u);
	ASSERT_EQUAL("test_split_exact_and_unsigned", 0ull, block % 1024ull);
	RETURN_TEST("test_split_exact_and_unsigned", result);
}

int test_split_zero_size() {
	int result = 0;
	const Size empty;
	ASSERT_EQUAL("test_split_zero_size", 0ull, empty / 4);
	ASSERT_EQUAL("test_split_zero_size", 0ull, empty % 4);
	RETURN_TEST("test_split_zero_size", result);
}

// -------------------
// Unit
// -------------------

int test_unit_iec_factors() {
	int result = 0;
	ASSERT_EQUAL("test_unit_iec_factors", 1ull, B.factor);
	ASSERT_EQUAL("test_unit_iec_factors", 1024ull, KiB.factor);
	ASSERT_EQUAL("test_unit_iec_factors", 1024ull * 1024, MiB.factor);
	ASSERT_EQUAL("test_unit_iec_factors", 1024ull * 1024 * 1024, GiB.factor);
	ASSERT_EQUAL("test_unit_iec_factors", 1024ull * 1024 * 1024 * 1024, TiB.factor);
	ASSERT_EQUAL("test_unit_iec_factors", 1024ull * 1024 * 1024 * 1024 * 1024, PiB.factor);
	ASSERT_EQUAL("test_unit_iec_factors", 1024ull * 1024 * 1024 * 1024 * 1024 * 1024, EiB.factor);
	RETURN_TEST("test_unit_iec_factors", result);
}

int test_unit_iec_not_si() {
	int result = 0;
	ASSERT_TRUE("test_unit_iec_not_si", (1 * KiB) != (1 * KB));
	ASSERT_TRUE("test_unit_iec_not_si", (1 * KiB) > (1 * KB));
	RETURN_TEST("test_unit_iec_not_si", result);
}

int test_unit_scale_float() {
	int result = 0;
	ASSERT_EQUAL("test_unit_scale_float", 1536ull, (1.5 * KiB).Value());
	ASSERT_EQUAL("test_unit_scale_float", 1536ull, (KiB * 1.5).Value());
	ASSERT_EQUAL("test_unit_scale_float", 4301ull, (4.2 * KiB).Value());
	ASSERT_EQUAL("test_unit_scale_float", 2ull * MiB.factor, (2.0 * MiB).Value());
	RETURN_TEST("test_unit_scale_float", result);
}

int test_unit_scale_integer() {
	int result = 0;
	ASSERT_EQUAL("test_unit_scale_integer", 4096ull, (4 * KiB).Value());
	ASSERT_EQUAL("test_unit_scale_integer", 4096ull, (KiB * 4).Value());
	ASSERT_EQUAL("test_unit_scale_integer", 4000ull, (4 * KB).Value());
	ASSERT_EQUAL("test_unit_scale_integer", 2ull * MiB.factor, (2u * MiB).Value());
	ASSERT_EQUAL("test_unit_scale_integer", 3ull * GiB.factor, (GiB * 3ull).Value());
	RETURN_TEST("test_unit_scale_integer", result);
}

int test_unit_si_factors() {
	int result = 0;
	ASSERT_EQUAL("test_unit_si_factors", 1000ull, KB.factor);
	ASSERT_EQUAL("test_unit_si_factors", 1000ull * 1000, MB.factor);
	ASSERT_EQUAL("test_unit_si_factors", 1000ull * 1000 * 1000, GB.factor);
	ASSERT_EQUAL("test_unit_si_factors", 1000ull * 1000 * 1000 * 1000, TB.factor);
	ASSERT_EQUAL("test_unit_si_factors", 1000ull * 1000 * 1000 * 1000 * 1000, PB.factor);
	ASSERT_EQUAL("test_unit_si_factors", 1000ull * 1000 * 1000 * 1000 * 1000 * 1000, EB.factor);
	RETURN_TEST("test_unit_si_factors", result);
}

int main() {
	int result = 0;

	// -------------------
	// Add
	// -------------------
	result += test_add_and_sub();
	result += test_add_mixed_integer();

	// -------------------
	// Compare
	// -------------------
	result += test_compare_equal_and_order();
	result += test_compare_mixed_integer();

	// -------------------
	// Construct
	// -------------------
	result += test_construct_assign_integer();
	result += test_construct_copy_move();
	result += test_construct_from_unit_expression();
	result += test_construct_implicit_from_int();
	result += test_construct_int_literal();
	result += test_construct_unsigned_and_ull();
	result += test_construct_zero();

	// -------------------
	// Convert
	// -------------------
	result += test_convert_cstring_and_string();
	result += test_convert_human_readable_bytes_and_fraction();
	result += test_convert_size_t_and_ptrdiff();
	result += test_convert_value_and_uint64();

	// -------------------
	// Scale
	// -------------------
	result += test_scale_size_integer();

	// -------------------
	// Split
	// -------------------
	result += test_split_div_and_mod();
	result += test_split_exact_and_unsigned();
	result += test_split_zero_size();

	// -------------------
	// Unit
	// -------------------
	result += test_unit_iec_factors();
	result += test_unit_iec_not_si();
	result += test_unit_scale_float();
	result += test_unit_scale_integer();
	result += test_unit_si_factors();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
