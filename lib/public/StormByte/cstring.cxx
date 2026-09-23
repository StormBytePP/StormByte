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

void CString::Reset(const char* str) noexcept {
	delete[] m_data;
	m_data = Duplicate(str);
}

std::size_t CString::Length() const noexcept {
	return m_data ? std::strlen(m_data) : 0;
}

void CString::swap(CString& other) noexcept {
	const char* tmp = m_data;
	m_data = other.m_data;
	other.m_data = tmp;
}

CString::operator const char*() const noexcept {
	return m_data;
}

bool CString::operator==(const CString& other) const noexcept {
	if (m_data == other.m_data)
		return true;
	if (!m_data || !other.m_data)
		return false;
	return std::strcmp(m_data, other.m_data) == 0;
}

bool CString::operator==(const char* str) const noexcept {
	if (m_data == str)
		return true;
	if (!m_data || !str)
		return false;
	return std::strcmp(m_data, str) == 0;
}

std::strong_ordering CString::operator<=>(const CString& other) const noexcept {
	if (!m_data && !other.m_data)
		return std::strong_ordering::equal;
	if (!m_data)
		return std::strong_ordering::less;
	if (!other.m_data)
		return std::strong_ordering::greater;
	const int cmp = std::strcmp(m_data, other.m_data);
	if (cmp < 0)
		return std::strong_ordering::less;
	if (cmp > 0)
		return std::strong_ordering::greater;
	return std::strong_ordering::equal;
}

std::strong_ordering CString::operator<=>(const char* str) const noexcept {
	if (!m_data && !str)
		return std::strong_ordering::equal;
	if (!m_data)
		return std::strong_ordering::less;
	if (!str)
		return std::strong_ordering::greater;
	const int cmp = std::strcmp(m_data, str);
	if (cmp < 0)
		return std::strong_ordering::less;
	if (cmp > 0)
		return std::strong_ordering::greater;
	return std::strong_ordering::equal;
}
