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

#pragma once

#include <StormByte/visibility.h>

#include <ostream>
#include <string>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @class WCString
	 * @brief Owned NUL-terminated wide buffer that may cross a DLL boundary.
	 *
	 * Wide counterpart of @ref CString. Not a replacement of `std::wstring`.
	 * The class is minimal: copy, move, reset, a C wide-string view,
	 * `Length`, and conversions. There is no append, find, iterator or
	 * allocator API.
	 *
	 * Allocation and free run inside the StormByte DLL.
	 *
	 * `operator const wchar_t*` is the analogue of `std::wstring::c_str()`.
	 * The pointer is valid only until this object is destroyed, moved
	 * from, assigned or @ref Reset. Using it afterwards is use-after-free.
	 *
	 * `operator std::wstring` and `operator<<` are inline so the
	 * `std::wstring` / stream write run in the caller’s translation unit.
	 */
	class STORMBYTE_PUBLIC WCString {
		public:
			/**
			 * @brief Empty (null) buffer.
			 */
			WCString() noexcept;

			/**
			 * @brief Copies @p str.
			 * @param str Source; may be null.
			 */
			explicit WCString(const wchar_t* str) noexcept;

			/**
			 * @brief Copy constructor.
			 * @param other Buffer to copy.
			 */
			WCString(const WCString& other) noexcept;

			/**
			 * @brief Move constructor.
			 * @param other Buffer to take.
			 */
			WCString(WCString&& other) noexcept;

			/**
			 * @brief Releases the buffer.
			 */
			~WCString() noexcept;

			/**
			 * @brief Copy assignment.
			 * @param other Buffer to copy.
			 * @return *this.
			 */
			WCString& operator=(const WCString& other) noexcept;

			/**
			 * @brief Move assignment.
			 * @param other Buffer to take.
			 * @return *this.
			 */
			WCString& operator=(WCString&& other) noexcept;

			/**
			 * @brief Replaces the buffer with a copy of @p str.
			 * @param str Source; may be null.
			 */
			void Reset(const wchar_t* str = nullptr) noexcept;

			/**
			 * @brief Character count (`wcslen`), or `0` when empty.
			 * @return Length.
			 */
			std::size_t Length() const noexcept;

			/**
			 * @brief View of the owned buffer.
			 * @return Buffer, or null.
			 * @note Same lifetime rules as `std::wstring::c_str()`.
			 */
			explicit operator const wchar_t*() const noexcept;

			/**
			 * @brief Copy of the text in the caller’s heap.
			 * @return Empty string when the buffer is null.
			 */
			inline operator std::wstring() const {
				const wchar_t* text = static_cast<const wchar_t*>(*this);
				return text ? std::wstring(text) : std::wstring();
			}

			/**
			 * @brief Writes the text to @p stream.
			 * @param stream Destination.
			 * @return @p stream.
			 */
			inline std::wostream& operator<<(std::wostream& stream) const {
				const wchar_t* text = static_cast<const wchar_t*>(*this);
				if (text)
					stream << text;
				return stream;
			}

		private:
			const wchar_t* m_data;	///< Owned buffer

			/**
			 * @brief Copies @p str into a new buffer.
			 * @param str Source; may be null.
			 * @return New buffer, or null.
			 */
			static const wchar_t* Duplicate(const wchar_t* str) noexcept;
	};

	/**
	 * @brief Writes @p text to @p stream.
	 * @param stream Destination.
	 * @param text Source.
	 * @return @p stream.
	 */
	inline std::wostream& operator<<(std::wostream& stream, const WCString& text) {
		return text.operator<<(stream);
	}
}
