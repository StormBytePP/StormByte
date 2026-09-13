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

#include <concepts>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	namespace Type {
		namespace Detail {
			/**
			 * @brief `true` when @p T is exactly `std::variant<Ts...>` after cv/ref strip.
			 * @tparam T Type to test.
			 *
			 * Partial specialization of a variable template — not SFINAE.
			 * Needed because there is no standard `std::is_variant`.
			 */
			template<typename T>
			constexpr bool is_variant_v = false;

			/**
			 * @brief Specialization for every `std::variant` alternative list.
			 * @tparam Ts Variant alternatives.
			 */
			template<typename... Ts>
			constexpr bool is_variant_v<std::variant<Ts...>> = true;

			/**
			 * @brief Fold: is cv/ref-stripped @p U one of @p VariantT's alternatives?
			 * @tparam VariantT A `std::variant<…>` (already stripped by the caller).
			 * @tparam U Candidate alternative.
			 * @tparam I Index pack over `std::variant_size_v<VariantT>`.
			 * @param[in] seq Index sequence; unused except to expand @p I.
			 * @return `true` if @p U matches any alternative.
			 */
			template<typename VariantT, typename U, std::size_t... I>
			constexpr bool variant_has_type_impl(std::index_sequence<I...> seq) noexcept {
				(void)seq;
				return ((std::is_same_v<
					std::remove_cvref_t<U>,
					std::remove_cvref_t<std::variant_alternative_t<I, VariantT>>
				>) || ...);
			}

			/**
			 * @brief Convenience wrapper around @ref variant_has_type_impl.
			 * @tparam VariantT A `std::variant<…>`.
			 * @tparam U Candidate alternative.
			 */
			template<typename VariantT, typename U>
			constexpr bool variant_has_type_v =
				variant_has_type_impl<VariantT, U>(
					std::make_index_sequence<std::variant_size_v<VariantT>>()
				);
		}

		/**
		 * @defgroup TypeWrappers Standard wrapper and aggregate concepts
		 * @brief Predicates for standard wrappers and pair-like aggregate values.
		 * @{
		 */

		/**
		 * @brief Exactly `std::optional<U>` for some `U`.
		 * @tparam T Type to test (no decay).
		 *
		 * A type that merely has `value_type` does not match.
		 *
		 * @code
		 * template<Type::Optional T>
		 * void process(T opt);
		 * @endcode
		 */
		template<typename T>
		concept Optional =
			requires { typename T::value_type; } &&
			std::same_as<T, std::optional<typename T::value_type>>;

		/**
		 * @brief Type with accessible `first` and `second` members.
		 * @tparam T Type to test.
		 *
		 * Broader than `std::pair`: any struct with those members matches.
		 * That is intentional and must stay that way.
		 *
		 * @code
		 * template<Type::Pair T>
		 * void process(T pair);
		 * @endcode
		 */
		template<typename T>
		concept Pair = requires(T t) {
			t.first;
			t.second;
		};

		/**
		 * @brief Instantiation of `std::variant`, after stripping cv/ref.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Variant T>
		 * void process(T var);
		 * @endcode
		 */
		template<typename T>
		concept Variant = Detail::is_variant_v<std::remove_cvref_t<T>>;

		/**
		 * @brief @ref Variant @p T that lists @p U among its alternatives.
		 * @tparam T Variant type (cv/ref stripped before the lookup).
		 * @tparam U Alternative to look for (cv/ref stripped).
		 *
		 * @code
		 * template<typename T, typename U>
		 * requires Type::VariantHasType<T, U>
		 * void handle_variant(T var);
		 * @endcode
		 */
		template<typename T, typename U>
		concept VariantHasType =
			Variant<T> &&
			Detail::variant_has_type_v<std::remove_cvref_t<T>, U>;
		/** @} */
	}
}
