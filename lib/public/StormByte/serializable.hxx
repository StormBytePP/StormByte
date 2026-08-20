/*
* Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
*
* This file is part of StormByte.
*
* StormByte is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* StormByte is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with StormByte. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <StormByte/exception.hxx>
#include <StormByte/expected.hxx>
#include <StormByte/helpers.hxx>
#include <StormByte/type_traits.hxx>

#include <bit>
#include <cstring>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>

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
	 *
	 * Dispatch order is intentional: optional / pair / container are handled
	 * *before* trivially-copyable.  Some standard-library types (notably
	 * std::optional with a trivial T) may report is_trivially_copyable; copying
	 * their object representation would include indeterminate padding and yield
	 * non-deterministic wire formats (observed on MSVC).
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
			Serializable(const Serializable& other) noexcept = delete;

			/**
			 * @brief The move constructor of the Serializable class.
			 * @param other The other Serializable to move.
			 */
			Serializable(Serializable&& other) noexcept = delete;

			/**
			 * @brief The destructor of the Serializable class.
			 */
			~Serializable() noexcept = default;

			/**
			 * @brief The assignment operator of the Serializable class.
			 * @param other The other Serializable to assign.
			 * @return The reference to the assigned Serializable.
			 */
			Serializable& operator=(const Serializable& other) noexcept = delete;

			/**
			 * @brief The move assignment operator of the Serializable class.
			 * @param other The other Serializable to assign.
			 * @return The reference to the assigned Serializable.
			 */
			Serializable& operator=(Serializable&& other) noexcept = delete;

			/**
			 * @brief Serializes the data into a byte vector.
			 *
			 * Selects the path by type category. Structured types (optional,
			 * pair, container) are preferred over a raw trivial memcpy so that
			 * padding bytes never appear on the wire.
			 *
			 * @return A vector of bytes containing the serialized data.
			 */
			std::vector<std::byte> Serialize() const noexcept {
				if constexpr (is_optional<T>::value) {
					return SerializeOptional();
				} else if constexpr (is_pair<T>::value) {
					return SerializePair();
				} else if constexpr (is_container<T>::value) {
					return SerializeContainer();
				} else if constexpr (std::is_trivially_copyable_v<T>) {
					return SerializeTrivial();
				} else {
					return SerializeComplex();
				}
			}

			/**
			 * @brief Deserializes data from a byte span.
			 *
			 * @param data The byte span containing the serialized data.
			 * @return An Expected object containing the deserialized data on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError> Deserialize(std::span<const std::byte> data) noexcept {
				if constexpr (is_optional<T>::value) {
					return DeserializeOptional(data);
				} else if constexpr (is_pair<T>::value) {
					return DeserializePair(data);
				} else if constexpr (is_container<T>::value) {
					return DeserializeContainer(data);
				} else if constexpr (std::is_trivially_copyable_v<T>) {
					return DeserializeTrivial(data);
				} else {
					return DeserializeComplex(data);
				}
			}

			/**
			 * @brief Deserializes data from a byte vector.
			 *
			 * @param data The byte vector containing the serialized data.
			 * @return An Expected object containing the deserialized data on success,
			 *         or a DeserializeError on failure.
			 */
			static StormByte::Expected<T, DeserializeError> Deserialize(const std::vector<std::byte>& data) noexcept {
				return Deserialize(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Calculates the serialized size of the data.
			 *
			 * @param data The data to calculate the size for.
			 * @return The size in bytes of the serialized data.
			 */
			static std::size_t Size(const DecayedT& data) noexcept {
				if constexpr (is_optional<T>::value) {
					return SizeOptional(data);
				} else if constexpr (is_pair<T>::value) {
					return SizePair(data);
				} else if constexpr (is_container<T>::value) {
					return SizeContainer(data);
				} else if constexpr (std::is_trivially_copyable_v<T>) {
					return sizeof(data);
				} else {
					return SizeComplex(data);
				}
			}

		private:
			const DecayedT& m_data;	///< The data to serialize.

			/**
			 * @brief Serializes trivially copyable data.
			 *
			 * Direct memory copy of the value. Only used for types that satisfy
			 * std::is_trivially_copyable_v and are not optional/pair/container.
			 *
			 * @return A vector of bytes containing the serialized data.
			 */
			std::vector<std::byte> SerializeTrivial() const noexcept {
				DecayedT value = m_data;

				if constexpr (std::endian::native != std::endian::little) {
					value = swap_endian(value);
				}

				return { reinterpret_cast<const std::byte*>(&value),
					reinterpret_cast<const std::byte*>(&value) + sizeof(value) };
			}

			/**
			 * @brief Serializes complex data types.
			 *
			 * @return A vector of bytes containing the serialized data.
			 */
			std::vector<std::byte> SerializeComplex() const noexcept;

			/**
			 * @brief Serializes container data.
			 *
			 * Writes uint64_t size followed by each element.
			 *
			 * @return A vector of bytes containing the serialized container data.
			 */
			std::vector<std::byte> SerializeContainer() const noexcept {
				std::uint64_t size = static_cast<std::uint64_t>(m_data.size());
				Serializable<std::uint64_t> size_serial(size);
				std::vector<std::byte> buffer = size_serial.Serialize();
				buffer.reserve(buffer.size() + SizeContainer(m_data));
				for (const auto& element : m_data) {
					Serializable<std::decay_t<decltype(element)>> element_serial(element);
					auto element_data = element_serial.Serialize();
					append_vector(buffer, std::move(element_data));
				}
				return buffer;
			}

			/**
			 * @brief Serializes pair data.
			 *
			 * @return A vector of bytes containing the serialized pair data.
			 */
			std::vector<std::byte> SerializePair() const noexcept {
				Serializable<std::decay_t<typename T::first_type>> first_serial(m_data.first);
				Serializable<std::decay_t<typename T::second_type>> second_serial(m_data.second);
				std::vector<std::byte> buffer;
				buffer.reserve(SizePair(m_data));
				auto first_data = first_serial.Serialize();
				auto second_data = second_serial.Serialize();
				append_vector(buffer, std::move(first_data));
				append_vector(buffer, std::move(second_data));
				return buffer;
			}

			/**
			 * @brief Serializes optional data.
			 *
			 * Writes a bool (has_value) then the value if present. Never copies
			 * the optional object representation (avoids padding).
			 *
			 * @return A vector of bytes containing the serialized optional data.
			 */
			std::vector<std::byte> SerializeOptional() const noexcept {
				bool has_value = m_data.has_value();
				std::vector<std::byte> buffer;
				buffer.reserve(SizeOptional(m_data));
				auto has_value_data = Serializable<bool>(has_value).Serialize();
				append_vector(buffer, std::move(has_value_data));
				if (m_data.has_value()) {
					Serializable<std::decay_t<decltype(m_data.value())>> value_serial(m_data.value());
					auto value_data = value_serial.Serialize();
					append_vector(buffer, std::move(value_data));
				}
				return buffer;
			}

			/**
			 * @brief Calculates the serialized size of complex data.
			 *
			 * @param data The data to calculate the size for.
			 * @return The size in bytes of the serialized data.
			 */
			static std::size_t SizeComplex(const DecayedT& data) noexcept;

			/**
			 * @brief Calculates the serialized size of container data.
			 *
			 * @param data The container data to calculate the size for.
			 * @return The size in bytes of the serialized container data.
			 */
			static std::size_t SizeContainer(const DecayedT& data) noexcept {
				std::size_t size = sizeof(std::uint64_t);
				for (const auto& element : data) {
					size += Serializable<std::decay_t<decltype(element)>>::Size(element);
				}
				return size;
			}

			/**
			 * @brief Calculates the serialized size of pair data.
			 *
			 * @param data The pair data to calculate the size for.
			 * @return The size in bytes of the serialized pair data.
			 */
			static std::size_t SizePair(const DecayedT& data) noexcept {
				return
					Serializable<std::decay_t<typename T::first_type>>::Size(data.first) +
					Serializable<std::decay_t<typename T::second_type>>::Size(data.second);
			}

			/**
			 * @brief Calculates the serialized size of optional data.
			 *
			 * @param data The optional data to calculate the size for.
			 * @return The size in bytes of the serialized optional data.
			 */
			static std::size_t SizeOptional(const DecayedT& data) noexcept {
				std::size_t size = sizeof(bool);
				if (data.has_value()) {
					size += Serializable<std::decay_t<decltype(data.value())>>::Size(data.value());
				}
				return size;
			}

			/**
			 * @brief Deserializes trivially copyable data.
			 *
			 * @param data The byte span containing the serialized data.
			 * @return An Expected with the value, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializeTrivial(std::span<const std::byte> data) noexcept {
				if (data.size() < sizeof(T))
					return StormByte::Unexpected<DeserializeError>("Insufficient data for deserialization");
				T result;
				std::memcpy(&result, data.data(), sizeof(T));

				if constexpr (std::endian::native != std::endian::little) {
					result = swap_endian(result);
				}

				return result;
			}

			/**
			 * @brief Deserializes trivially copyable data from a vector.
			 *
			 * @param data The byte vector containing the serialized data.
			 * @return An Expected with the value, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializeTrivial(const std::vector<std::byte>& data) noexcept {
				return DeserializeTrivial(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Deserializes complex data types.
			 *
			 * @param data The byte span containing the serialized data.
			 * @return An Expected with the value, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializeComplex(std::span<const std::byte> data) noexcept;

			/**
			 * @brief Deserializes complex data types from a vector.
			 *
			 * @param data The byte vector containing the serialized data.
			 * @return An Expected with the value, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializeComplex(const std::vector<std::byte>& data) noexcept {
				return DeserializeComplex(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Deserializes container data.
			 *
			 * @param data The byte span containing the serialized container data.
			 * @return An Expected with the container, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializeContainer(std::span<const std::byte> data) noexcept {
				std::size_t offset = 0;

				if (offset + sizeof(std::uint64_t) > data.size())
					return StormByte::Unexpected<DeserializeError>("Insufficient data for container size");

				auto expected_container_size = Serializable<std::uint64_t>::Deserialize(data.subspan(offset, sizeof(std::uint64_t)));
				if (!expected_container_size)
					return StormByte::Unexpected(expected_container_size.error());

				std::uint64_t size = expected_container_size.value();
				offset += sizeof(std::uint64_t);

				if (size > static_cast<std::uint64_t>(data.size() - offset))
					return StormByte::Unexpected<DeserializeError>("Claimed container size exceeds remaining buffer");

				T container;
				for (std::uint64_t i = 0; i < size; ++i) {
					using ElementT = std::decay_t<typename T::value_type>;

					if (offset >= data.size())
						return StormByte::Unexpected<DeserializeError>("Insufficient data for container element");

					auto expected_element = Serializable<ElementT>::Deserialize(data.subspan(offset));
					if (!expected_element)
						return StormByte::Unexpected(expected_element.error());

					std::size_t element_size = Serializable<ElementT>::Size(expected_element.value());
					container.insert(container.end(), std::move(expected_element.value()));
					offset += element_size;
				}
				return container;
			}

			/**
			 * @brief Deserializes container data from a vector.
			 *
			 * @param data The byte vector containing the serialized container data.
			 * @return An Expected with the container, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializeContainer(const std::vector<std::byte>& data) noexcept {
				return DeserializeContainer(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Deserializes pair data.
			 *
			 * @param data The byte span containing the serialized pair data.
			 * @return An Expected with the pair, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializePair(std::span<const std::byte> data) noexcept {
				using FirstT = std::decay_t<typename T::first_type>;
				using SecondT = std::decay_t<typename T::second_type>;
				std::size_t offset = 0;

				auto expected_first = Serializable<FirstT>::Deserialize(data.subspan(offset));
				if (!expected_first)
					return StormByte::Unexpected(expected_first.error());

				offset += Serializable<FirstT>::Size(expected_first.value());

				if (offset >= data.size())
					return StormByte::Unexpected<DeserializeError>("Insufficient data for pair second element");

				auto expected_second = Serializable<SecondT>::Deserialize(data.subspan(offset));
				if (!expected_second)
					return StormByte::Unexpected(expected_second.error());

				return T { std::move(expected_first.value()), std::move(expected_second.value()) };
			}

			/**
			 * @brief Deserializes pair data from a vector.
			 *
			 * @param data The byte vector containing the serialized pair data.
			 * @return An Expected with the pair, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializePair(const std::vector<std::byte>& data) noexcept {
				return DeserializePair(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Deserializes optional data.
			 *
			 * @param data The byte span containing the serialized optional data.
			 * @return An Expected with the optional, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializeOptional(std::span<const std::byte> data) noexcept {
				std::size_t offset = 0;

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
			 * @brief Deserializes optional data from a vector.
			 *
			 * @param data The byte vector containing the serialized optional data.
			 * @return An Expected with the optional, or DeserializeError.
			 */
			static StormByte::Expected<T, DeserializeError> DeserializeOptional(const std::vector<std::byte>& data) noexcept {
				return DeserializeOptional(std::span<const std::byte>(data.data(), data.size()));
			}
	};
}
