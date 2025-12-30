#include <StormByte/type_traits.hxx>

using namespace StormByte;

/**
 * @brief Bitwise OR operator for unsigned enum classes.
 * @tparam E The enumeration type.
 * @param a First enum value.
 * @param b Second enum value.
 * @return The result of the bitwise OR operation.
 *
 * This operator allows combining flags of an unsigned enum class using the bitwise OR operator.
 */
template <Type::UnsignedEnum E>
constexpr E operator|(E a, E b) noexcept {
	return static_cast<E>(Type::ToUnderlying(a) | Type::ToUnderlying(b));
}

/**
 * @brief Bitwise AND operator for unsigned enum classes.
 * @tparam E The enumeration type.
 * @param a First enum value.
 * @param b Second enum value.
 * @return The result of the bitwise AND operation.
 *
 * This operator allows checking common flags of an unsigned enum class using the bitwise AND operator.
 */
template <Type::UnsignedEnum E>
constexpr E operator&(E a, E b) noexcept {
	return static_cast<E>(Type::ToUnderlying(a) & Type::ToUnderlying(b));
}

/**
 * @brief Bitwise XOR operator for unsigned enum classes.
 * @tparam E The enumeration type.
 * @param a First enum value.
 * @param b Second enum value.
 * @return The result of the bitwise XOR operation.
 *
 * This operator allows toggling flags of an unsigned enum class using the bitwise XOR operator.
 */
template <Type::UnsignedEnum E>
constexpr E operator^(E a, E b) noexcept {
	return static_cast<E>(Type::ToUnderlying(a) ^ Type::ToUnderlying(b));
}

/**
 * @brief Bitwise NOT operator for unsigned enum classes.
 * @tparam E The enumeration type.
 * @param a Enum value to invert.
 * @return The result of the bitwise NOT operation.
 *
 * This operator allows inverting flags of an unsigned enum class using the bitwise NOT operator.
 */
