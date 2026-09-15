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

#include <StormByte/clonable.hxx>
#include <StormByte/error.hxx>
#include <StormByte/test_handlers.h>
#include <memory>
#include <string>
using namespace StormByte;

namespace {
	class TestCloneable: public Clonable<TestCloneable> {
		public:
			explicit TestCloneable(int value): value(value) {}

			PointerType Clone() const override {
				return MakePointer<TestCloneable>(value);
			}

			PointerType Move() override {
				return MakePointer<TestCloneable>(std::move(*this));
			}

			int value;
	};
}

int test_clonable_copy_and_move() {
	int result = 0;
	TestCloneable original(42);
	auto clone = original.Clone();
	ASSERT_TRUE("test_clonable_copy_and_move", clone != nullptr);
	ASSERT_EQUAL("test_clonable_copy_and_move", 42, clone->value);
	auto moved = original.Move();
	ASSERT_TRUE("test_clonable_copy_and_move", moved != nullptr);
	ASSERT_EQUAL("test_clonable_copy_and_move", 42, moved->value);
	RETURN_TEST("test_clonable_copy_and_move", result);
}

int test_error_code_integration() {
	int result = 0;
	const auto code = make_error_code(static_cast<Error::Code>(0));
	ASSERT_TRUE("test_error_code_integration", code.category() == Error::category());
	ASSERT_EQUAL("test_error_code_integration", std::string("StormByte Error"), std::string(code.category().name()));
	ASSERT_EQUAL("test_error_code_integration", std::string("Unknown StormByte error"), code.message());
	RETURN_TEST("test_error_code_integration", result);
}

int main() {
	int result = 0;
	result += test_clonable_copy_and_move();
	result += test_error_code_integration();
	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}

	return result;
}
