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

#include <StormByte/exception.hxx>
#include <cstring>
using namespace StormByte;
Exception::Exception(const std::string& message):m_what(copy_str(message.c_str())) {}
Exception::Exception(std::string&& message):m_what(copy_str(message.c_str())) {}
Exception::Exception(const Exception& e):m_what(copy_str(e.m_what)) {}
Exception::Exception(Exception&& e) noexcept:m_what(e.m_what) {
	e.m_what = nullptr;
}
Exception& Exception::operator=(const Exception& e) {
	if (this != &e) {
		free_str();
		m_what = copy_str(e.m_what);
	}
	return *this;
}
Exception& Exception::operator=(Exception&& e) noexcept {
	if (this != &e) {
		free_str();
		m_what = e.m_what;
		e.m_what = nullptr;
	}
	return *this;
}
Exception::~Exception() noexcept {
	free_str();
}
const char* Exception::what() const noexcept {
	return m_what;
}
const char* Exception::copy_str(const char* str) noexcept {
	std::size_t len = std::strlen(str) + 1;
	char* out = new char[len];
	std::memcpy(out, str, len);
	return out;
}
void Exception::free_str() noexcept {
	if (m_what != nullptr) {
		delete[] m_what;
		m_what = nullptr;
	}
}
