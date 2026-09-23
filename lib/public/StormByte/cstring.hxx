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
	 * @class CString
	 * @brief Owned NUL-terminated buffer that may cross a DLL boundary.
	 *
	 * This is not a replacement or reimplementation of `std::string`.
	 * The class is minimal on purpose: copy, move, reset, a C-string
	 * view, `Length`, and conversions. There is no append, find,
	 * iterator or allocator API.
	 *
	 * On Windows a `std::string` created in one DLL and consumed in
	 * another can use a different CRT heap. Destroying that string
	 * on the wrong side of the boundary is undefined. @ref CString
	 * allocates and frees the buffer inside the StormByte DLL, so
	 * the object itself is safe to return or hold across modules.
	 *
	 * `operator const char*` is the analogue of `std::string::c_str()`.
	 * The pointer is valid only until this object is destroyed, moved
	 * from, assigned or @ref Reset. Using it afterwards is use-after-free.
	 *
	 * `operator std::string` and `operator<<` are inline so the
	 * `std::string` / stream write run in the caller’s translation
	 * unit and do not return a Base-allocated `std::string`.
	 *
	 * If the text never leaves the module that created it, or the
	 * program is not built for Windows, use `std::string`.
	 */
	class STORMBYTE_PUBLIC CString {
		public:
			/**
			 * @brief Empty (null) buffer.
			 */
			CString() noexcept;

			/**
			 * @brief Copies @p str.
			 * @param str Source; may be null.
			 */
			explicit CString(const char* str) noexcept;

			/**
			 * @brief Copy constructor.
			 * @param other Buffer to copy.
			 */
			CString(const CString& other) noexcept;

			/**
			 * @brief Move constructor.
			 * @param other Buffer to take.
			 */
			CString(CString&& other) noexcept;

			/**
			 * @brief Releases the buffer.
			 */
			~CString() noexcept;

			/**
			 * @brief Copy assignment.
			 * @param other Buffer to copy.
			 * @return *this.
			 */
			CString& operator=(const CString& other) noexcept;

			/**
			 * @brief Move assignment.
			 * @param other Buffer to take.
			 * @return *this.
			 */
			CString& operator=(CString&& other) noexcept;

			/**
			 * @brief Replaces the buffer with a copy of @p str.
			 * @param str Source; may be null.
			 */
			void Reset(const char* str = nullptr) noexcept;

			/**
			 * @brief Character count (`strlen`), or `0` when empty.
			 * @return Length.
			 */
			std::size_t Length() const noexcept;

			/**
			 * @brief View of the owned buffer.
			 * @return Buffer, or null.
			 * @note Same lifetime rules as `std::string::c_str()`.
			 */
			explicit operator const char*() const noexcept;

			/**
			 * @brief Copy of the text in the caller’s heap.
			 * @return Empty string when the buffer is null.
			 */
			inline operator std::string() const {
				const char* text = static_cast<const char*>(*this);
				return text ? std::string(text) : std::string();
			}

			/**
			 * @brief Writes the text to @p stream.
			 * @param stream Destination.
			 * @return @p stream.
			 */
			inline std::ostream& operator<<(std::ostream& stream) const {
				const char* text = static_cast<const char*>(*this);
				if (text)
					stream << text;
				return stream;
			}

		private:
			const char* m_data;	///< Owned buffer

			/**
			 * @brief Copies @p str into a new buffer.
			 * @param str Source; may be null.
			 * @return New buffer, or null.
			 */
			static const char* Duplicate(const char* str) noexcept;
	};

	/**
	 * @brief Writes @p text to @p stream.
	 * @param stream Destination.
	 * @param text Source.
	 * @return @p stream.
	 */
	inline std::ostream& operator<<(std::ostream& stream, const CString& text) {
		return text.operator<<(stream);
	}
}
