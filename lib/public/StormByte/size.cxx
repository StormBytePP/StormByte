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

#include <array>
#include <cstdint>
#include <cstdio>

using namespace StormByte;

namespace {
	CString FormatByteSize(std::uint64_t bytes) noexcept {
		static constexpr std::array<const char*, 7> units{
			"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"
		};
		std::uint64_t value = bytes;
		std::size_t unit = 0;
		std::uint64_t remainder = 0;
		while (value >= 1024 && unit + 1 < units.size()) {
			remainder = value % 1024;
			value /= 1024;
			++unit;
		}

		char buffer[32];
		if (unit == 0 || remainder == 0)
			std::snprintf(buffer, sizeof(buffer), "%llu %s",
				static_cast<unsigned long long>(value), units[unit]);
		else {
			const unsigned tenths = static_cast<unsigned>((remainder * 10 + 512) / 1024);
			if (tenths == 0)
				std::snprintf(buffer, sizeof(buffer), "%llu %s",
					static_cast<unsigned long long>(value), units[unit]);
			else if (tenths == 10)
				std::snprintf(buffer, sizeof(buffer), "%llu %s",
					static_cast<unsigned long long>(value + 1), units[unit]);
			else
				std::snprintf(buffer, sizeof(buffer), "%llu.%u %s",
					static_cast<unsigned long long>(value), tenths, units[unit]);
		}
		return CString(buffer);
	}
}

template<Type::Integral T>
Size::Size(T value) noexcept: m_value(0) {
	if constexpr (Type::Signed<T>)
		assert(value >= T{0});
	m_value = static_cast<std::uint64_t>(value);
}

template<Type::Integral T>
Size StormByte::operator*(T count, Unit unit) noexcept {
	if constexpr (Type::Signed<T>)
		assert(count >= T{0});
	const std::uint64_t factor = static_cast<std::uint64_t>(count);
	assert(unit.factor == 0 || factor <= std::numeric_limits<std::uint64_t>::max() / unit.factor);
	return Size(factor * unit.factor);
}

template<Type::Integral T>
Size StormByte::operator*(Unit unit, T count) noexcept {
	return count * unit;
}

template<Type::FloatingPoint T>
Size StormByte::operator*(T count, Unit unit) noexcept {
	assert(count >= T{0});
	const long double product = static_cast<long double>(count) * static_cast<long double>(unit.factor);
	assert(product <= static_cast<long double>(std::numeric_limits<std::uint64_t>::max()));
	return Size(static_cast<std::uint64_t>(product + 0.5L));
}

template<Type::FloatingPoint T>
Size StormByte::operator*(Unit unit, T count) noexcept {
	return count * unit;
}

template<Type::Integral T>
Size StormByte::operator*(T count, const Size& size) noexcept {
	if constexpr (Type::Signed<T>)
		assert(count > T{0});
	else
		assert(count != T{0});
	const std::uint64_t factor = static_cast<std::uint64_t>(count);
	assert(size.Value() == 0 || factor <= std::numeric_limits<std::uint64_t>::max() / size.Value());
	return Size(factor * size.Value());
}

template<Type::Integral T>
Size StormByte::operator*(const Size& size, T count) noexcept {
	return count * size;
}

template<Type::Integral T>
std::uint64_t StormByte::operator/(const Size& size, T count) noexcept {
	if constexpr (Type::Signed<T>)
		assert(count > T{0});
	else
		assert(count != T{0});
	return size.Value() / static_cast<std::uint64_t>(count);
}

template<Type::Integral T>
std::uint64_t StormByte::operator%(const Size& size, T count) noexcept {
	if constexpr (Type::Signed<T>)
		assert(count > T{0});
	else
		assert(count != T{0});
	return size.Value() % static_cast<std::uint64_t>(count);
}

Size::operator CString() const noexcept {
	return FormatByteSize(m_value);
}

