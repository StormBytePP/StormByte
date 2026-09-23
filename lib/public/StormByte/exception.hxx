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

#pragma once

#include <StormByte/cstring.hxx>
#include <StormByte/visibility.h>

#include <format>
#include <string>
#include <string_view>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @struct Component
	 * @brief Wraps a module name for the component-prefixed `Exception` constructor.
	 *
	 * Exists only to disambiguate `Exception(component, fmt, args...)` from
	 * `Exception(fmt, args...)`: with both taking a leading string-like
	 * parameter, overload resolution would otherwise silently prefer the
	 * fmt-only constructor and discard the component and message.
	 */
	struct Component {
		std::string_view name;	///< Module name inserted after `StormByte::`.

		/**
		 * @brief Wraps @p name.
		 * @param name Module name.
		 */
		constexpr explicit Component(std::string_view name) noexcept: name(name) {}
	};

	/**
	 * @class Exception
	 * @brief Base exception type for the suite.
	 *
	 * The message is a @ref CString so the text does not cross a DLL
	 * boundary as `std::string`.
	 */
	class STORMBYTE_PUBLIC Exception {
	public:
		/**
		 * @brief Constructs from a string.
		 * @param message Exception text.
		 */
		explicit Exception(const std::string& message);

		/**
		 * @brief Constructs from a moved string.
		 * @param message Exception text.
		 */
		explicit Exception(std::string&& message);

		/**
		 * @brief Constructs with `std::format`.
		 * @tparam Args Format argument types.
		 * @param fmt Format string.
		 * @param args Format arguments.
		 * @note With zero arguments the format string is the message as-is.
		 */
		template <typename... Args>
		Exception(std::format_string<Args...> fmt, Args&&... args) {
			if constexpr (sizeof...(Args) == 0) {
				m_what.Reset(std::string(fmt.get()).c_str());
			} else {
				const std::string formatted = std::format(fmt, std::forward<Args>(args)...);
				m_what.Reset(formatted.c_str());
			}
		}

		/**
		 * @brief Constructs with a component prefix and `std::format`.
		 * @tparam Args Format argument types.
		 * @param component Module name, wrapped so it cannot be mistaken for `fmt`.
		 * @param fmt Format string.
		 * @param args Format arguments.
		 * @note Final text is `StormByte::<component>: <formatted>`.
		 */
		template <typename... Args>
		Exception(Component component, std::format_string<Args...> fmt, Args&&... args) {
			const std::string formatted = std::format(fmt, std::forward<Args>(args)...);
			const std::string full = "StormByte::" + std::string(component.name) + ": " + formatted;
			m_what.Reset(full.c_str());
		}

		Exception(const Exception& e) = default;
		Exception(Exception&& e) noexcept = default;
		virtual ~Exception() noexcept = default;
		Exception& operator=(const Exception& e) = default;
		Exception& operator=(Exception&& e) noexcept = default;

		/**
		 * @brief Message pointer.
		 * @return NUL-terminated message owned by this object.
		 */
		virtual const char* what() const noexcept;

	private:
		CString m_what;	///< Owned message
	};

	/**
	 * @class DeserializeError
	 * @brief Thrown when deserialization fails.
	 */
	class STORMBYTE_PUBLIC DeserializeError: public Exception {
		public:
			using Exception::Exception;
	};

	/**
	 * @class OutOfBoundsError
	 * @brief Thrown when an index or range is out of bounds.
	 */
	class STORMBYTE_PUBLIC OutOfBoundsError: public Exception {
		public:
			using Exception::Exception;
	};

	/**
	 * @class Base64Error
	 * @brief Thrown when Base64 encode or decode fails.
	 */
	class STORMBYTE_PUBLIC Base64Error: public Exception {
		public:
			using Exception::Exception;
	};

	/**
	 * @class UTF8Error
	 * @brief Thrown when UTF-8 or wide-string input contains invalid Unicode.
	 */
	class STORMBYTE_PUBLIC UTF8Error: public Exception {
		public:
			using Exception::Exception;
	};

	/**
	 * @class SystemError
	 * @brief Thrown when a System helper cannot obtain a path or create a temporary file.
	 */
	class STORMBYTE_PUBLIC SystemError: public Exception {
		public:
			using Exception::Exception;
	};
}