template <Type::UnsignedEnum E>
constexpr E operator~(E a) noexcept {
	return static_cast<E>(~Type::ToUnderlying(a));
}

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @brief Bitmask class template for managing enum class flags.
	 * @tparam Derived The derived class type.
	 * @tparam E The enumeration type representing the flags.
	 *
	 * This class provides bitwise operations for enum class types,
	 * allowing easy manipulation of flag combinations.
	 *
	 * Example usage:
	 * @code
	 * enum class MyFlags : uint8_t {
	 *     FlagA = 0x01,
	 *     FlagB = 0x02,
	 *     FlagC = 0x04
	 * };
	 *
	 * class MyBitmask : public Bitmask<MyBitmask, MyFlags> {
	 * public:
	 *     using Bitmask<MyBitmask, MyFlags>::Bitmask;
	 * };
	 *
	 * MyBitmask mask(MyFlags::FlagA);
	 * mask |= MyBitmask(MyFlags::FlagB);
	 * if (mask.Has(MyFlags::FlagA)) { ... }
	 * @endcode
	 */
	template<typename Derived, Type::UnsignedEnum E>
	class Bitmask {
		public:
			/** 
			 * @brief Default constructor initializes the bitmask to zero.
			 */
			constexpr Bitmask() noexcept: m_value(static_cast<E>(0)) {}

			/** 
			 * @brief Constructor initializes the bitmask with a specific enum value.
			 * @param value The initial enum value for the bitmask.
			 */
			constexpr Bitmask(E value) noexcept: m_value(value) {}

			/** 
			 * @brief Copy constructor.
			 * @param other The other Bitmask to copy from.
			 */
			constexpr Bitmask(const Bitmask& other) noexcept = default;

			/** 
			 * @brief Move constructor.
			 * @param other The other Bitmask to move from.
			 */
			constexpr Bitmask(Bitmask&& other) noexcept = default;

			/** 
			 * @brief Destructor.
			 */
			constexpr virtual ~Bitmask() noexcept = default;

			/** 
			 * @brief Copy assignment operator.
			 * @param other The other Bitmask to copy from.
			 * @return Reference to this Bitmask.
			 */
			constexpr Bitmask& operator=(const Bitmask& other) noexcept = default;

			/** 
			 * @brief Move assignment operator.
			 * @param other The other Bitmask to move from.
			 * @return Reference to this Bitmask.
			 */
			constexpr Bitmask& operator=(Bitmask&& other) noexcept = default;

			/** 
			 * @brief Equality operator.
			 * @param other The other Bitmask to compare with.
			 * @return True if the bitmasks are equal, false otherwise.
			 */
			constexpr bool operator==(const Bitmask& other) const noexcept {
				return m_value == other.m_value;
			}

			/** 
			 * @brief Inequality operator.
			 * @param other The other Bitmask to compare with.
			 * @return True if the bitmasks are not equal, false otherwise.
			 */
			constexpr bool operator!=(const Bitmask& other) const noexcept {
				return m_value != other.m_value;
			}

			/** 
			 * @brief Bitwise OR operator.
			 * @param other The other Bitmask to combine with.
			 * @return A new Bitmask representing the result of the bitwise OR operation.
			 */
			constexpr Derived operator|(const Bitmask& other) const noexcept {
				return Derived(m_value | other.m_value);
			}

			/** 
			 * @brief Bitwise AND operator.
			 * @param other The other Bitmask to combine with.
			 * @return A new Bitmask representing the result of the bitwise AND operation.
			 */
			constexpr Derived operator&(const Bitmask& other) const noexcept {
				return Derived(m_value & other.m_value);
			}

			/** 
			 * @brief Bitwise XOR operator.
			 * @param other The other Bitmask to combine with.
			 * @return A new Bitmask representing the result of the bitwise XOR operation.
			 */
			constexpr Derived operator^(const Bitmask& other) const noexcept {
				return Derived(m_value ^ other.m_value);
			}

			/** 
			 * @brief Bitwise NOT operator.
			 * @return A new Bitmask representing the result of the bitwise NOT operation.
			 */
			constexpr Derived operator~() const noexcept {
				return Derived(~m_value);
			}

			/** 
			 * @brief Bitwise OR assignment operator.
			 * @param other The other Bitmask to combine with.
			 * @return Reference to this Bitmask after the operation.
			 */
			constexpr Bitmask& operator|=(const Bitmask& other) noexcept {
				m_value = m_value | other.m_value;
				return *this;
			}

			/** 
			 * @brief Bitwise AND assignment operator.
			 * @param other The other Bitmask to combine with.
			 * @return Reference to this Bitmask after the operation.
			 */
			constexpr Bitmask& operator&=(const Bitmask& other) noexcept {
				m_value = m_value & other.m_value;
				return *this;
			}

			/** 
			 * @brief Bitwise XOR assignment operator.
			 * @param other The other Bitmask to combine with.
			 * @return Reference to this Bitmask after the operation.
			 */
			constexpr Bitmask& operator^=(const Bitmask& other) noexcept {
				m_value = m_value ^ other.m_value;
				return *this;
			}

			/** 
			 * @brief Add a flag to the bitmask.
			 * @param value The enum value to add.
			 */
			constexpr void Add(E value) noexcept {
				m_value = m_value | value;
			}

			/** 
			 * @brief Remove a flag from the bitmask.
			 * @param value The enum value to remove.
			 */
			constexpr void Remove(E value) noexcept {
				m_value = m_value & ~value;
			}

			/** 
			 * @brief Get the current value of the bitmask.
			 * @return The current enum value of the bitmask.
			 */
			constexpr E Value() const noexcept {
				return m_value;
			}

			/** 
			 * @brief Check if the bitmask has a specific flag set.
			 * @param value The enum value to check.
			 * @return True if the flag is set, false otherwise.
			 */
			constexpr bool Has(E value) const noexcept {
				return (m_value & value) == value;
			}

			/** 
			 * @brief Check if the bitmask has all flags set from another bitmask.
			 * @param other The other Bitmask to check against.
			 * @return True if all flags are set, false otherwise.
			 */
			constexpr bool Has(const Bitmask& other) const noexcept {
				return Has(other.m_value);
			}

			/** 
			 * @brief Check if any of the specified flags are set in the bitmask.
			 * @param value The enum value(s) to check.
			 * @return True if any of the flags are set, false otherwise.
			 */
			constexpr bool HasAny(E value) const noexcept {
				return (m_value & value) != static_cast<E>(0);
			}

			/** 
			 * @brief Check if any of the flags from another bitmask are set in this bitmask.
			 * @param other The other Bitmask to check against.
			 * @return True if any of the flags are set, false otherwise.
			 */
			constexpr bool HasAny(const Bitmask& other) const noexcept {
				return HasAny(other.m_value);
			}

			/** 
			 * @brief Check if none of the specified flags are set in the bitmask.
			 * @param value The enum value(s) to check.
			 * @return True if none of the flags are set, false otherwise.
			 */
			constexpr bool HasNone(E value) const noexcept {
				return (m_value & value) == static_cast<E>(0);
			}

			/** 
			 * @brief Check if none of the flags from another bitmask are set in this bitmask.
			 * @param other The other Bitmask to check against.
			 * @return True if none of the flags are set, false otherwise.
			 */
			constexpr bool HasNone(const Bitmask& other) const noexcept {
				return HasNone(other.m_value);
			}

		protected:
			E m_value;
	};
}