namespace StormByte {
	template Size::Size(bool) noexcept;
	template Size::Size(char) noexcept;
	template Size::Size(signed char) noexcept;
	template Size::Size(unsigned char) noexcept;
	template Size::Size(wchar_t) noexcept;
	template Size::Size(char8_t) noexcept;
	template Size::Size(char16_t) noexcept;
	template Size::Size(char32_t) noexcept;
	template Size::Size(short) noexcept;
	template Size::Size(unsigned short) noexcept;
	template Size::Size(int) noexcept;
	template Size::Size(unsigned) noexcept;
	template Size::Size(long) noexcept;
	template Size::Size(unsigned long) noexcept;
	template Size::Size(long long) noexcept;
	template Size::Size(unsigned long long) noexcept;

	template Size operator*<bool>(bool, Unit) noexcept;
	template Size operator*<char>(char, Unit) noexcept;
	template Size operator*<signed char>(signed char, Unit) noexcept;
	template Size operator*<unsigned char>(unsigned char, Unit) noexcept;
	template Size operator*<wchar_t>(wchar_t, Unit) noexcept;
	template Size operator*<char8_t>(char8_t, Unit) noexcept;
	template Size operator*<char16_t>(char16_t, Unit) noexcept;
	template Size operator*<char32_t>(char32_t, Unit) noexcept;
	template Size operator*<short>(short, Unit) noexcept;
	template Size operator*<unsigned short>(unsigned short, Unit) noexcept;
	template Size operator*<int>(int, Unit) noexcept;
	template Size operator*<unsigned>(unsigned, Unit) noexcept;
	template Size operator*<long>(long, Unit) noexcept;
	template Size operator*<unsigned long>(unsigned long, Unit) noexcept;
	template Size operator*<long long>(long long, Unit) noexcept;
	template Size operator*<unsigned long long>(unsigned long long, Unit) noexcept;

	template Size operator*<bool>(Unit, bool) noexcept;
	template Size operator*<char>(Unit, char) noexcept;
	template Size operator*<signed char>(Unit, signed char) noexcept;
	template Size operator*<unsigned char>(Unit, unsigned char) noexcept;
	template Size operator*<wchar_t>(Unit, wchar_t) noexcept;
	template Size operator*<char8_t>(Unit, char8_t) noexcept;
	template Size operator*<char16_t>(Unit, char16_t) noexcept;
	template Size operator*<char32_t>(Unit, char32_t) noexcept;
	template Size operator*<short>(Unit, short) noexcept;
	template Size operator*<unsigned short>(Unit, unsigned short) noexcept;
	template Size operator*<int>(Unit, int) noexcept;
	template Size operator*<unsigned>(Unit, unsigned) noexcept;
	template Size operator*<long>(Unit, long) noexcept;
	template Size operator*<unsigned long>(Unit, unsigned long) noexcept;
	template Size operator*<long long>(Unit, long long) noexcept;
	template Size operator*<unsigned long long>(Unit, unsigned long long) noexcept;

	template Size operator*<float>(float, Unit) noexcept;
	template Size operator*<double>(double, Unit) noexcept;
	template Size operator*<long double>(long double, Unit) noexcept;
	template Size operator*<float>(Unit, float) noexcept;
	template Size operator*<double>(Unit, double) noexcept;
	template Size operator*<long double>(Unit, long double) noexcept;

	template Size operator*<bool>(bool, const Size&) noexcept;
	template Size operator*<char>(char, const Size&) noexcept;
	template Size operator*<signed char>(signed char, const Size&) noexcept;
	template Size operator*<unsigned char>(unsigned char, const Size&) noexcept;
	template Size operator*<wchar_t>(wchar_t, const Size&) noexcept;
	template Size operator*<char8_t>(char8_t, const Size&) noexcept;
	template Size operator*<char16_t>(char16_t, const Size&) noexcept;
	template Size operator*<char32_t>(char32_t, const Size&) noexcept;
	template Size operator*<short>(short, const Size&) noexcept;
	template Size operator*<unsigned short>(unsigned short, const Size&) noexcept;
	template Size operator*<int>(int, const Size&) noexcept;
	template Size operator*<unsigned>(unsigned, const Size&) noexcept;
	template Size operator*<long>(long, const Size&) noexcept;
	template Size operator*<unsigned long>(unsigned long, const Size&) noexcept;
	template Size operator*<long long>(long long, const Size&) noexcept;
	template Size operator*<unsigned long long>(unsigned long long, const Size&) noexcept;

