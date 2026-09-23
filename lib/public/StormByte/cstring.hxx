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
	 * The class is minimal on purpose: copy, move, reset, and a
	 * `const char*` view. There is no append, find, iterator or
	 * allocator API.
	 *
	 * On Windows a `std::string` created in one DLL and consumed in
	 * another can use a different CRT heap. Destroying that string
	 * on the wrong side of the boundary is undefined. @ref CString
	 * allocates and frees the buffer inside the StormByte DLL, so
	 * the object itself is safe to return or hold across modules.
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
			 * @brief Pointer to the owned text.
			 * @return Buffer, or null.
			 */
			const char* Get() const noexcept;

			/**
			 * @brief Relinquishes ownership.
			 * @return Buffer; this object becomes null.
			 *
			 * The caller must `delete[]` the pointer or hand it to
			 * another @ref CString via @ref Reset.
			 */
			const char* Release() noexcept;

			/**
			 * @brief Replaces the buffer with a copy of @p str.
			 * @param str Source; may be null.
			 */
			void Reset(const char* str = nullptr) noexcept;

		private:
			const char* m_data;	///< Owned buffer

			/**
			 * @brief Copies @p str into a new buffer.
			 * @param str Source; may be null.
			 * @return New buffer, or null.
			 */
			static const char* Duplicate(const char* str) noexcept;
	};
}
