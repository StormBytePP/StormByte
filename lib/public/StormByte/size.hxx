#pragma once

#include <StormByte/cstring.hxx>
#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <cassert>
#include <compare>
#include <cstddef>
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
	 * Construction from a non-negative integer is implicit and
	 * `constexpr`: `Size s = 0`, `s = 16`, `{ Status::Ok, 0 }`,
	 * `constexpr Size MinWindow{16ull * 1024ull}`. A negative
	 * integer is undefined and `assert`s when assertions are on.
	 *
	 * Mixed comparisons and addition/subtraction with integers work
	 * on both sides (`5 == s`, `s == 5`, `16 + s`, `s + 16`) via
	 * hidden friends. There is no implicit conversion *to* an
	 * integer. `Value()`, `explicit operator uint64_t`,
	 * `explicit operator size_t` (only when `size_t` is not
	 * `uint64_t`) and `explicit operator ptrdiff_t` are the numeric
	 * exits. A value that does not fit the target type is undefined
	 * and `assert`s when assertions are on.
	 *
	 * Addition, subtraction and unit scale (`4 * MiB`, `s * 4`,
	 * `s / 4`, `s % 4`) are `constexpr`. Overflow, a negative
	 * integer or a zero divisor is undefined and `assert`s when
	 * assertions are on. There is no `Size * Size` and no
	 * floating-point scale of a @ref Size.
	 *
	 * Units are IEC (`KiB`, `MiB`, …) and SI (`KB`, `MB`, …). A
	 * fractional factor rounds to the nearest byte.
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
			constexpr Size(T value) noexcept: m_value(0) {
				if constexpr (Type::Signed<T>)
					assert(value >= static_cast<T>(0));
				m_value = static_cast<std::uint64_t>(value);
			}

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

			/**
			 * @brief Assign an integer count.
			 * @tparam T Integral type.
			 * @param value Byte count.
			 * @return *this.
			 * @note A negative @p value is undefined. Checked with `assert` when assertions are on.
			 */
			template<Type::Integral T>
			constexpr Size& operator=(T value) noexcept {
				*this = Size(value);
				return *this;
			}

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
			 * @brief Stored count as host `size_t`.
			 * @tparam T Must be `std::size_t` and a different type from `uint64_t`.
			 * @return Value as `std::size_t`.
			 * @note Ill-formed when `size_t` and `uint64_t` are the same type;
			 *       `static_cast<std::size_t>` then uses `operator uint64_t`.
			 *       A count above `size_t` is undefined. Checked with `assert` when assertions are on.
			 */
			template<typename T>
			requires Type::SameAs<T, std::size_t> && (!Type::SameAs<std::size_t, std::uint64_t>)
			constexpr explicit operator T() const noexcept {
				assert(m_value <= static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max()));
				return static_cast<std::size_t>(m_value);
			}

			/**
			 * @brief Stored count as `ptrdiff_t` (non-negative offsets).
			 * @return Value as `std::ptrdiff_t`.
			 * @note A count above `ptrdiff_t` is undefined. Checked with `assert` when assertions are on.
			 */
			constexpr explicit operator std::ptrdiff_t() const noexcept {
				assert(m_value <= static_cast<std::uint64_t>(std::numeric_limits<std::ptrdiff_t>::max()));
				return static_cast<std::ptrdiff_t>(m_value);
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
			 * @param lhs Left count.
			 * @param rhs Right count.
			 * @return Whether the counts are equal.
			 */
			friend constexpr bool operator==(Size lhs, Size rhs) noexcept {
				return lhs.m_value == rhs.m_value;
			}

			/**
			 * @brief Numeric order.
			 * @param lhs Left count.
			 * @param rhs Right count.
			 * @return Ordering.
			 */
			friend constexpr std::strong_ordering operator<=>(Size lhs, Size rhs) noexcept {
				return lhs.m_value <=> rhs.m_value;
			}

			/** @} */

			/**
			 * @name Arithmetic
			 * @{
			 */

			/**
			 * @brief Sum.
			 * @param lhs Left addend.
			 * @param rhs Right addend.
			 * @return Sum.
			 * @note Overflow of `uint64_t` is undefined. Checked with `assert` when assertions are on.
			 */
			friend constexpr Size operator+(Size lhs, Size rhs) noexcept {
				assert(lhs.m_value <= std::numeric_limits<std::uint64_t>::max() - rhs.m_value);
				return Size(static_cast<std::uint64_t>(lhs.m_value + rhs.m_value));
			}

			/**
			 * @brief Difference.
			 * @param lhs Minuend.
			 * @param rhs Subtrahend.
			 * @return Difference.
			 * @note `lhs < rhs` is undefined. Checked with `assert` when assertions are on.
			 */
			friend constexpr Size operator-(Size lhs, Size rhs) noexcept {
				assert(lhs.m_value >= rhs.m_value);
				return Size(static_cast<std::uint64_t>(lhs.m_value - rhs.m_value));
			}

			/**
			 * @brief Adds @p other to this size.
			 * @param other Addend.
			 * @return *this.
			 * @note Overflow of `uint64_t` is undefined. Checked with `assert` when assertions are on.
			 */
			constexpr Size& operator+=(Size other) noexcept {
				*this = *this + other;
				return *this;
			}

			/**
			 * @brief Subtracts @p other from this size.
			 * @param other Subtrahend.
			 * @return *this.
			 * @note `*this < other` is undefined. Checked with `assert` when assertions are on.
			 */
			constexpr Size& operator-=(Size other) noexcept {
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
	constexpr Size operator*(T count, Unit unit) noexcept {
		if constexpr (Type::Signed<T>)
			assert(count >= T{0});
		const std::uint64_t factor = static_cast<std::uint64_t>(count);
		assert(unit.factor == 0 || factor <= std::numeric_limits<std::uint64_t>::max() / unit.factor);
		return Size(factor * unit.factor);
	}

	/**
	 * @brief Scale @p unit by an integer count.
	 * @tparam T Integral type.
	 * @param unit Unit.
	 * @param count Multiplier.
	 * @return `unit * count` as @ref Size.
	 */
	template<Type::Integral T>
	constexpr Size operator*(Unit unit, T count) noexcept {
		return count * unit;
	}

	/**
	 * @brief Scale @p unit by a non-negative real factor.
	 * @tparam T Floating-point type.
	 * @param count Multiplier.
	 * @param unit Unit.
	 * @return Nearest byte count as @ref Size.
	 * @note A negative factor or a result above `uint64_t` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::FloatingPoint T>
	constexpr Size operator*(T count, Unit unit) noexcept {
		assert(count >= T{0});
		const long double product = static_cast<long double>(count) * static_cast<long double>(unit.factor);
		assert(product <= static_cast<long double>(std::numeric_limits<std::uint64_t>::max()));
		return Size(static_cast<std::uint64_t>(product + 0.5L));
	}

	/**
	 * @brief Scale @p unit by a non-negative real factor.
	 * @tparam T Floating-point type.
	 * @param unit Unit.
	 * @param count Multiplier.
	 * @return Nearest byte count as @ref Size.
	 */
	template<Type::FloatingPoint T>
	constexpr Size operator*(Unit unit, T count) noexcept {
		return count * unit;
	}

	/**
	 * @brief Scale @p size by a positive integer.
	 * @tparam T Integral type.
	 * @param count Multiplier (`> 0`).
	 * @param size Size.
	 * @return `count * size`.
	 * @note `@p count <= 0` or overflow of `uint64_t` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::Integral T>
	constexpr Size operator*(T count, const Size& size) noexcept {
		if constexpr (Type::Signed<T>)
			assert(count > T{0});
		else
			assert(count != T{0});
		const std::uint64_t factor = static_cast<std::uint64_t>(count);
		assert(size.Value() == 0 || factor <= std::numeric_limits<std::uint64_t>::max() / size.Value());
		return Size(factor * size.Value());
	}

	/**
	 * @brief Scale @p size by a positive integer.
	 * @tparam T Integral type.
	 * @param size Size.
	 * @param count Multiplier (`> 0`).
	 * @return `size * count`.
	 */
	template<Type::Integral T>
	constexpr Size operator*(const Size& size, T count) noexcept {
		return count * size;
	}

	/**
	 * @brief How many pieces of @p count bytes fit in @p size.
	 * @tparam T Integral type.
	 * @param size Size.
	 * @param count Piece size in bytes (`> 0`).
	 * @return Quotient.
	 * @note `@p count <= 0` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::Integral T>
	constexpr std::uint64_t operator/(const Size& size, T count) noexcept {
		if constexpr (Type::Signed<T>)
			assert(count > T{0});
		else
			assert(count != T{0});
		return size.Value() / static_cast<std::uint64_t>(count);
	}

	/**
	 * @brief Leftover bytes after splitting @p size into pieces of @p count bytes.
	 * @tparam T Integral type.
	 * @param size Size.
	 * @param count Piece size in bytes (`> 0`).
	 * @return Remainder.
	 * @note `@p count <= 0` is undefined. Checked with `assert` when assertions are on.
	 */
	template<Type::Integral T>
	constexpr std::uint64_t operator%(const Size& size, T count) noexcept {
		if constexpr (Type::Signed<T>)
			assert(count > T{0});
		else
			assert(count != T{0});
		return size.Value() % static_cast<std::uint64_t>(count);
	}

	/**
	 * @brief Writes the IEC text of @p size.
	 * @param stream Destination.
	 * @param size Value.
	 * @return @p stream.
	 */
	inline std::ostream& operator<<(std::ostream& stream, const Size& size) {
		return stream << static_cast<std::string>(size);
	}
}
