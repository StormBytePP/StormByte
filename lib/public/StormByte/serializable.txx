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

// Out-of-line implementation of StormByte::Serializable, included by serializable.hxx.
// See serializable.hxx for documentation of each member.

namespace StormByte {
	template<typename T>
	std::vector<std::byte> Serializable<T>::Serialize() const noexcept {
		if constexpr (Type::Optional<T>) {
			return SerializeOptional();
		} else if constexpr (Type::Pair<T>) {
			return SerializePair();
		} else if constexpr (Type::Container<T>) {
			return SerializeContainer();
		} else if constexpr (Type::TriviallyCopyable<T>) {
			return SerializeTrivial();
		} else {
			return Detail::Codec<DecayedT>::Write(m_data);
		}
	}

	template<typename T>
	Expected<T, DeserializeError> Serializable<T>::Deserialize(std::span<const std::byte> data) noexcept {
		if constexpr (Type::Optional<T>) {
			return DeserializeOptional(data);
		} else if constexpr (Type::Pair<T>) {
			return DeserializePair(data);
		} else if constexpr (Type::Container<T>) {
			return DeserializeContainer(data);
		} else if constexpr (Type::TriviallyCopyable<T>) {
			return DeserializeTrivial(data);
		} else {
			return Detail::Codec<DecayedT>::Read(data);
		}
	}

	template<typename T>
	Expected<T, DeserializeError> Serializable<T>::Deserialize(const std::vector<std::byte>& data) noexcept {
		return Deserialize(std::span<const std::byte>(data.data(), data.size()));
	}

	template<typename T>
	std::size_t Serializable<T>::Size(const DecayedT& data) noexcept {
		if constexpr (Type::Optional<T>) {
			return SizeOptional(data);
		} else if constexpr (Type::Pair<T>) {
			return SizePair(data);
		} else if constexpr (Type::Container<T>) {
			return SizeContainer(data);
		} else if constexpr (Type::TriviallyCopyable<T>) {
			return sizeof(data);
		} else {
			return Detail::Codec<DecayedT>::Size(data);
		}
	}

	template<typename T>
	std::vector<std::byte> Serializable<T>::SerializeTrivial() const noexcept
	requires Type::TriviallyCopyable<T> {
		DecayedT value = m_data;

		if constexpr (!std::is_same_v<DecayedT, bool> &&
				std::endian::native != std::endian::little) {
			value = Type::Detail::swap_endian(value);
		}

		return {
			reinterpret_cast<const std::byte*>(&value),
			reinterpret_cast<const std::byte*>(&value) + sizeof(value)
		};
	}

	template<typename T>
	std::vector<std::byte> Serializable<T>::SerializeContainer() const noexcept
	requires Type::Container<T> {
		const std::uint64_t size = static_cast<std::uint64_t>(m_data.size());
		std::vector<std::byte> buffer = Serializable<std::uint64_t>(size).Serialize();
		buffer.reserve(buffer.size() + SizeContainer(m_data));
		for (const auto& element : m_data) {
			Serializable<std::decay_t<decltype(element)>> element_serial(element);
			append_vector(buffer, element_serial.Serialize());
		}
		return buffer;
	}

	template<typename T>
	std::vector<std::byte> Serializable<T>::SerializePair() const noexcept
	requires Type::Pair<T> {
		Serializable<std::decay_t<typename T::first_type>> first_serial(m_data.first);
		Serializable<std::decay_t<typename T::second_type>> second_serial(m_data.second);
		std::vector<std::byte> buffer;
		buffer.reserve(SizePair(m_data));
		append_vector(buffer, first_serial.Serialize());
		append_vector(buffer, second_serial.Serialize());
		return buffer;
	}

	template<typename T>
	std::vector<std::byte> Serializable<T>::SerializeOptional() const noexcept
	requires Type::Optional<T> {
		const bool has_value = m_data.has_value();
		std::vector<std::byte> buffer;
		buffer.reserve(SizeOptional(m_data));
		append_vector(buffer, Serializable<bool>(has_value).Serialize());
		if (m_data.has_value()) {
			Serializable<std::decay_t<decltype(m_data.value())>> value_serial(m_data.value());
			append_vector(buffer, value_serial.Serialize());
		}
		return buffer;
	}

	template<typename T>
	std::size_t Serializable<T>::SizeContainer(const DecayedT& data) noexcept
	requires Type::Container<T> {
		std::size_t size = sizeof(std::uint64_t);
		for (const auto& element : data) {
			size += Serializable<std::decay_t<decltype(element)>>::Size(element);
		}
		return size;
	}

	template<typename T>
	std::size_t Serializable<T>::SizePair(const DecayedT& data) noexcept
	requires Type::Pair<T> {
		return
			Serializable<std::decay_t<typename T::first_type>>::Size(data.first) +
			Serializable<std::decay_t<typename T::second_type>>::Size(data.second);
	}

