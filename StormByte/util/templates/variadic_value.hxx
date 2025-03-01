#pragma once

#include <StormByte/util/exception.hxx>

#include <typeinfo>
#include <variant>

/**
 * @namespace Util
 * @brief Contains utility classes and functions.
 */
namespace StormByte::Util::Templates {
	/**
	 * @class VariadicValue
	 * @brief A class that can hold multiple types of values
	 */
	template<typename... Types> class VariadicValue {
		public:
			/**
			 * Constructor
			 * @param value value
			 */
			template <typename T, typename = std::enable_if_t<(std::is_same_v<T, Types> || ...)>>
			VariadicValue(T value): m_values(value) {}

			/**
			 * Copy constructor
			 */
			VariadicValue(const VariadicValue&) 				= default;

			/**
			 * Move constructor
			 */
			VariadicValue(VariadicValue&&) noexcept 			= default;

			/**
			 * Assignment operator
			 */
			VariadicValue& operator=(const VariadicValue&) 		= default;

			/**
			 * Move operator
			 */
			VariadicValue& operator=(VariadicValue&&) noexcept = default;

			/**
			 * Destructor
			 */
			~VariadicValue() noexcept 							= default;

			/**
			 * Gets the value
			 * @return reference to value
			 */
			template <typename T> T& Get() {
				// Static assertion to ensure T is one of the Types
            	static_assert((std::is_same_v<T, Types> || ...), "Requested type is not in the variant");

				if (std::holds_alternative<T>(m_values)) {
					return std::get<T>(m_values);
				}
				throw ValueTypeMissmatch(typeid(T).name());
			}

			/**
			 * Gets the value
			 * @return const reference to value
			 */
			template <typename T> const T& Get() const {
				// Static assertion to ensure T is one of the Types
            	static_assert((std::is_same_v<T, Types> || ...), "Requested type is not in the variant");

				if (std::holds_alternative<T>(m_values)) {
					return std::get<T>(m_values);
				}
				throw ValueTypeMissmatch(typeid(T).name());
			}

			/**
			 * Gets the value
			 * @return reference to value
			 */
			template<typename T> operator T() {
				return Get<T>();
			}

			/**
			 * Gets the value
			 * @return const reference to value
			 */
			template<typename T> operator T() const {
				return Get<T>();
			}

			/**
			 * Equality operator
			 * @param other value to compare
			 * @return bool equal?
			 */
			constexpr bool operator==(const VariadicValue& other) const {
				return m_values == other.m_values;
			}

			/**
			 * Inequality operator
			 * @param other value to compare
			 * @return bool not equal?
			 */
			constexpr bool operator!=(const VariadicValue& other) const {
				return m_values != other.m_values;
			}

			/**
			 * Checks if the value is of a certain type
			 * @return true if the value is of the type
			 */
			template <typename T> bool IsType() const {
				return std::holds_alternative<T>(m_values);
			}

			bool IsNull() const {
				return std::holds_alternative<std::nullptr_t>(m_values);
			}

		private:
			std::variant<Types...> m_values; ///< Values variant
	};
}