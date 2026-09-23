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

#include <cstring>

using namespace StormByte;

const char* CString::Duplicate(const char* str) noexcept {
	if (!str)
		return nullptr;
	const std::size_t len = std::strlen(str) + 1;
	char* out = new char[len];
	std::memcpy(out, str, len);
	return out;
}

CString::CString() noexcept
: m_data(nullptr) {}

CString::CString(const char* str) noexcept
: m_data(Duplicate(str)) {}

CString::CString(const CString& other) noexcept
: m_data(Duplicate(other.m_data)) {}

CString::CString(CString&& other) noexcept
: m_data(other.m_data) {
	other.m_data = nullptr;
}

CString::~CString() noexcept {
	delete[] m_data;
	m_data = nullptr;
}

CString& CString::operator=(const CString& other) noexcept {
	if (this != &other) {
		delete[] m_data;
		m_data = Duplicate(other.m_data);
	}
	return *this;
}

CString& CString::operator=(CString&& other) noexcept {
	if (this != &other) {
		delete[] m_data;
		m_data = other.m_data;
		other.m_data = nullptr;
	}
	return *this;
}

const char* CString::Get() const noexcept {
	return m_data;
}

const char* CString::Release() noexcept {
	const char* out = m_data;
	m_data = nullptr;
	return out;
}

void CString::Reset(const char* str) noexcept {
	delete[] m_data;
	m_data = Duplicate(str);
}
