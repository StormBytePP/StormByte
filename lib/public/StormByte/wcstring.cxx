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

#include <StormByte/wcstring.hxx>

#include <cwchar>

using namespace StormByte;

const wchar_t* WCString::Duplicate(const wchar_t* str) noexcept {
	if (!str)
		return nullptr;
	const std::size_t len = std::wcslen(str) + 1;
	wchar_t* out = new wchar_t[len];
	std::wmemcpy(out, str, len);
	return out;
}

WCString::WCString() noexcept
: m_data(nullptr) {}

WCString::WCString(const wchar_t* str) noexcept
: m_data(Duplicate(str)) {}

WCString::WCString(const WCString& other) noexcept
: m_data(Duplicate(other.m_data)) {}

WCString::WCString(WCString&& other) noexcept
: m_data(other.m_data) {
	other.m_data = nullptr;
}

WCString::~WCString() noexcept {
	delete[] m_data;
	m_data = nullptr;
}

WCString& WCString::operator=(const WCString& other) noexcept {
	if (this != &other) {
		delete[] m_data;
		m_data = Duplicate(other.m_data);
	}
	return *this;
}

WCString& WCString::operator=(WCString&& other) noexcept {
	if (this != &other) {
		delete[] m_data;
		m_data = other.m_data;
		other.m_data = nullptr;
	}
	return *this;
}

void WCString::Reset(const wchar_t* str) noexcept {
	delete[] m_data;
	m_data = Duplicate(str);
}

std::size_t WCString::Length() const noexcept {
	return m_data ? std::wcslen(m_data) : 0;
}

void WCString::swap(WCString& other) noexcept {
	const wchar_t* tmp = m_data;
	m_data = other.m_data;
	other.m_data = tmp;
}

WCString::operator const wchar_t*() const noexcept {
	return m_data;
}

bool WCString::operator==(const WCString& other) const noexcept {
	if (m_data == other.m_data)
		return true;
	if (!m_data || !other.m_data)
		return false;
	return std::wcscmp(m_data, other.m_data) == 0;
}

bool WCString::operator==(const wchar_t* str) const noexcept {
	if (m_data == str)
		return true;
	if (!m_data || !str)
		return false;
	return std::wcscmp(m_data, str) == 0;
}

std::strong_ordering WCString::operator<=>(const WCString& other) const noexcept {
	if (!m_data && !other.m_data)
		return std::strong_ordering::equal;
	if (!m_data)
		return std::strong_ordering::less;
	if (!other.m_data)
		return std::strong_ordering::greater;
	const int cmp = std::wcscmp(m_data, other.m_data);
	if (cmp < 0)
		return std::strong_ordering::less;
	if (cmp > 0)
		return std::strong_ordering::greater;
	return std::strong_ordering::equal;
}

std::strong_ordering WCString::operator<=>(const wchar_t* str) const noexcept {
	if (!m_data && !str)
		return std::strong_ordering::equal;
	if (!m_data)
		return std::strong_ordering::less;
	if (!str)
		return std::strong_ordering::greater;
	const int cmp = std::wcscmp(m_data, str);
	if (cmp < 0)
		return std::strong_ordering::less;
	if (cmp > 0)
		return std::strong_ordering::greater;
	return std::strong_ordering::equal;
}
