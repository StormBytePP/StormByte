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

#include <StormByte/type_traits/categories.hxx>
#include <StormByte/type_traits/conversions.hxx>

#include <cstddef>
#include <iterator>
#include <ranges>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @namespace Type
	 * @brief Named concepts and small type utilities used across the suite.
	 */
	namespace Type {
		/**
		 * @defgroup TypeRanges Range and iterator concepts
		 * @brief Range and iterator predicates forwarding to the standard ranges library.
		 *
		 * These concepts inspect their template arguments as written. Pass `R&` to
		 * preserve lvalue range semantics when needed.
		 * @{
		 */

		/**
		 * @name Ranges
		 * @{
		 */

		/**
		 * @brief Valid range (`std::ranges::range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept Range = std::ranges::range<R>;

		/**
		 * @brief Input range (`std::ranges::input_range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept InputRange = std::ranges::input_range<R>;

		/**
		 * @brief Output range for values of type @p T (`std::ranges::output_range`).
		 * @tparam R Type to test.
		 * @tparam T Value type to write.
		 */
		template<typename R, typename T>
		concept OutputRange = std::ranges::output_range<R, T>;

		/**
		 * @brief Forward range (`std::ranges::forward_range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept ForwardRange = std::ranges::forward_range<R>;

		/**
		 * @brief Bidirectional range (`std::ranges::bidirectional_range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept BidirectionalRange = std::ranges::bidirectional_range<R>;

		/**
		 * @brief Random-access range (`std::ranges::random_access_range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept RandomAccessRange = std::ranges::random_access_range<R>;

		/**
		 * @brief Contiguous range (`std::ranges::contiguous_range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept ContiguousRange = std::ranges::contiguous_range<R>;

		/**
		 * @brief Sized range (`std::ranges::sized_range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept SizedRange = std::ranges::sized_range<R>;

		/**
		 * @brief Common range (`std::ranges::common_range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept CommonRange = std::ranges::common_range<R>;

		/**
		 * @brief Range view (`std::ranges::view`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept View = std::ranges::view<R>;

		/**
		 * @brief Borrowed range (`std::ranges::borrowed_range`).
		 * @tparam R Type to test.
		 */
		template<typename R>
		concept BorrowedRange = std::ranges::borrowed_range<R>;

		/** @} */

		/**
		 * @name Iterators
		 * @{
		 */

		/**
		 * @brief Input iterator (`std::input_iterator`).
		 * @tparam I Type to test.
		 */
		template<typename I>
		concept InputIterator = std::input_iterator<I>;

		/**
		 * @brief Output iterator for values of type @p T (`std::output_iterator`).
		 * @tparam I Iterator type.
		 * @tparam T Value type to write.
		 */
		template<typename I, typename T>
		concept OutputIterator = std::output_iterator<I, T>;

		/**
		 * @brief Forward iterator (`std::forward_iterator`).
		 * @tparam I Type to test.
		 */
		template<typename I>
		concept ForwardIterator = std::forward_iterator<I>;

		/**
		 * @brief Bidirectional iterator (`std::bidirectional_iterator`).
		 * @tparam I Type to test.
		 */
		template<typename I>
		concept BidirectionalIterator = std::bidirectional_iterator<I>;

		/**
		 * @brief Random-access iterator (`std::random_access_iterator`).
		 * @tparam I Type to test.
		 */
		template<typename I>
		concept RandomAccessIterator = std::random_access_iterator<I>;

		/**
		 * @brief Contiguous iterator (`std::contiguous_iterator`).
		 * @tparam I Type to test.
		 */
		template<typename I>
		concept ContiguousIterator = std::contiguous_iterator<I>;

		/**
		 * @brief Valid sentinel for an iterator (`std::sentinel_for`).
		 * @tparam S Sentinel type.
		 * @tparam I Iterator type.
		 */
		template<typename S, typename I>
		concept SentinelFor = std::sentinel_for<S, I>;

		/**
		 * @brief Sized sentinel for an iterator (`std::sized_sentinel_for`).
		 * @tparam S Sentinel type.
		 * @tparam I Iterator type.
		 */
		template<typename S, typename I>
		concept SizedSentinelFor = std::sized_sentinel_for<S, I>;

		/** @} */

		/**
		 * @name Aliases
		 * @{
		 */

		/**
		 * @brief Value type of a valid range.
		 * @tparam R Range type.
		 */
		template<Range R>
		using RangeValue = std::ranges::range_value_t<R>;

		/**
		 * @brief Reference type produced by a valid range.
		 * @tparam R Range type.
		 */
		template<Range R>
		using RangeReference = std::ranges::range_reference_t<R>;

		/**
		 * @brief Difference type associated with a valid range.
		 * @tparam R Range type.
		 */
		template<Range R>
		using RangeDifference = std::ranges::range_difference_t<R>;

		/**
		 * @brief Value type associated with an iterator.
		 * @tparam I Iterator type.
		 */
		template<typename I>
		using IteratorValue = std::iter_value_t<I>;

		/** @} */

		/**
		 * @name Bytes
		 * @{
		 */

		/**
		 * @brief Input range whose scalar values can be explicitly converted to `std::byte`.
		 * @tparam R Range type.
		 *
		 * Class-valued ranges are rejected. Conversion may be implicit or explicit.
		 */
		template<typename R>
		concept ByteInputRange =
			InputRange<R> &&
			!Class<RangeValue<R>> &&
			ExplicitlyConvertibleTo<RangeValue<R>, std::byte>;

		/**
		 * @brief Input iterator whose scalar values can be explicitly converted to `std::byte`.
		 * @tparam I Iterator type.
		 *
		 * Class-valued iterator values are rejected. Conversion may be implicit or explicit.
		 */
		template<typename I>
		concept ByteInputIterator =
			InputIterator<I> &&
			!Class<IteratorValue<I>> &&
			ExplicitlyConvertibleTo<IteratorValue<I>, std::byte>;

		/** @} */
		/** @} */
	}
}