	template Size operator*<bool>(const Size&, bool) noexcept;
	template Size operator*<char>(const Size&, char) noexcept;
	template Size operator*<signed char>(const Size&, signed char) noexcept;
	template Size operator*<unsigned char>(const Size&, unsigned char) noexcept;
	template Size operator*<wchar_t>(const Size&, wchar_t) noexcept;
	template Size operator*<char8_t>(const Size&, char8_t) noexcept;
	template Size operator*<char16_t>(const Size&, char16_t) noexcept;
	template Size operator*<char32_t>(const Size&, char32_t) noexcept;
	template Size operator*<short>(const Size&, short) noexcept;
	template Size operator*<unsigned short>(const Size&, unsigned short) noexcept;
	template Size operator*<int>(const Size&, int) noexcept;
	template Size operator*<unsigned>(const Size&, unsigned) noexcept;
	template Size operator*<long>(const Size&, long) noexcept;
	template Size operator*<unsigned long>(const Size&, unsigned long) noexcept;
	template Size operator*<long long>(const Size&, long long) noexcept;
	template Size operator*<unsigned long long>(const Size&, unsigned long long) noexcept;

	template std::uint64_t operator/<bool>(const Size&, bool) noexcept;
	template std::uint64_t operator/<char>(const Size&, char) noexcept;
	template std::uint64_t operator/<signed char>(const Size&, signed char) noexcept;
	template std::uint64_t operator/<unsigned char>(const Size&, unsigned char) noexcept;
	template std::uint64_t operator/<wchar_t>(const Size&, wchar_t) noexcept;
	template std::uint64_t operator/<char8_t>(const Size&, char8_t) noexcept;
	template std::uint64_t operator/<char16_t>(const Size&, char16_t) noexcept;
	template std::uint64_t operator/<char32_t>(const Size&, char32_t) noexcept;
	template std::uint64_t operator/<short>(const Size&, short) noexcept;
	template std::uint64_t operator/<unsigned short>(const Size&, unsigned short) noexcept;
	template std::uint64_t operator/<int>(const Size&, int) noexcept;
	template std::uint64_t operator/<unsigned>(const Size&, unsigned) noexcept;
	template std::uint64_t operator/<long>(const Size&, long) noexcept;
	template std::uint64_t operator/<unsigned long>(const Size&, unsigned long) noexcept;
	template std::uint64_t operator/<long long>(const Size&, long long) noexcept;
	template std::uint64_t operator/<unsigned long long>(const Size&, unsigned long long) noexcept;

	template std::uint64_t operator%<bool>(const Size&, bool) noexcept;
	template std::uint64_t operator%<char>(const Size&, char) noexcept;
	template std::uint64_t operator%<signed char>(const Size&, signed char) noexcept;
	template std::uint64_t operator%<unsigned char>(const Size&, unsigned char) noexcept;
	template std::uint64_t operator%<wchar_t>(const Size&, wchar_t) noexcept;
	template std::uint64_t operator%<char8_t>(const Size&, char8_t) noexcept;
	template std::uint64_t operator%<char16_t>(const Size&, char16_t) noexcept;
	template std::uint64_t operator%<char32_t>(const Size&, char32_t) noexcept;
	template std::uint64_t operator%<short>(const Size&, short) noexcept;
	template std::uint64_t operator%<unsigned short>(const Size&, unsigned short) noexcept;
	template std::uint64_t operator%<int>(const Size&, int) noexcept;
	template std::uint64_t operator%<unsigned>(const Size&, unsigned) noexcept;
	template std::uint64_t operator%<long>(const Size&, long) noexcept;
	template std::uint64_t operator%<unsigned long>(const Size&, unsigned long) noexcept;
	template std::uint64_t operator%<long long>(const Size&, long long) noexcept;
	template std::uint64_t operator%<unsigned long long>(const Size&, unsigned long long) noexcept;
}
