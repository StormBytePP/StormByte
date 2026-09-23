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
	 * @class WCString
	 * @brief Owned NUL-terminated wide buffer, safe to use across a DLL boundary.
	 *
	 * Wide counterpart of @ref CString. Not a replacement of `std::wstring`.
	 * The class is minimal: copy, move, reset, a C wide-string view,
	 * `Length`, subscript, equality, ordering, swap and conversions.
	 *
	 * `operator const wchar_t*` is the analogue of `std::wstring::c_str()`.
	 * The pointer is valid only until this object is destroyed, moved
	 * from, assigned or @ref Reset. Using it afterwards is use-after-free.
	 *
	 * `operator std::wstring_view` is explicit and follows the same
	 * lifetime. A null buffer yields an empty view. The view covers
	 * `[0, Length())` and does not include the trailing NUL.
	 *
	 * `operator bool` is true when the pointer is not null. A buffer
	 * constructed from `L""` is empty (`Length() == 0`) and valid.
	 * A default-constructed object is null.
	 *
	 * `operator[]` is an observer. Valid indices are `[0, Length()]`;
	 * `Length()` is the trailing NUL. A null buffer or an index past
	 * `Length()` is undefined and `assert`s when assertions are on.
	 *
	 * Equality and `<=>` compare text, not addresses. Two nulls are
	 * equal. Null is not equal to `L""`. Null orders before any text.
	 *
	 * `operator std::wstring`, `operator std::wstring_view` and
	 * `operator<<` are inline so they run in the caller’s translation unit.
	 */
	class STORMBYTE_PUBLIC WCString {
		public:
			/**
			 * @name Life
			 * @{
			 */

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
			 * @param other Buffer to take. @p other becomes null.
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
			 * @param other Buffer to take. @p other becomes null.
			 * @return *this.
			 */
			WCString& operator=(WCString&& other) noexcept;

			/** @} */

			/**
			 * @name Modifiers
			 * @{
			 */

			/**
			 * @brief Replaces the buffer with a copy of @p str.
			 * @param str Source; may be null.
			 */
			void Reset(const wchar_t* str = nullptr) noexcept;

			/**
			 * @brief Swaps buffers with @p other.
			 * @param other Other buffer.
			 */
			void swap(WCString& other) noexcept;

			/** @} */

			/**
			 * @name Observers
			 * @{
			 */

			/**
			 * @brief Character count (`wcslen`), or `0` when empty or null.
			 * @return Length.
			 */
			std::size_t Length() const noexcept;

			/**
			 * @brief Character at @p index.
			 * @param index Position in `[0, Length()]`. `Length()` is the trailing NUL.
			 * @return The character.
			 * @note Null or `index > Length()` is undefined. Checked with `assert` when assertions are on.
			 */
			wchar_t operator[](std::size_t index) const noexcept;

			/**
			 * @brief `true` when the buffer pointer is not null.
			 * @note `L""` is valid and empty. A default object is null.
			 * @return Whether a buffer is held.
			 */
			inline explicit operator bool() const noexcept {
				return static_cast<const wchar_t*>(*this) != nullptr;
			}

			/** @} */

			/**
			 * @name Conversions
			 * @{
			 */

			/**
			 * @brief View of the owned buffer.
			 * @return Buffer, or null.
			 * @note Same lifetime rules as `std::wstring::c_str()`.
			 */
			explicit operator const wchar_t*() const noexcept;

			/**
			 * @brief Non-owning view of the text.
			 * @return Empty view when the buffer is null.
			 * @note Same lifetime rules as `std::wstring::c_str()`.
			 */
			inline explicit operator std::wstring_view() const noexcept {
				const wchar_t* text = static_cast<const wchar_t*>(*this);
				return text ? std::wstring_view(text) : std::wstring_view();
			}

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
			bool operator==(const WCString& other) const noexcept;

			/**
			 * @brief Content inequality.
			 * @param other Other buffer.
			 * @return Whether the texts differ.
			 */
			bool operator!=(const WCString& other) const noexcept {
				return !(*this == other);
			}

			/**
			 * @brief Content equality with a C wide string.
			 * @param str May be null (treated as a null @ref WCString).
			 * @return Whether the texts are equal.
			 */
			bool operator==(const wchar_t* str) const noexcept;

			/**
			 * @brief Content inequality with a C wide string.
			 * @param str May be null.
			 * @return Whether the texts differ.
			 */
			bool operator!=(const wchar_t* str) const noexcept {
				return !(*this == str);
			}

			/**
			 * @brief Content order. Null is less than any text.
			 * @param other Other buffer.
			 * @return Ordering.
			 */
			std::strong_ordering operator<=>(const WCString& other) const noexcept;

			/**
			 * @brief Content order against a C wide string.
			 * @param str May be null.
			 * @return Ordering.
			 */
			std::strong_ordering operator<=>(const wchar_t* str) const noexcept;

			/** @} */

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

	/**
	 * @brief Content equality.
	 * @param str C wide string; may be null.
	 * @param text Buffer.
	 * @return Whether the texts are equal.
	 */
	inline bool operator==(const wchar_t* str, const WCString& text) noexcept {
		return text == str;
	}

	/**
	 * @brief Content inequality.
	 * @param str C wide string; may be null.
	 * @param text Buffer.
	 * @return Whether the texts differ.
	 */
	inline bool operator!=(const wchar_t* str, const WCString& text) noexcept {
		return text != str;
	}

	/**
	 * @brief Swaps two buffers.
	 * @param left First buffer.
	 * @param right Second buffer.
	 */
	inline void swap(WCString& left, WCString& right) noexcept {
		left.swap(right);
	}
}

/**
 * @brief Hash of the text (`0` when the buffer is null).
 */
template<>
struct std::hash<StormByte::WCString> {
	/**
	 * @brief Hashes @p text.
	 * @param text Buffer.
	 * @return Hash.
	 */
	std::size_t operator()(const StormByte::WCString& text) const noexcept {
		const wchar_t* raw = static_cast<const wchar_t*>(text);
		if (!raw)
			return 0;
		return std::hash<std::wstring_view>{}(raw);
	}
};
