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

#include <StormByte/error.txx>

#include <format>

using namespace StormByte;

const Error::Category<Error::Code>& Error::category() noexcept {
	static Category<Code> instance;
	return instance;
}

std::error_code Error::make_error_code(Error::Code e) noexcept {
	return std::error_code(static_cast<int>(e), Error::category());
}

Error::Fault::Fault() noexcept
: Fault(make_error_code(Error::Code::Success)) {}

Error::Fault::Fault(const std::error_code& code)
: m_code(code), m_what(std::format("{}: {}", code.category().name(), code.message()).c_str()) {}

const std::error_code& Error::Fault::code() const noexcept {
	return m_code;
}

const char* Error::Fault::what() const noexcept {
	const char* text = m_what.Get();
	return text ? text : "";
}

Error::Fault::operator bool() const noexcept {
	return static_cast<bool>(m_code);
}
