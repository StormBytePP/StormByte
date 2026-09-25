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

#include <StormByte/exception.hxx>
#include <StormByte/size.hxx>
#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <compare>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <string_view>
#include <utility>

/**
 * @file StormByte/binary_data.hxx
 * @brief Owned raw byte sequence for the StormByte suite.
 */

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @class BinaryData
	 * @brief Owned raw byte sequence, safe across shared-library boundaries.
	 *
	 * @par What it is
	 * A contiguous container of @c std::byte with the same kind of API as
	 * @c std::vector&lt;std::byte&gt; (iterators, @c &lt;algorithm&gt;,
	 * @c std::ranges, @c std::span, insert / erase / assign). Lengths use
	 * @ref StormByte::Size. Member names stay lowercase to match the STL.
	 *
	 * @par Why it exists
	 * @c std::vector is not a safe ABI type between two copies of a C++
	 * runtime. A vector allocated in the application and grown, returned
	 * or destroyed inside a StormByte shared library (or the other way
	 * around) uses two heaps. On Windows that is a hard crash when CRTs
	 * differ; on Unix it fails when libc++ and libstdc++ mix.
	 *
	 * @c BinaryData owns its storage on StormByte Base's heap.
	 * Construction, growth and destruction always run in this library.
	 * Other suite modules can put @c BinaryData in a public signature
	 * without exporting @c std::vector&lt;std::byte&gt;.
	 *
	 * @par What it is for
	 * Any StormByte component that must carry raw binary — payloads,
	 * encoded blobs, file images, wire fragments — uses this type
	 * instead of a standard vector of bytes. It is not text
	 * (@ref StormByte::CString) and not a structured document.
	 *
	 * @par Thread safety
	 * Not thread-safe. Callers that share an instance must synchronise.
	 */
	class STORMBYTE_PUBLIC BinaryData {
		public:
			/**
			 * @brief Element type.
			 */
			using value_type = std::byte;

			/**
			 * @brief STL size typedef; @ref size() returns @ref StormByte::Size.
			 */
			using size_type = std::size_t;

			/**
			 * @brief Iterator difference.
			 */
			using difference_type = std::ptrdiff_t;

			/**
			 * @brief Mutable reference to an element.
			 */
			using reference = std::byte&;

			/**
			 * @brief Const reference to an element.
			 */
			using const_reference = const std::byte&;

			/**
			 * @brief Mutable pointer and iterator.
			 */
			using pointer = std::byte*;

			/**
			 * @brief Const pointer and iterator.
			 */
			using const_pointer = const std::byte*;

			/**
			 * @brief Contiguous mutable iterator.
			 */
			using iterator = std::byte*;

			/**
			 * @brief Contiguous const iterator.
			 */
			using const_iterator = const std::byte*;

			/**
			 * @brief Reverse iterator.
			 */
			using reverse_iterator = std::reverse_iterator<iterator>;

			/**
			 * @brief Const reverse iterator.
			 */
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

			/**
			 * @name Constructors / destructor / assignment
			 * @{
			 */

			/**
			 * @brief Construct an empty sequence.
			 */
			BinaryData() noexcept;

			/**
			 * @brief Construct @p count bytes filled with @p value.
			 * @param count Element count.
			 * @param value Fill byte.
			 */
			BinaryData(const Size& count, std::byte value);

			/**
			 * @brief Construct @p count zeroed bytes.
			 * @param count Element count.
			 */
			explicit BinaryData(const Size& count);

			/**
			 * @brief Copy bytes from a contiguous span.
			 * @param bytes Source view (not owned).
			 */
			explicit BinaryData(std::span<const std::byte> bytes);

			/**
			 * @brief Copy @p count bytes starting at @p bytes.
			 * @param bytes Source pointer; may be null when @p count is zero.
			 * @param count Byte count.
			 */
			BinaryData(const std::byte* bytes, const Size& count);

			/**
			 * @brief Copy from an initializer list.
			 * @param list Source bytes.
			 */
			BinaryData(std::initializer_list<std::byte> list);

			/**
			 * @brief Copy character bytes from a string view (no trailing NUL).
			 * @param sv Source characters.
			 */
			explicit BinaryData(std::string_view sv);

			/**
			 * @brief Copy character bytes from a C string (no trailing NUL).
			 * @param s Source; a null pointer yields an empty sequence.
			 */
			explicit BinaryData(const char* s);

			/**
			 * @brief Copy from an input range of byte-convertible values.
			 * @tparam R Range type satisfying @ref StormByte::Type::ByteInputRange.
			 * @param range Source range.
			 */
			template<Type::ByteInputRange R>
			explicit BinaryData(const R& range);

			/**
			 * @brief Consume an rvalue range. Moves when @p R is an rvalue @ref StormByte::BinaryData.
			 * @tparam R Range type satisfying @ref StormByte::Type::ByteInputRange.
			 * @param range Source range.
			 * @note An lvalue @ref StormByte::BinaryData is copied. @ref StormByte::Type::SameAs
			 *       ignores references, so the implementation uses
			 *       @ref StormByte::Type::LvalueReference before moving.
			 */
			template<Type::ByteInputRange R>
			explicit BinaryData(R&& range);

			/**
			 * @brief Copy construct. Allocation runs in this library.
			 * @param other Source sequence.
			 */
			BinaryData(const BinaryData& other);

			/**
			 * @brief Move construct. @p other is left empty.
			 * @param other Source sequence.
			 */
			BinaryData(BinaryData&& other) noexcept;

			/**
			 * @brief Destroy the sequence on this library's heap.
			 */
			~BinaryData() noexcept;

			/**
			 * @brief Copy assign. Allocation runs in this library.
			 * @param other Source sequence.
			 * @return @c *this.
			 */
			BinaryData& operator=(const BinaryData& other);

			/**
			 * @brief Move assign. @p other is left empty.
			 * @param other Source sequence.
			 * @return @c *this.
			 */
			BinaryData& operator=(BinaryData&& other) noexcept;

			/**
			 * @brief Replace contents with an initializer list.
			 * @param list Source bytes.
			 * @return @c *this.
			 */
			BinaryData& operator=(std::initializer_list<std::byte> list);

			/** @} */

			/**
			 * @name Comparison
			 * @{
			 */

			/**
			 * @brief Equality of byte contents.
			 * @param other Other sequence.
			 * @return @c true when sizes and bytes match.
			 */
			bool operator==(const BinaryData& other) const noexcept;

			/**
			 * @brief Inequality of byte contents.
			 * @param other Other sequence.
			 * @return Negation of @ref operator==.
			 */
			bool operator!=(const BinaryData& other) const noexcept;

			/**
			 * @brief Three-way lexicographical comparison (same order as @c std::vector).
			 * @param other Other sequence.
			 * @return @c std::strong_ordering.
			 */
			std::strong_ordering operator<=>(const BinaryData& other) const noexcept;

			/** @} */

			/**
			 * @name Iterators
			 * @{
			 */

			/**
			 * @brief Mutable iterator to the first byte.
			 * @return @c data() when non-empty; a valid empty iterator otherwise.
			 */
			iterator begin() noexcept;

			/**
			 * @brief Const iterator to the first byte.
			 * @return @c data() when non-empty; a valid empty iterator otherwise.
			 */
			const_iterator begin() const noexcept;

			/**
			 * @brief Mutable iterator one past the last byte.
			 * @return @c data() + size.
			 */
			iterator end() noexcept;

			/**
			 * @brief Const iterator one past the last byte.
			 * @return @c data() + size.
			 */
			const_iterator end() const noexcept;

			/**
			 * @brief Const iterator to the first byte.
			 * @return Same as const @ref begin().
			 */
			const_iterator cbegin() const noexcept;

			/**
			 * @brief Const iterator one past the last byte.
			 * @return Same as const @ref end().
			 */
			const_iterator cend() const noexcept;

			/**
			 * @brief Mutable reverse iterator to the last byte.
			 * @return @c reverse_iterator(end()).
			 */
			reverse_iterator rbegin() noexcept;

			/**
			 * @brief Mutable reverse iterator to before the first byte.
			 * @return @c reverse_iterator(begin()).
			 */
			reverse_iterator rend() noexcept;

			/**
			 * @brief Const reverse iterator to the last byte.
			 * @return @c const_reverse_iterator(end()).
			 */
			const_reverse_iterator rbegin() const noexcept;

			/**
			 * @brief Const reverse iterator to before the first byte.
			 * @return @c const_reverse_iterator(begin()).
			 */
			const_reverse_iterator rend() const noexcept;

			/**
			 * @brief Const reverse iterator to the last byte.
			 * @return Same as const @ref rbegin().
			 */
			const_reverse_iterator crbegin() const noexcept;

			/**
			 * @brief Const reverse iterator to before the first byte.
			 * @return Same as const @ref rend().
			 */
			const_reverse_iterator crend() const noexcept;

			/** @} */

			/**
			 * @name Capacity
			 * @{
			 */

			/**
			 * @brief Occupied length in bytes.
			 * @return @ref StormByte::Size (this is a byte length).
			 */
			Size size() const noexcept;

			/**
			 * @brief Implementation maximum size in bytes.
			 * @return @ref StormByte::Size.
			 */
			Size max_size() const noexcept;

			/**
			 * @brief Allocated capacity in bytes.
			 * @return @ref StormByte::Size.
			 */
			Size capacity() const noexcept;

			/**
			 * @brief Whether the sequence holds no bytes.
			 * @return @c true when @ref size() is zero.
			 */
			bool empty() const noexcept;

			/**
			 * @brief Request capacity of at least @p new_cap bytes.
			 * @param new_cap Requested capacity.
			 */
			void reserve(const Size& new_cap);

			/**
			 * @brief Resize to @p new_size bytes. Appended bytes are zero.
			 * @param new_size New size.
			 */
			void resize(const Size& new_size);

			/**
			 * @brief Resize to @p new_size bytes. Appended bytes are @p value.
			 * @param new_size New size.
			 * @param value Fill for new bytes.
			 */
			void resize(const Size& new_size, std::byte value);

			/**
			 * @brief Release unused capacity when the implementation allows it.
			 */
			void shrink_to_fit();

			/**
			 * @brief Drop every byte. Capacity may remain.
			 */
			void clear() noexcept;

			/** @} */

			/**
			 * @name Element access
			 * @{
			 */

			/**
			 * @brief Unchecked mutable subscript.
			 * @param index Byte offset.
			 * @return Reference to the byte at @p index.
			 */
			std::byte& operator[](const Size& index) noexcept;

			/**
			 * @brief Unchecked const subscript.
			 * @param index Byte offset.
			 * @return Const reference to the byte at @p index.
			 */
			const std::byte& operator[](const Size& index) const noexcept;

			/**
			 * @brief Checked mutable subscript.
			 * @param index Byte offset.
			 * @return Reference to the byte at @p index.
			 * @throws OutOfBoundsError When @p index is not less than @ref size().
			 */
			std::byte& at(const Size& index);

			/**
			 * @brief Checked const subscript.
			 * @param index Byte offset.
			 * @return Const reference to the byte at @p index.
			 * @throws OutOfBoundsError When @p index is not less than @ref size().
			 */
			const std::byte& at(const Size& index) const;

			/**
			 * @brief First byte.
			 * @return Mutable reference to the first byte.
			 * @warning Undefined when @ref empty().
			 */
			std::byte& front() noexcept;

			/**
			 * @brief First byte.
			 * @return Const reference to the first byte.
			 * @warning Undefined when @ref empty().
			 */
			const std::byte& front() const noexcept;

			/**
			 * @brief Last byte.
			 * @return Mutable reference to the last byte.
			 * @warning Undefined when @ref empty().
			 */
			std::byte& back() noexcept;

			/**
			 * @brief Last byte.
			 * @return Const reference to the last byte.
			 * @warning Undefined when @ref empty().
			 */
			const std::byte& back() const noexcept;

			/**
			 * @brief Pointer to the first byte, or null when empty.
			 * @return Mutable pointer.
			 */
			std::byte* data() noexcept;

			/**
			 * @brief Pointer to the first byte, or null when empty.
			 * @return Const pointer.
			 */
			const std::byte* data() const noexcept;

			/**
			 * @brief Mutable span covering the occupied bytes.
			 * @return @c std::span&lt;std::byte&gt;.
			 */
			std::span<std::byte> span() noexcept;

			/**
			 * @brief Const span covering the occupied bytes.
			 * @return @c std::span&lt;const std::byte&gt;.
			 */
			std::span<const std::byte> span() const noexcept;

			/**
			 * @brief Convert to a mutable span.
			 * @return Same as @ref span() noexcept.
			 */
			operator std::span<std::byte>() noexcept;

			/**
			 * @brief Convert to a const span.
			 * @return Same as const @ref span() const.
			 */
			operator std::span<const std::byte>() const noexcept;

			/** @} */

			/**
			 * @name Modifiers
			 * @{
			 */

			/**
			 * @brief Append one byte.
			 * @param value Byte to append.
			 */
			void push_back(std::byte value);

			/**
			 * @brief Append one byte constructed from @p args.
			 * @tparam Args Constructor argument types for @c std::byte.
			 * @param args Arguments forwarded to @c std::byte.
			 */
			template<typename... Args>
			void emplace_back(Args&&... args);

			/**
			 * @brief Remove the last byte.
			 * @warning Undefined when @ref empty().
			 */
			void pop_back();

			/**
			 * @brief Insert @p value before @p pos.
			 * @param pos Insertion iterator.
			 * @param value Byte to insert.
			 * @return Iterator to the inserted byte.
			 */
			iterator insert(const_iterator pos, std::byte value);

			/**
			 * @brief Insert @p count copies of @p value before @p pos.
			 * @param pos Insertion iterator.
			 * @param count Number of bytes.
			 * @param value Fill byte.
			 * @return Iterator to the first inserted byte, or @p pos when @p count is 0.
			 */
			iterator insert(const_iterator pos, const Size& count, std::byte value);

			/**
			 * @brief Insert a copy of @p list before @p pos.
			 * @param pos Insertion iterator.
			 * @param list Bytes to insert.
			 * @return Iterator to the first inserted byte, or @p pos when @p list is empty.
			 */
			iterator insert(const_iterator pos, std::initializer_list<std::byte> list);

			/**
			 * @brief Insert a copy of @p bytes before @p pos.
			 * @param pos Insertion iterator.
			 * @param bytes Source span.
			 * @return Iterator to the first inserted byte, or @p pos when @p bytes is empty.
			 */
			iterator insert(const_iterator pos, std::span<const std::byte> bytes);

			/**
			 * @brief Insert the range @c [first, last) before @p pos.
			 * @tparam InputIt Input iterator type.
			 * @param pos Insertion iterator.
			 * @param first Range begin.
			 * @param last Range end.
			 * @return Iterator to the first inserted byte, or @p pos when the range is empty.
			 */
			template<typename InputIt>
			iterator insert(const_iterator pos, InputIt first, InputIt last);

			/**
			 * @brief Erase the byte at @p pos.
			 * @param pos Byte to erase.
			 * @return Iterator following the erased byte.
			 */
			iterator erase(const_iterator pos);

			/**
			 * @brief Erase @c [first, last).
			 * @param first Range begin.
			 * @param last Range end.
			 * @return Iterator following the last erased byte.
			 */
			iterator erase(const_iterator first, const_iterator last);

			/**
			 * @brief Replace contents with @p count copies of @p value.
			 * @param count New size.
			 * @param value Fill byte.
			 */
			void assign(const Size& count, std::byte value);

			/**
			 * @brief Replace contents with @p list.
			 * @param list Source bytes.
			 */
			void assign(std::initializer_list<std::byte> list);

			/**
			 * @brief Replace contents with @p bytes.
			 * @param bytes Source span.
			 */
			void assign(std::span<const std::byte> bytes);

			/**
			 * @brief Replace contents with @c [first, last).
			 * @tparam InputIt Input iterator type.
			 * @param first Range begin.
			 * @param last Range end.
			 */
			template<typename InputIt>
			void assign(InputIt first, InputIt last);

			/**
			 * @brief Append a copy of @p bytes.
			 * @param bytes Source span.
			 */
			void append(std::span<const std::byte> bytes);

			/**
			 * @brief Append @p count bytes from @p bytes.
			 * @param bytes Source pointer; may be null when @p count is zero.
			 * @param count Byte count.
			 */
			void append(const std::byte* bytes, const Size& count);

			/**
			 * @brief Append a copy of @p other.
			 * @param other Source sequence.
			 */
			void append(const BinaryData& other);

			/**
			 * @brief Append by moving @p other. @p other is left empty.
			 * @param other Source sequence.
			 */
			void append(BinaryData&& other);

			/**
			 * @brief Exchange contents with @p other.
			 * @param other Other sequence.
			 */
			void swap(BinaryData& other) noexcept;

			/** @} */

		private:
			/// @cond
			class Storage;
			std::unique_ptr<Storage> m_storage;
			/// @endcond

			/**
			 * @brief Byte offset of @p pos from @ref data().
			 * @param pos Iterator into this sequence.
			 * @return Offset in bytes.
			 */
			Size offset_of(const_iterator pos) const noexcept;

			/**
			 * @brief Insert @p count bytes from @p bytes before offset @p index.
			 * @param index Insertion offset.
			 * @param bytes Source pointer; may be null when @p count is zero.
			 * @param count Byte count.
			 * @return Iterator to the first inserted byte.
			 */
			iterator insert_at(const Size& index, const std::byte* bytes, const Size& count);
	};

	/**
	 * @brief Swap two @ref StormByte::BinaryData sequences.
	 * @param lhs Left-hand side.
	 * @param rhs Right-hand side.
	 */
	STORMBYTE_PUBLIC void swap(BinaryData& lhs, BinaryData& rhs) noexcept;

	/// @cond
	template<Type::ByteInputRange R>
	BinaryData::BinaryData(const R& range)
		: BinaryData() {
		if constexpr (requires { std::ranges::size(range); }) {
			const auto s = std::ranges::size(range);
			if (s > 0)
				reserve(Size{static_cast<std::size_t>(s)});
		}
		for (auto&& element : range)
			push_back(static_cast<std::byte>(element));
	}

	template<Type::ByteInputRange R>
	BinaryData::BinaryData(R&& range)
		: BinaryData() {
		if constexpr (Type::SameAs<R, BinaryData>) {
			if constexpr (Type::LvalueReference<R>)
				*this = range;
			else
				*this = std::move(range);
		}
		else {
			if constexpr (requires { std::ranges::size(range); }) {
				const auto s = std::ranges::size(range);
				if (s > 0)
					reserve(Size{static_cast<std::size_t>(s)});
			}
			for (auto&& element : range)
				push_back(static_cast<std::byte>(element));
		}
	}

	template<typename... Args>
	void BinaryData::emplace_back(Args&&... args) {
		push_back(std::byte(std::forward<Args>(args)...));
	}

	template<typename InputIt>
	BinaryData::iterator BinaryData::insert(const_iterator pos, InputIt first, InputIt last) {
		const Size index = offset_of(pos);
		BinaryData scratch;
		for (; first != last; ++first)
			scratch.push_back(static_cast<std::byte>(*first));
		return insert_at(index, scratch.data(), scratch.size());
	}

	template<typename InputIt>
	void BinaryData::assign(InputIt first, InputIt last) {
		clear();
		for (; first != last; ++first)
			push_back(static_cast<std::byte>(*first));
	}
	/// @endcond
}
