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

#include <StormByte/cstring.hxx>
#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <cassert>
#include <compare>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @class Size
	 * @brief Byte count that is the same width on every host.
	 *
	 * Stored as `std::uint64_t`. Safe to return across a DLL boundary
	 * and between 32-bit and 64-bit modules. Not a replacement for
	 * `std::size_t` inside a single translation unit.
	 *
	 * `Size{100}` is valid. A negative integer is undefined and
	 * `assert`s when assertions are on.
	 *
	 * The numeric conversions are `Value()` and `explicit operator
	 * std::uint64_t`. There is no `size_t` conversion; the caller
	 * casts `Value()` if a host `size_t` is required.
	 *
	 * Addition and subtraction that would wrap `uint64_t` are
	 * undefined and `assert` when assertions are on.
	 *
	 * Units are IEC (`KiB`, `MiB`, …) and SI (`KB`, `MB`, …). Scale
	 * a unit with `*`: `4 * MiB`, `4.2 * KiB`, `GiB * 2`. A
	 * fractional factor rounds to the nearest byte.
	 *
	 * Scale an existing @ref Size with a positive integer:
	 * `4 * s`, `s * 4`. `s / 4` and `s % 4` return a count
	 * (`uint64_t`), not a Size: how many pieces fit, and the
	 * leftover bytes. Zero or a negative divisor/factor, or
	 * overflow, is undefined and `assert`s when assertions are on.
	 * There is no `Size * Size` and no floating-point scale of a
	 * @ref Size.
	 *
	 * `operator CString` and `operator std::string` are the IEC
	 * human-readable form (`B`, `KiB`, …). `CString` is built in the
	 * StormByte DLL. `std::string` is built in the caller’s heap from
	 * that `CString`.
	 */
	class STORMBYTE_PUBLIC Size {
		public:
			/**
			 * @name Life
			 * @{
			 */

			/**
			 * @brief Zero.
			 */
			constexpr Size() noexcept: m_value(0) {}

			/**
			 * @brief From an integer count.
			 * @tparam T Integral type.
			 * @param value Byte count.
			 * @note A negative @p value is undefined. Checked with `assert` when assertions are on.
			 */
			template<Type::Integral T>
			explicit Size(T value) noexcept;

			/**
			 * @brief Copy constructor.
			 * @param other Size to copy.
			 */
			constexpr Size(const Size& other) noexcept = default;

			/**
			 * @brief Move constructor.
			 * @param other Size to take.
			 */
			constexpr Size(Size&& other) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			constexpr ~Size() noexcept = default;

			/**
			 * @brief Copy assignment.
			 * @param other Size to copy.
			 * @return *this.
			 */
			constexpr Size& operator=(const Size& other) noexcept = default;

			/**
			 * @brief Move assignment.
			 * @param other Size to take.
			 * @return *this.
			 */
			constexpr Size& operator=(Size&& other) noexcept = default;

			/** @} */

			/**
			 * @name Observers
			 * @{
			 */

			/**
			 * @brief Stored count.
			 * @return Value as `uint64_t`.
			 */
			constexpr std::uint64_t Value() const noexcept {
				return m_value;
			}

			/** @} */

			/**
			 * @name Conversions
			 * @{
			 */

			/**
			 * @brief Stored count.
			 * @return Value as `uint64_t`.
			 */
			constexpr explicit operator std::uint64_t() const noexcept {
				return m_value;
			}

			/**
			 * @brief IEC text owned by StormByte.
			 * @return Human-readable size (`B`, `KiB`, `MiB`, `GiB`, `TiB`, `PiB`, `EiB`).
			 */
			explicit operator CString() const noexcept;

			/**
			 * @brief IEC text in the caller’s heap.
			 * @return Human-readable size.
			 */
			inline operator std::string() const {
				return static_cast<std::string>(static_cast<CString>(*this));
			}

			/** @} */

			/**
			 * @name Comparison
			 * @{
			 */

			/**
			 * @brief Numeric equality.
			 * @param other Other size.
			 * @return Whether the counts are equal.
			 */
			constexpr bool operator==(const Size& other) const noexcept = default;

			/**
			 * @brief Numeric order.
			 * @param other Other size.
			 * @return Ordering.
			 */
			constexpr std::strong_ordering operator<=>(const Size& other) const noexcept = default;

			/** @} */

			/**
			 * @name Arithmetic
			 * @{
			 */

			/**
			 * @brief Sum.
			 * @param other Addend.
			 * @return Sum.
			 * @note Overflow of `uint64_t` is undefined. Checked with `assert` when assertions are on.
			 */
			constexpr Size operator+(const Size& other) const noexcept {
				assert(m_value <= std::numeric_limits<std::uint64_t>::max() - other.m_value);
				return Size(static_cast<std::uint64_t>(m_value + other.m_value));
			}

			/**
			 * @brief Difference.
			 * @param other Subtrahend.
			 * @return Difference.
			 * @note `*this < other` is undefined. Checked with `assert` when assertions are on.
			 */
			constexpr Size operator-(const Size& other) const noexcept {
				assert(m_value >= other.m_value);
				return Size(static_cast<std::uint64_t>(m_value - other.m_value));
			}

			/**
			 * @brief Adds @p other to this size.
			 * @param other Addend.
			 * @return *this.
			 * @note Overflow of `uint64_t` is undefined. Checked with `assert` when assertions are on.
			 */
			constexpr Size& operator+=(const Size& other) noexcept {
				*this = *this + other;
				return *this;
			}

			/**
			 * @brief Subtracts @p other from this size.
			 * @param other Subtrahend.
			 * @return *this.
			 * @note `*this < other` is undefined. Checked with `assert` when assertions are on.
			 */
			constexpr Size& operator-=(const Size& other) noexcept {
				*this = *this - other;
				return *this;
			}

			/** @} */

		private:
			std::uint64_t m_value;	///< Byte count
	};

	/**
	 * @struct Unit
	 * @brief Scale factor for @ref Size (`KiB`, `KB`, …).
	 */
	struct Unit {
		std::uint64_t factor;	///< Bytes in one unit
	};

	/**
	 * @name IEC units
	 * @{
	 */
	inline constexpr Unit B{1};				///< 2^0 bytes
	inline constexpr Unit KiB{1024};			///< 2^10 bytes
	inline constexpr Unit MiB{1024ull * 1024};		///< 2^20 bytes
	inline constexpr Unit GiB{1024ull * 1024 * 1024};	///< 2^30 bytes
	inline constexpr Unit TiB{1024ull * 1024 * 1024 * 1024};			///< 2^40 bytes
	inline constexpr Unit PiB{1024ull * 1024 * 1024 * 1024 * 1024};		///< 2^50 bytes
	inline constexpr Unit EiB{1024ull * 1024 * 1024 * 1024 * 1024 * 1024};	///< 2^60 bytes
	/** @} */

	/**
	 * @name SI units
	 * @{
	 */
	inline constexpr Unit KB{1000};				///< 10^3 bytes
	inline constexpr Unit MB{1000ull * 1000};		///< 10^6 bytes
	inline constexpr Unit GB{1000ull * 1000 * 1000};	///< 10^9 bytes
	inline constexpr Unit TB{1000ull * 1000 * 1000 * 1000};			///< 10^12 bytes
	inline constexpr Unit PB{1000ull * 1000 * 1000 * 1000 * 1000};		///< 10^15 bytes
	inline constexpr Unit EB{1000ull * 1000 * 1000 * 1000 * 1000 * 1000};	///< 10^18 bytes
	/** @} */

	/**
	 * @brief Scale @p unit by an integer count.
	 * @tparam T Integral type.
	 * @param count Multiplier.
	 * @param unit Unit.
	 * @return `count * unit` as @ref Size.
	 * @note A negative @p count or overflow of `uint64_t` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::Integral T>
	Size operator*(T count, Unit unit) noexcept;

	/**
	 * @brief Scale @p unit by an integer count.
	 * @tparam T Integral type.
	 * @param unit Unit.
	 * @param count Multiplier.
	 * @return `unit * count` as @ref Size.
	 */
	template<Type::Integral T>
	Size operator*(Unit unit, T count) noexcept;

	/**
	 * @brief Scale @p unit by a non-negative real factor.
	 * @tparam T Floating-point type.
	 * @param count Multiplier.
	 * @param unit Unit.
	 * @return Nearest byte count as @ref Size.
	 * @note A negative factor or a result above `uint64_t` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::FloatingPoint T>
	Size operator*(T count, Unit unit) noexcept;

	/**
	 * @brief Scale @p unit by a non-negative real factor.
	 * @tparam T Floating-point type.
	 * @param unit Unit.
	 * @param count Multiplier.
	 * @return Nearest byte count as @ref Size.
	 */
	template<Type::FloatingPoint T>
	Size operator*(Unit unit, T count) noexcept;

	/**
	 * @brief Scale @p size by a positive integer.
	 * @tparam T Integral type.
	 * @param count Multiplier (`> 0`).
	 * @param size Size.
	 * @return `count * size`.
	 * @note `@p count <= 0` or overflow of `uint64_t` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::Integral T>
	Size operator*(T count, const Size& size) noexcept;

	/**
	 * @brief Scale @p size by a positive integer.
	 * @tparam T Integral type.
	 * @param size Size.
	 * @param count Multiplier (`> 0`).
	 * @return `size * count`.
	 */
	template<Type::Integral T>
	Size operator*(const Size& size, T count) noexcept;

	/**
	 * @brief How many pieces of @p count bytes fit in @p size.
	 * @tparam T Integral type.
	 * @param size Size.
	 * @param count Piece size in bytes (`> 0`).
	 * @return Quotient.
	 * @note `@p count <= 0` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::Integral T>
	std::uint64_t operator/(const Size& size, T count) noexcept;

	/**
	 * @brief Leftover bytes after splitting @p size into pieces of @p count bytes.
	 * @tparam T Integral type.
	 * @param size Size.
	 * @param count Piece size in bytes (`> 0`).
	 * @return Remainder.
	 * @note `@p count <= 0` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::Integral T>
	std::uint64_t operator%(const Size& size, T count) noexcept;

	/**
	 * @brief Writes the IEC text of @p size.
	 * @param stream Destination.
	 * @param size Value.
	 * @return @p stream.
	 */
	inline std::ostream& operator<<(std::ostream& stream, const Size& size) {
		return stream << static_cast<std::string>(size);
	}

	/// @cond
	extern template STORMBYTE_PUBLIC Size::Size(bool) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(char) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(signed char) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(unsigned char) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(wchar_t) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(char8_t) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(char16_t) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(char32_t) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(short) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(unsigned short) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(int) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(unsigned) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(long) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(unsigned long) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(long long) noexcept;
	extern template STORMBYTE_PUBLIC Size::Size(unsigned long long) noexcept;

	extern template Size STORMBYTE_PUBLIC operator*<bool>(bool, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char>(char, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<signed char>(signed char, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned char>(unsigned char, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<wchar_t>(wchar_t, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char8_t>(char8_t, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char16_t>(char16_t, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char32_t>(char32_t, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<short>(short, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned short>(unsigned short, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<int>(int, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned>(unsigned, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long>(long, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned long>(unsigned long, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long long>(long long, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned long long>(unsigned long long, Unit) noexcept;

	extern template Size STORMBYTE_PUBLIC operator*<bool>(Unit, bool) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char>(Unit, char) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<signed char>(Unit, signed char) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned char>(Unit, unsigned char) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<wchar_t>(Unit, wchar_t) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char8_t>(Unit, char8_t) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char16_t>(Unit, char16_t) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char32_t>(Unit, char32_t) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<short>(Unit, short) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned short>(Unit, unsigned short) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<int>(Unit, int) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned>(Unit, unsigned) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long>(Unit, long) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned long>(Unit, unsigned long) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long long>(Unit, long long) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned long long>(Unit, unsigned long long) noexcept;

	extern template Size STORMBYTE_PUBLIC operator*<float>(float, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<double>(double, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long double>(long double, Unit) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<float>(Unit, float) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<double>(Unit, double) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long double>(Unit, long double) noexcept;

	extern template Size STORMBYTE_PUBLIC operator*<bool>(bool, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char>(char, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<signed char>(signed char, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned char>(unsigned char, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<wchar_t>(wchar_t, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char8_t>(char8_t, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char16_t>(char16_t, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char32_t>(char32_t, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<short>(short, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned short>(unsigned short, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<int>(int, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned>(unsigned, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long>(long, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned long>(unsigned long, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long long>(long long, const Size&) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned long long>(unsigned long long, const Size&) noexcept;

	extern template Size STORMBYTE_PUBLIC operator*<bool>(const Size&, bool) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char>(const Size&, char) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<signed char>(const Size&, signed char) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned char>(const Size&, unsigned char) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<wchar_t>(const Size&, wchar_t) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char8_t>(const Size&, char8_t) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char16_t>(const Size&, char16_t) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<char32_t>(const Size&, char32_t) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<short>(const Size&, short) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned short>(const Size&, unsigned short) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<int>(const Size&, int) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned>(const Size&, unsigned) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long>(const Size&, long) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned long>(const Size&, unsigned long) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<long long>(const Size&, long long) noexcept;
	extern template Size STORMBYTE_PUBLIC operator*<unsigned long long>(const Size&, unsigned long long) noexcept;

	extern template std::uint64_t STORMBYTE_PUBLIC operator/<bool>(const Size&, bool) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<char>(const Size&, char) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<signed char>(const Size&, signed char) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<unsigned char>(const Size&, unsigned char) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<wchar_t>(const Size&, wchar_t) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<char8_t>(const Size&, char8_t) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<char16_t>(const Size&, char16_t) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<char32_t>(const Size&, char32_t) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<short>(const Size&, short) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<unsigned short>(const Size&, unsigned short) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<int>(const Size&, int) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<unsigned>(const Size&, unsigned) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<long>(const Size&, long) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<unsigned long>(const Size&, unsigned long) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<long long>(const Size&, long long) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator/<unsigned long long>(const Size&, unsigned long long) noexcept;

	extern template std::uint64_t STORMBYTE_PUBLIC operator%<bool>(const Size&, bool) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<char>(const Size&, char) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<signed char>(const Size&, signed char) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<unsigned char>(const Size&, unsigned char) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<wchar_t>(const Size&, wchar_t) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<char8_t>(const Size&, char8_t) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<char16_t>(const Size&, char16_t) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<char32_t>(const Size&, char32_t) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<short>(const Size&, short) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<unsigned short>(const Size&, unsigned short) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<int>(const Size&, int) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<unsigned>(const Size&, unsigned) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<long>(const Size&, long) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<unsigned long>(const Size&, unsigned long) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<long long>(const Size&, long long) noexcept;
	extern template std::uint64_t STORMBYTE_PUBLIC operator%<unsigned long long>(const Size&, unsigned long long) noexcept;
	/// @endcond
}
