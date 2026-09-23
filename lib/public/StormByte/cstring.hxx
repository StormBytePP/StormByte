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

#include <compare>
#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <string_view>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @class CString
	 * @brief Owned NUL-terminated buffer, safe to use across a DLL boundary.
	 *
	 * This is not a replacement or reimplementation of `std::string`.
	 * The class is minimal on purpose: copy, move, reset, a C-string
	 * view, `Length`, subscript, equality, ordering, swap and conversions.
	 *
	 * `operator const char*` is the analogue of `std::string::c_str()`.
	 * The pointer is valid only until this object is destroyed, moved
	 * from, assigned or @ref Reset. Using it afterwards is use-after-free.
	 *
	 * `operator bool` is true when the pointer is not null. A buffer
	 * constructed from `""` is empty (`Length() == 0`) and valid.
	 * A default-constructed object is null.
	 *
	 * `operator[]` is an observer. Valid indices are `[0, Length()]`;
	 * `Length()` is the trailing NUL. A null buffer or an index past
	 * `Length()` is undefined and `assert`s when assertions are on.
	 *
	 * Equality and `<=>` compare text, not addresses. Two nulls are
	 * equal. Null is not equal to `""`. Null orders before any text.
	 *
	 * `operator std::string` and `operator<<` are inline so the
	 * `std::string` / stream write run in the caller’s translation unit.
	 *
	 * If the text never leaves the module that created it, or the
	 * program is not built for Windows, use `std::string`.
	 */
	class STORMBYTE_PUBLIC CString {
		public:
			/**
			 * @name Life
			 * @{
			 */

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
			 * @param other Buffer to take. @p other becomes null.
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
			 * @param other Buffer to take. @p other becomes null.
			 * @return *this.
			 */
			CString& operator=(CString&& other) noexcept;

			/** @} */

			/**
			 * @name Modifiers
			 * @{
			 */

			/**
			 * @brief Replaces the buffer with a copy of @p str.
			 * @param str Source; may be null.
			 */
			void Reset(const char* str = nullptr) noexcept;

			/**
			 * @brief Swaps buffers with @p other.
			 * @param other Other buffer.
			 */
			void swap(CString& other) noexcept;

			/** @} */

			/**
			 * @name Observers
			 * @{
			 */

			/**
			 * @brief Character count (`strlen`), or `0` when empty or null.
			 * @return Length.
			 */
			std::size_t Length() const noexcept;

			/**
			 * @brief Character at @p index.
			 * @param index Position in `[0, Length()]`. `Length()` is the trailing NUL.
			 * @return The character.
			 * @note Null or `index > Length()` is undefined. Checked with `assert` when assertions are on.
			 */
			char operator[](std::size_t index) const noexcept;

			/**
			 * @brief `true` when the buffer pointer is not null.
			 * @note `""` is valid and empty. A default object is null.
			 * @return Whether a buffer is held.
			 */
			inline explicit operator bool() const noexcept {
				return static_cast<const char*>(*this) != nullptr;
			}

			/** @} */

			/**
			 * @name Conversions
			 * @{
			 */

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

			/** @} */

			/**
			 * @name Comparison
			 * @{
			 */

			/**
			 * @brief Content equality.
			 * @param other Other buffer.
			 * @return Whether the texts are equal.
			 */
			bool operator==(const CString& other) const noexcept;

			/**
			 * @brief Content inequality.
			 * @param other Other buffer.
			 * @return Whether the texts differ.
			 */
			bool operator!=(const CString& other) const noexcept {
				return !(*this == other);
			}

			/**
			 * @brief Content equality with a C string.
			 * @param str May be null (treated as a null @ref CString).
			 * @return Whether the texts are equal.
			 */
			bool operator==(const char* str) const noexcept;

			/**
			 * @brief Content inequality with a C string.
			 * @param str May be null.
			 * @return Whether the texts differ.
			 */
			bool operator!=(const char* str) const noexcept {
				return !(*this == str);
			}

			/**
			 * @brief Content order. Null is less than any text.
			 * @param other Other buffer.
			 * @return Ordering.
			 */
			std::strong_ordering operator<=>(const CString& other) const noexcept;

			/**
			 * @brief Content order against a C string.
			 * @param str May be null.
			 * @return Ordering.
			 */
			std::strong_ordering operator<=>(const char* str) const noexcept;

			/** @} */

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

	/**
	 * @brief Content equality.
	 * @param str C string; may be null.
	 * @param text Buffer.
	 * @return Whether the texts are equal.
	 */
	inline bool operator==(const char* str, const CString& text) noexcept {
		return text == str;
	}

	/**
	 * @brief Content inequality.
	 * @param str C string; may be null.
	 * @param text Buffer.
	 * @return Whether the texts differ.
	 */
	inline bool operator!=(const char* str, const CString& text) noexcept {
		return text != str;
	}

	/**
	 * @brief Swaps two buffers.
	 * @param left First buffer.
	 * @param right Second buffer.
	 */
	inline void swap(CString& left, CString& right) noexcept {
		left.swap(right);
	}
}

/**
 * @brief Hash of the text (`0` when the buffer is null).
 */
template<>
struct std::hash<StormByte::CString> {
	/**
	 * @brief Hashes @p text.
	 * @param text Buffer.
	 * @return Hash.
	 */
	std::size_t operator()(const StormByte::CString& text) const noexcept {
		const char* raw = static_cast<const char*>(text);
		if (!raw)
			return 0;
		return std::hash<std::string_view>{}(raw);
	}
};
