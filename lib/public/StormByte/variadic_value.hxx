#pragma once

#include <StormByte/exception.hxx>

#include <variant>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include <concepts>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @brief Concept to check if a type is a valid variadic type
	 * @tparam T type
	 */
	template <typename T> concept ValidVariadicType =
		(!std::is_pointer_v<T>) &&
		(std::is_same_v<T, std::shared_ptr<typename T::element_type>> ||
			std::is_same_v<T, std::unique_ptr<typename T::element_type>> ||
			!std::is_class_v<T> ||
			std::is_class_v<T>);

	/**
	 * @class VariadicValue
	 * @deprecated Use std::variant directly instead.
	 * @brief A class that can hold a value of any of the specified types
	 * 
	 * This class is a variant that can hold a value of any of the specified types.
	 * It is useful when you need to store a value that can be of different types.
	 * 
	 * @tparam Types The types that the value can be of.
	 */
	template <ValidVariadicType... Types> class VariadicValue {
		public:
			/**
			 * @brief Default constructor.
			 */
			VariadicValue() 							= default;

			/**
			 * @brief Constructor with a value of one of the specified types.
			 * @tparam T The type of the value to store.
			 * @param value The value to store.
			 */
			template <typename T, typename = std::enable_if_t<(std::is_same_v<T, Types> || ...)>>
			explicit VariadicValue(T value) : m_values(std::move(value)) {}

			/**
			 * @brief Destructor.
			 */
			~VariadicValue() noexcept = default;

			/**
			 * @brief Copy constructor with deep copy for std::unique_ptr.
			 * @param other The VariadicValue instance to copy from.
			 */
			VariadicValue(const VariadicValue& other) {
				_processCopy(other.m_values);
			}

			/**
			 * @brief Copy assignment operator with deep copy for std::unique_ptr.
			 * @param other The VariadicValue instance to copy from.
			 * @return Reference to this object.
			 */
			VariadicValue& operator=(const VariadicValue& other) {
				if (this != &other) {
					_processCopy(other.m_values);
				}
				return *this;
			}

			/**
			 * @brief Move constructor (default behavior).
			 * @param other The VariadicValue instance to move from.
			 */
			VariadicValue(VariadicValue&&) noexcept = default;

			/**
			 * @brief Move assignment operator (default behavior).
			 * @param other The VariadicValue instance to move from.
			 * @return Reference to this object.
			 */
			VariadicValue& operator=(VariadicValue&&) noexcept = default;

			/**
			 * @brief Equality operator.
			 * @param other The other VariadicValue instance to compare with.
			 * @return True if the stored values are equal, false otherwise.
			 */
			constexpr bool operator==(const VariadicValue& other) const {
				return m_values == other.m_values;
			}

			/**
			 * @brief Inequality operator.
			 * @param other The other VariadicValue instance to compare with.
			 * @return True if the stored values are not equal, false otherwise.
			 */
			constexpr bool operator!=(const VariadicValue& other) const {
				return !(*this == other);
			}

			/**
			 * @brief Retrieves the stored value of the specified type.
			 * @tparam T The type of the value to retrieve.
			 * @return A reference to the stored value.
			 * @throws ValueTypeMismatch If the requested type does not match the stored type.
			 */
			template <typename T> T& Get() {
				constexpr bool isDirectMatch = (std::is_same_v<T, Types> || ...);
				constexpr bool isDereferencedMatch = (
					([]<typename U>() constexpr {
						if constexpr (std::is_same_v<U, std::shared_ptr<T>> || std::is_same_v<U, std::unique_ptr<T>>) {
							return true;
						}
						return false;
					}.template operator()<Types>() || ...)
				);
	
				static_assert(isDirectMatch || isDereferencedMatch,
							"Requested type is not in the variant or is not compatible with smart pointers");
	
				if constexpr (isDirectMatch) {
					if (std::holds_alternative<T>(m_values)) {
						return std::get<T>(m_values);
					}
					throw Exception("Variant does not hold the requested type");
				}
	
				if constexpr (isDereferencedMatch) {
					return std::visit([](auto& value) -> T& {
						using ValueType = std::decay_t<decltype(value)>;
						if constexpr (std::is_same_v<ValueType, std::shared_ptr<T>> || std::is_same_v<ValueType, std::unique_ptr<T>>) {
							if (value) {
								return *value;
							}
							throw Exception("Pointer is null");
						}
						throw Exception("Invalid type for dereferencing");
					}, m_values);
				}
	
				throw Exception("Value type mismatch");
			}

			/**
			 * @brief Retrieves the stored value of the specified type (const version).
			 * @tparam T The type of the value to retrieve.
			 * @return A const reference to the stored value.
			 * @throws ValueTypeMismatch If the requested type does not match the stored type.
			 */
			template <typename T> const T& Get() const {
				return const_cast<VariadicValue*>(this)->Get<T>();
			}

		private:
			/**
			 * @brief Helper type trait to check if a type is std::unique_ptr
			 * @tparam T type
			 */
			template <typename T>
			struct is_unique_ptr : std::false_type {};
		
			/**
			 * @brief Helper type trait to check if a type is std::unique_ptr
			 * @tparam T type
			 */
			template <typename T>
			struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};
		
			/**
			 * @brief Helper type trait to check if a type is std::shared_ptr
			 * @tparam T type
			 */
			template <typename T>
			struct is_shared_ptr : std::false_type {};
		
			/**
			 * @brief Helper type trait to check if a type is std::shared_ptr
			 * @tparam T type
			 */
			template <typename T>
			struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
		
			/**
			 * @brief Helper function to process deep copy logic using std::visit.
			 * @param variant The variant to process for deep copy.
			 */
			void _processCopy(const std::variant<Types...>& variant) {
				std::visit([this](const auto& value) {
					using ValueType = std::decay_t<decltype(value)>;
					if constexpr (is_unique_ptr<ValueType>::value) {
						// Handle std::unique_ptr<Foo>
						using Foo = typename ValueType::element_type;
						if constexpr (!std::is_abstract_v<Foo>) { // Only process non-abstract types
							if (value) {
								m_values = std::make_unique<Foo>(*value);
							} else {
								m_values = std::unique_ptr<Foo>(); // Assign a null unique_ptr
							}
						} else {
							static_assert(!std::is_abstract_v<Foo>, "Abstract classes cannot be copied directly");
						}
					} else if constexpr (is_shared_ptr<ValueType>::value) {
						// Copy shared_ptr (no deep copy necessary)
						m_values = value;
					} else {
						// For other types, simply copy the value
						m_values = value;
					}
				}, variant);
			}		

			std::variant<Types...> m_values;
	};
}
