#pragma once

#include <StormByte/exception.hxx>
#include <StormByte/expected.hxx>
#include <StormByte/type_traits.hxx>

#include <cstring>
#include <optional>
#include <span>
#include <utility>
#include <vector>


namespace {
	// Helper function to append one vector to another
	inline void AppendVector(std::vector<std::byte>& dest, std::vector<std::byte>&& src) {
		dest.insert(dest.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
	}
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
	 * @class Serializable
	 * @brief The class to serialize and deserialize data.
	 * @tparam T The type of the data to serialize and deserialize.
	 */
	template<typename T>
	class Serializable {
		using DecayedT = std::decay_t<T>;	///< The decayed type of the data to serialize and deserialize.
		
		public:
			/**
			 * @brief The constructor of the Serializable class.
			 * @param data The data to serialize.
			 */
			Serializable(const DecayedT& data) noexcept : m_data(data) {}

			/**
			 * @brief The copy constructor of the Serializable class.
			 * @param other The other Serializable to copy.
			 */
			Serializable(const Serializable& other) noexcept 				= delete;

			/**
			 * @brief The move constructor of the Serializable class.
			 * @param other The other Serializable to move.
			 */
			Serializable(Serializable&& other) noexcept 					= delete;

			/**
			 * @brief The destructor of the Serializable class.
			 */
			~Serializable() noexcept 										= default;

			/**
			 * @brief The assignment operator of the Serializable class.
			 * @param other The other Serializable to assign.
			 * @return The reference to the assigned Serializable.
			 */
			Serializable& operator=(const Serializable& other) noexcept 	= delete;

			/**
			 * @brief The move assignment operator of the Serializable class.
			 * @param other The other Serializable to assign.
			 * @return The reference to the assigned Serializable.
			 */
			Serializable& operator=(Serializable&& other) noexcept 			= delete;

			/**
			 * @brief Serializes the data into a byte vector.
			 * 
			 * This function automatically selects the appropriate serialization method based on
			 * the type of data: trivial types, containers, pairs, optionals, or complex types.
			 * 
			 * @return A vector of bytes containing the serialized data.
			 */
			std::vector<std::byte>											Serialize() const noexcept {
				if constexpr (std::is_trivially_copyable_v<T>) {
					return SerializeTrivial();
				} else if constexpr (is_container<T>::value) {
					return SerializeContainer();
				} else if constexpr (is_pair<T>::value) {
					return SerializePair();
				} else if constexpr (is_optional<T>::value) {
					return SerializeOptional();
				} else {
					return SerializeComplex();
				}
			}

			/**
			 * @brief Deserializes data from a byte span.
			 * 
			 * This function is a convenience overload that accepts a std::span of bytes
			 * and forwards it to the vector-based Deserialize function.
			 * 
			 * @param data The byte span containing the serialized data.
			 * @return An Expected object containing the deserialized data on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError> 				Deserialize(std::span<const std::byte> data) noexcept {
				if constexpr (std::is_trivially_copyable_v<T>) {
					return DeserializeTrivial(data);
				} else if constexpr (is_container<T>::value) {
					return DeserializeContainer(data);
				} else if constexpr (is_pair<T>::value) {
					return DeserializePair(data);
				} else if constexpr (is_optional<T>::value) {
					return DeserializeOptional(data);
				} else {
					return DeserializeComplex(data);
				}
			}

			/**
			 * @brief Deserializes data from a byte vector.
			 * 
			 * This function automatically selects the appropriate deserialization method based on
			 * the type of data: trivial types, containers, pairs, optionals, or complex types.
			 * 
			 * @param data The byte vector containing the serialized data.
			 * @return An Expected object containing the deserialized data on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError> 				Deserialize(const std::vector<std::byte>& data) noexcept {
				return Deserialize(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Calculates the serialized size of the data.
			 * 
			 * This function computes the size in bytes that the serialized data will occupy.
			 * 
			 * @param data The data to calculate the size for.
			 * @return The size in bytes of the serialized data.
			 */
			static std::size_t												Size(const DecayedT& data) noexcept {
				if constexpr (std::is_trivially_copyable_v<T>) {
					return sizeof(data);
				} else if constexpr (is_container<T>::value) {
					return SizeContainer(data);
				} else if constexpr (is_pair<T>::value) {
					return SizePair(data);
				} else if constexpr (is_optional<T>::value) {
					return SizeOptional(data);
				} else {
					return SizeComplex(data);
				}
			}

		private:
			const DecayedT& m_data;											///< The data to serialize.

			/**
			 * @brief Serializes trivially copyable data.
			 * 
			 * This function performs a direct memory copy of the data into a byte vector.
			 * Only used for types that satisfy std::is_trivially_copyable_v.
			 * 
			 * @return A vector of bytes containing the serialized data.
			 */
			std::vector<std::byte>											SerializeTrivial() const noexcept {
				return { reinterpret_cast<const std::byte*>(&m_data), reinterpret_cast<const std::byte*>(&m_data) + sizeof(m_data) };
			}

			/**
			 * @brief Serializes complex data types.
			 * 
			 * This function handles serialization of complex types that don't fall into
			 * the trivial, container, pair, or optional categories.
			 * 
			 * @return A vector of bytes containing the serialized data.
			 */
			std::vector<std::byte>											SerializeComplex() const noexcept;

			/**
			 * @brief Serializes container data.
			 * 
			 * This function serializes container types (vector, list, etc.) by first serializing
			 * the size of the container, followed by each element.
			 * 
			 * @return A vector of bytes containing the serialized container data.
			 */
			std::vector<std::byte>											SerializeContainer() const noexcept {
				std::size_t size = m_data.size();
				Serializable<std::size_t> size_serial(size);
				std::vector<std::byte> buffer = size_serial.Serialize();
				for (const auto& element: m_data) {
					Serializable<std::decay_t<decltype(element)>> element_serial(element);
					AppendVector(buffer, element_serial.Serialize());
				}
				return buffer;
			}

			/**
			 * @brief Serializes pair data.
			 * 
			 * This function serializes std::pair types by serializing the first element
			 * followed by the second element.
			 * 
			 * @return A vector of bytes containing the serialized pair data.
			 */
			std::vector<std::byte>											SerializePair() const noexcept {
				Serializable<std::decay_t<typename T::first_type>> first_serial(m_data.first);
				Serializable<std::decay_t<typename T::second_type>> second_serial(m_data.second);
				std::vector<std::byte> buffer = first_serial.Serialize();
				AppendVector(buffer, second_serial.Serialize());
				return buffer;
			}

			/**
			 * @brief Serializes optional data.
			 * 
			 * This function serializes std::optional types by first serializing a boolean
			 * indicating whether the optional has a value, followed by the value itself if present.
			 * 
			 * @return A vector of bytes containing the serialized optional data.
			 */
			std::vector<std::byte>											SerializeOptional() const noexcept {
				bool has_value = m_data.has_value();
				std::vector<std::byte> buffer = std::move(Serializable<bool>(has_value).Serialize());
				if (m_data.has_value()) {
					Serializable<std::decay_t<decltype(m_data.value())>> value_serial(m_data.value());
					AppendVector(buffer, value_serial.Serialize());
				}
				return buffer;
			}

			/**
			 * @brief Calculates the serialized size of complex data.
			 * 
			 * This function computes the size in bytes for complex types that don't fall into
			 * the trivial, container, pair, or optional categories.
			 * 
			 * @param data The data to calculate the size for.
			 * @return The size in bytes of the serialized data.
			 */
			static std::size_t												SizeComplex(const DecayedT& data) noexcept;

			/**
			 * @brief Calculates the serialized size of container data.
			 * 
			 * This function computes the size in bytes for container types, including
			 * the size field and all contained elements.
			 * 
			 * @param data The container data to calculate the size for.
			 * @return The size in bytes of the serialized container data.
			 */
			static std::size_t												SizeContainer(const DecayedT& data) noexcept {
				std::size_t size = sizeof(std::size_t);
				for (const auto& element: data) {
					size += Serializable<std::decay_t<decltype(element)>>::Size(element);
				}
				return size;
			}

			/**
			 * @brief Calculates the serialized size of pair data.
			 * 
			 * This function computes the size in bytes for std::pair types by summing
			 * the sizes of both elements.
			 * 
			 * @param data The pair data to calculate the size for.
			 * @return The size in bytes of the serialized pair data.
			 */
			static std::size_t												SizePair(const DecayedT& data) noexcept {
				return
					Serializable<std::decay_t<typename T::first_type>>::Size(data.first) +
					Serializable<std::decay_t<typename T::second_type>>::Size(data.second);
			}

			/**
			 * @brief Calculates the serialized size of optional data.
			 * 
			 * This function computes the size in bytes for std::optional types, including
			 * the boolean flag and the contained value if present.
			 * 
			 * @param data The optional data to calculate the size for.
			 * @return The size in bytes of the serialized optional data.
			 */
			static std::size_t 												SizeOptional(const DecayedT& data) noexcept {
				std::size_t size = sizeof(bool);
				if (data.has_value()) {
					size += Serializable<std::decay_t<decltype(data.value())>>::Size(data.value());
				}
				return size;
			}

			/**
			 * @brief Deserializes trivially copyable data.
			 * 
			 * This function performs a direct memory copy from the byte vector to reconstruct
			 * the original data. Only used for types that satisfy std::is_trivially_copyable_v.
			 * 
			 * @param data The byte vector containing the serialized data.
			 * @return An Expected object containing the deserialized data on success,
			 *         or a DeserializeError if the data is insufficient.
			 */
			static StormByte::Expected<T, DeserializeError>						DeserializeTrivial(std::span<const std::byte> data) noexcept {
				if (data.size() < sizeof(T))
					return StormByte::Unexpected<DeserializeError>("Insufficient data for deserialization");
				T result;
				std::memcpy(&result, data.data(), sizeof(T));
				return result;
			}

			/**
			 * @brief Deserializes trivially copyable data.
			 * 
			 * This function performs a direct memory copy from the byte vector to reconstruct
			 * the original data. Only used for types that satisfy std::is_trivially_copyable_v.
			 * 
			 * @param data The byte vector containing the serialized data.
			 * @return An Expected object containing the deserialized data on success,
			 *         or a DeserializeError if the data is insufficient.
			 */
			static StormByte::Expected<T, DeserializeError>						DeserializeTrivial(const std::vector<std::byte>& data) noexcept {
				return DeserializeTrivial(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Deserializes complex data types.
			 * 
			 * This function handles deserialization of complex types that don't fall into
			 * the trivial, container, pair, or optional categories.
			 * 
			 * @param data The byte vector containing the serialized data.
			 * @return An Expected object containing the deserialized data on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError>						DeserializeComplex(std::span<const std::byte> data) noexcept;

			/**
			 * @brief Deserializes complex data types.
			 * 
			 * This function handles deserialization of complex types that don't fall into
			 * the trivial, container, pair, or optional categories.
			 * 
			 * @param data The byte vector containing the serialized data.
			 * @return An Expected object containing the deserialized data on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError>						DeserializeComplex(const std::vector<std::byte>& data) noexcept {
				return DeserializeComplex(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Deserializes container data.
			 * 
			 * This function deserializes container types by first reading the size,
			 * then deserializing each element and inserting it into the container.
			 * 
			 * @param data The byte vector containing the serialized container data.
			 * @return An Expected object containing the deserialized container on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError> 				DeserializeContainer(std::span<const std::byte> data) noexcept {
				std::size_t offset = 0;
				
				// Deserialize the container size
				if (offset + sizeof(std::size_t) > data.size())
					return StormByte::Unexpected<DeserializeError>("Insufficient data for container size");
				
				auto expected_container_size = Serializable<std::size_t>::Deserialize(data.subspan(offset, sizeof(std::size_t)));
				if (!expected_container_size)
					return StormByte::Unexpected(expected_container_size.error());
				
				std::size_t size = expected_container_size.value();
				offset += sizeof(std::size_t);
	
				T container;
				for (std::size_t i = 0; i < size; ++i) {
					using ElementT = std::decay_t<typename T::value_type>;
					
					if (offset >= data.size())
						return StormByte::Unexpected<DeserializeError>("Insufficient data for container element");
					
					auto expected_element = Serializable<ElementT>::Deserialize(data.subspan(offset));
					if (!expected_element)
						return StormByte::Unexpected(expected_element.error());

					// Calculate the actual size of the deserialized element
					std::size_t element_size = Serializable<ElementT>::Size(expected_element.value());
					container.insert(container.end(), std::move(expected_element.value()));
					offset += element_size;
				}
				return container;
			}

			/**
			 * @brief Deserializes container data.
			 * 
			 * This function deserializes container types by first reading the size,
			 * then deserializing each element and inserting it into the container.
			 * 
			 * @param data The byte vector containing the serialized container data.
			 * @return An Expected object containing the deserialized container on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError> 				DeserializeContainer(const std::vector<std::byte>& data) noexcept {
				return DeserializeContainer(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Deserializes pair data.
			 * 
			 * This function deserializes std::pair types by deserializing the first element
			 * followed by the second element.
			 * 
			 * @param data The byte vector containing the serialized pair data.
			 * @return An Expected object containing the deserialized pair on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError>						DeserializePair(std::span<const std::byte> data) noexcept {
				using FirstT = std::decay_t<typename T::first_type>;
				using SecondT = std::decay_t<typename T::second_type>;
				std::size_t offset = 0;
	
				// Deserialize first element
				auto expected_first = Serializable<FirstT>::Deserialize(data.subspan(offset));
				if (!expected_first)
					return StormByte::Unexpected(expected_first.error());
				
				offset += Serializable<FirstT>::Size(expected_first.value());
	
				// Deserialize second element
				if (offset >= data.size())
					return StormByte::Unexpected<DeserializeError>("Insufficient data for pair second element");
				
				auto expected_second = Serializable<SecondT>::Deserialize(data.subspan(offset));
				if (!expected_second)
					return StormByte::Unexpected(expected_second.error());
	
				return T { std::move(expected_first.value()), std::move(expected_second.value()) };
			}

			/**
			 * @brief Deserializes pair data.
			 * 
			 * This function deserializes std::pair types by deserializing the first element
			 * followed by the second element.
			 * 
			 * @param data The byte vector containing the serialized pair data.
			 * @return An Expected object containing the deserialized pair on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError>						DeserializePair(const std::vector<std::byte>& data) noexcept {
				return DeserializePair(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Deserializes optional data.
			 * 
			 * This function deserializes std::optional types by first reading the boolean flag
			 * indicating whether a value is present, then deserializing the value if it exists.
			 * 
			 * @param data The byte vector containing the serialized optional data.
			 * @return An Expected object containing the deserialized optional on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError>						DeserializeOptional(std::span<const std::byte> data) noexcept {
				std::size_t offset = 0;
				
				// Deserialize the has_value boolean
				if (offset + sizeof(bool) > data.size())
					return StormByte::Unexpected<DeserializeError>("Insufficient data for optional flag");
				
				auto expected_has_value = Serializable<bool>::Deserialize(data.subspan(offset, sizeof(bool)));
				if (!expected_has_value)
					return StormByte::Unexpected(expected_has_value.error());
				
				offset += sizeof(bool);
	
				if (expected_has_value.value()) {
					if (offset >= data.size())
						return StormByte::Unexpected<DeserializeError>("Insufficient data for optional value");
					
					using ValueT = std::decay_t<typename T::value_type>;
					auto expected_value = Serializable<ValueT>::Deserialize(data.subspan(offset));
					if (!expected_value)
						return StormByte::Unexpected(expected_value.error());
	
					return T { std::move(expected_value.value()) };
				} else {
					return T {};
				}
			}

			/**
			 * @brief Deserializes optional data.
			 * 
			 * This function deserializes std::optional types by first reading the boolean flag
			 * indicating whether a value is present, then deserializing the value if it exists.
			 * 
			 * @param data The byte vector containing the serialized optional data.
			 * @return An Expected object containing the deserialized optional on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError>						DeserializeOptional(const std::vector<std::byte>& data) noexcept {
				return DeserializeOptional(std::span<const std::byte>(data.data(), data.size()));
			}
	};
}
