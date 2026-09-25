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

Size::operator CString() const noexcept {
	return FormatByteSize(m_value);
}
