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

#include <StormByte/error.hxx>

using namespace StormByte;

const char* Error::Category::name() const noexcept {
	return "StormByte Error";
}

std::string Error::Category::message(int ev) const {
	switch(static_cast<Error::Code>(ev)) {
		default:
			return "Unknown StormByte error";
	}
}

std::error_condition Error::Category::default_error_condition(int ev) const noexcept {
	switch(static_cast<Error::Code>(ev)) {
		default:
			return std::error_condition(ev, *this);
	}
}

const Error::Category& Error::category() noexcept {
	static Category instance;
	return instance;
}

std::error_code StormByte::make_error_code(Error::Code e) {
	return std::error_code(static_cast<int>(e), Error::category());
}