	template<typename T>
	std::size_t Serializable<T>::SizeOptional(const DecayedT& data) noexcept
	requires Type::Optional<T> {
		std::size_t size = sizeof(bool);
		if (data.has_value()) {
			size += Serializable<std::decay_t<decltype(data.value())>>::Size(data.value());
		}
		return size;
	}

	template<typename T>
	Expected<T, DeserializeError> Serializable<T>::DeserializeTrivial(std::span<const std::byte> data) noexcept
	requires Type::TriviallyCopyable<T> {
		if constexpr (std::is_same_v<T, bool>) {
			if (data.empty())
				return Unexpected<DeserializeError>("Insufficient data for bool");

			const auto raw = static_cast<unsigned char>(data[0]);
			if (raw != 0 && raw != 1)
				return Unexpected<DeserializeError>("Invalid bool value in stream");

			return raw != 0;
		} else {
			if (data.size() < sizeof(T))
				return Unexpected<DeserializeError>("Insufficient data for deserialization");

			T result;
			std::memcpy(&result, data.data(), sizeof(T));

			if constexpr (std::endian::native != std::endian::little) {
				result = Type::Detail::swap_endian(result);
			}

			return result;
		}
	}

	template<typename T>
	Expected<T, DeserializeError> Serializable<T>::DeserializeContainer(std::span<const std::byte> data) noexcept
	requires Type::Container<T> {
		std::size_t offset = 0;

		if (offset + sizeof(std::uint64_t) > data.size())
			return Unexpected<DeserializeError>("Insufficient data for container size");

		auto expected_container_size = Serializable<std::uint64_t>::Deserialize(
			data.subspan(offset, sizeof(std::uint64_t)));
		if (!expected_container_size)
			return Unexpected(expected_container_size.error());

		const std::uint64_t size = expected_container_size.value();
		offset += sizeof(std::uint64_t);

		if (size > static_cast<std::uint64_t>(data.size() - offset))
			return Unexpected<DeserializeError>("Claimed container size exceeds remaining buffer");
		if constexpr (Detail::is_std_array_v<T>) {
			if (size != static_cast<std::uint64_t>(std::tuple_size_v<T>))
				return Unexpected<DeserializeError>("Array size does not match serialized element count");
		}

		T container;
		for (std::uint64_t i = 0; i < size; ++i) {
			using ElementT = std::decay_t<typename T::value_type>;

			if (offset >= data.size())
				return Unexpected<DeserializeError>("Insufficient data for container element");

			auto expected_element = Serializable<ElementT>::Deserialize(data.subspan(offset));
			if (!expected_element)
				return Unexpected(expected_element.error());

			const std::size_t element_size = Serializable<ElementT>::Size(expected_element.value());
			if constexpr (Detail::is_std_array_v<T>) {
				container[static_cast<std::size_t>(i)] = std::move(expected_element.value());
			} else {
				container.insert(container.end(), std::move(expected_element.value()));
			}
			offset += element_size;
		}
		return container;
	}

	template<typename T>
	Expected<T, DeserializeError> Serializable<T>::DeserializePair(std::span<const std::byte> data) noexcept
	requires Type::Pair<T> {
		using FirstT = std::decay_t<typename T::first_type>;
		using SecondT = std::decay_t<typename T::second_type>;

		auto expected_first = Serializable<FirstT>::Deserialize(data);
		if (!expected_first)
			return Unexpected(expected_first.error());

		const std::size_t first_size = Serializable<FirstT>::Size(expected_first.value());
		if (first_size > data.size())
			return Unexpected<DeserializeError>("Insufficient data for pair second");

		auto expected_second = Serializable<SecondT>::Deserialize(data.subspan(first_size));
		if (!expected_second)
			return Unexpected(expected_second.error());

		return T{ std::move(expected_first.value()), std::move(expected_second.value()) };
	}

	template<typename T>
	Expected<T, DeserializeError> Serializable<T>::DeserializeOptional(std::span<const std::byte> data) noexcept
	requires Type::Optional<T> {
		auto expected_has = Serializable<bool>::Deserialize(data);
		if (!expected_has)
			return Unexpected(expected_has.error());

		if (!expected_has.value())
			return T{};

		const std::size_t flag_size = Serializable<bool>::Size(true);
		if (flag_size > data.size())
			return Unexpected<DeserializeError>("Insufficient data for optional value");

		using ValueT = std::decay_t<typename T::value_type>;
		auto expected_value = Serializable<ValueT>::Deserialize(data.subspan(flag_size));
		if (!expected_value)
			return Unexpected(expected_value.error());

		return T{ std::move(expected_value.value()) };
	}
}
