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

/**
 * @file serializable.hxx
 * @brief Little-endian binary encode/decode for StormByte types.
 *
 * This is a mechanism, not a format detector. There is no BOM, no version
 * tag and no runtime endian sniffing. The consumer owns higher-level
 * framing. The only guarantee is: multi-byte values on the wire are
 * little-endian.
 */

#include <StormByte/exception.hxx>
#include <StormByte/expected.hxx>
#include <StormByte/helpers.hxx>
#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <bit>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte library.
 */
namespace StormByte {
	class DeserializeError;

	/**
	 * @namespace StormByte::Detail
	 * @brief Private specialization point for non-trivial types.
	 *
	 * Other StormByte modules (and only those) specialize @ref Codec
	 * for their own types. Do not specialize @ref Serializable itself.
	 */
	namespace Detail {
		/**
		 * @brief Compile-time false dependent on @p T.
		 * @tparam T Ignored; exists so `static_assert` is delayed to instantiation.
		 */
		template<typename T>
		constexpr bool codec_always_false_v = false;

		/**
		 * @struct Codec
		 * @brief Size / write / read for one type @p T.
		 * @tparam T Type being encoded.
		 *
		 * Primary template is intentionally unusable. Specialize the whole
		 * struct (all three members) for a type that is not optional, pair,
		 * container or trivially copyable.
		 *
		 * @code
		 * template<>
		 * struct StormByte::Detail::Codec<MyType> {
		 *     static std::size_t Size(const MyType& v) noexcept;
		 *     static std::vector<std::byte> Write(const MyType& v) noexcept;
		 *     static Expected<MyType, DeserializeError> Read(std::span<const std::byte>) noexcept;
		 * };
		 * @endcode
		 *
		 * @note The three members must agree with each other. `Size(v)` must
		 *       equal `Write(v).size()`. `Read` consumes a prefix of that
		 *       length; leftover bytes belong to the caller.
		 */
		template<typename T>
		struct Codec {
			/**
			 * @brief Serialized size of @p data.
			 * @param[in] data Value to measure.
			 * @return Size in bytes.
			 */
			static std::size_t Size(const T& data) noexcept {
				static_assert(codec_always_false_v<T>,
					"Specialize StormByte::Detail::Codec<T> instead of Serializable<T>");
				(void)data;
				return 0;
			}

			/**
			 * @brief Encodes @p data to a little-endian blob.
			 * @param[in] data Value to encode.
			 * @return Blob. No framing beyond what @p T itself needs.
			 */
			static std::vector<std::byte> Write(const T& data) noexcept {
				static_assert(codec_always_false_v<T>,
					"Specialize StormByte::Detail::Codec<T> instead of Serializable<T>");
				(void)data;
				return {};
			}

			/**
			 * @brief Decodes @p T from the start of @p data.
			 * @param[in] data Input span; may be longer than one value.
			 * @return Value, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> Read(std::span<const std::byte> data) noexcept {
				static_assert(codec_always_false_v<T>,
					"Specialize StormByte::Detail::Codec<T> instead of Serializable<T>");
				(void)data;
				return Unexpected<DeserializeError>("Codec<T> is not specialized");
			}
		};

		/**
		 * @brief @ref Codec specialization for `std::string`.
		 *
		 * Wire: `uint64` byte count (LE) + raw bytes. No encoding transform.
		 */
		template<>
		struct Codec<std::string> {
			static STORMBYTE_PUBLIC std::size_t Size(const std::string& data) noexcept;
			static STORMBYTE_PUBLIC std::vector<std::byte> Write(const std::string& data) noexcept;
			static STORMBYTE_PUBLIC Expected<std::string, DeserializeError> Read(std::span<const std::byte> data) noexcept;
		};

		/**
		 * @brief @ref Codec specialization for `std::wstring`.
		 *
		 * Wire: `uint64` UTF-8 byte count (LE) + UTF-8 payload. Host `wchar_t`
		 * width (2 on Windows, 4 on POSIX) never appears on the wire.
		 */
		template<>
		struct Codec<std::wstring> {
			static STORMBYTE_PUBLIC std::size_t Size(const std::wstring& data) noexcept;
			static STORMBYTE_PUBLIC std::vector<std::byte> Write(const std::wstring& data) noexcept;
			static STORMBYTE_PUBLIC Expected<std::wstring, DeserializeError> Read(std::span<const std::byte> data) noexcept;
		};

		/**
		 * @brief @ref Codec specialization for `std::u16string`.
		 *
		 * Same UTF-8 wire as @ref Codec<std::wstring>.
		 */
		template<>
		struct Codec<std::u16string> {
			static STORMBYTE_PUBLIC std::size_t Size(const std::u16string& data) noexcept;
			static STORMBYTE_PUBLIC std::vector<std::byte> Write(const std::u16string& data) noexcept;
			static STORMBYTE_PUBLIC Expected<std::u16string, DeserializeError> Read(std::span<const std::byte> data) noexcept;
		};

		/**
		 * @brief @ref Codec specialization for `std::u32string`.
		 *
		 * Same UTF-8 wire as @ref Codec<std::wstring>.
		 */
		template<>
		struct Codec<std::u32string> {
			static STORMBYTE_PUBLIC std::size_t Size(const std::u32string& data) noexcept;
			static STORMBYTE_PUBLIC std::vector<std::byte> Write(const std::u32string& data) noexcept;
			static STORMBYTE_PUBLIC Expected<std::u32string, DeserializeError> Read(std::span<const std::byte> data) noexcept;
		};
	}

	/**
	 * @class Serializable
	 * @brief Encodes and decodes one value of type @p T.
	 * @tparam T Type to serialize. Dispatch uses the undecayed @p T.
	 *
	 * Dispatch order is intentional: optional / pair / container run
	 * *before* trivially-copyable. `std::optional<U>` with trivial `U` may
	 * report `is_trivially_copyable` and copying its object representation
	 * includes padding (observed on MSVC).
	 *
	 * Custom types: specialize @ref Detail::Codec, then this class routes
	 * them through the “complex” path automatically.
	 *
	 * @note `Type::String` (`string` / `wstring` / `u16string` / `u32string`)
	 *       is excluded from @ref Type::Container so those types hit Codec
	 *       instead of being encoded as a sequence of code units.
	 */
	template<typename T>
	class Serializable {
		using DecayedT = std::decay_t<T>;	///< Value type stored by this wrapper.

		public:
			/**
			 * @brief Binds @p data for a later @ref Serialize.
			 * @param[in] data Value to encode. Must outlive this object.
			 */
			Serializable(const DecayedT& data) noexcept : m_data(data) {}

			/**
			 * @brief Copy is deleted: the wrapper only holds a reference.
			 */
			Serializable(const Serializable& other) noexcept = delete;

			/**
			 * @brief Move is deleted: the wrapper only holds a reference.
			 */
			Serializable(Serializable&& other) noexcept = delete;

			/**
			 * @brief Destructor.
			 */
			~Serializable() noexcept = default;

			/**
			 * @brief Copy assignment is deleted.
			 */
			Serializable& operator=(const Serializable& other) noexcept = delete;

			/**
			 * @brief Move assignment is deleted.
			 */
			Serializable& operator=(Serializable&& other) noexcept = delete;

			/**
			 * @brief Encodes @ref m_data to a little-endian blob.
			 * @return Blob for this one value. No outer framing.
			 */
			std::vector<std::byte> Serialize() const noexcept {
				if constexpr (Type::Optional<T>) {
					return SerializeOptional();
				} else if constexpr (Type::Pair<T>) {
					return SerializePair();
				} else if constexpr (Type::Container<T>) {
					return SerializeContainer();
				} else if constexpr (std::is_trivially_copyable_v<T>) {
					return SerializeTrivial();
				} else {
					return Detail::Codec<DecayedT>::Write(m_data);
				}
			}

			/**
			 * @brief Decodes one @p T from the start of @p data.
			 * @param[in] data Input span; may be longer than one value.
			 * @return Value, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> Deserialize(std::span<const std::byte> data) noexcept {
				if constexpr (Type::Optional<T>) {
					return DeserializeOptional(data);
				} else if constexpr (Type::Pair<T>) {
					return DeserializePair(data);
				} else if constexpr (Type::Container<T>) {
					return DeserializeContainer(data);
				} else if constexpr (std::is_trivially_copyable_v<T>) {
					return DeserializeTrivial(data);
				} else {
					return Detail::Codec<DecayedT>::Read(data);
				}
			}

			/**
			 * @brief Decodes one @p T from a vector.
			 * @param[in] data Input blob.
			 * @return Value, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> Deserialize(const std::vector<std::byte>& data) noexcept {
				return Deserialize(std::span<const std::byte>(data.data(), data.size()));
			}

			/**
			 * @brief Serialized size of @p data.
			 * @param[in] data Value to measure.
			 * @return Size in bytes of @ref Serialize for the same value.
			 */
			static std::size_t Size(const DecayedT& data) noexcept {
				if constexpr (Type::Optional<T>) {
					return SizeOptional(data);
				} else if constexpr (Type::Pair<T>) {
					return SizePair(data);
				} else if constexpr (Type::Container<T>) {
					return SizeContainer(data);
				} else if constexpr (std::is_trivially_copyable_v<T>) {
					return sizeof(data);
				} else {
					return Detail::Codec<DecayedT>::Size(data);
				}
			}

		private:
			const DecayedT& m_data;	///< Referenced value. Not owned.

			/**
			 * @brief Encodes a trivially copyable value.
			 *
			 * On a big-endian host the object representation is byte-swapped
			 * before the copy. `bool` is one byte and is not swapped.
			 *
			 * @return Blob of `sizeof(T)` bytes.
			 */
			std::vector<std::byte> SerializeTrivial() const noexcept {
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

			/**
			 * @brief Encodes a container: `uint64` count (LE) then each element.
			 * @return Blob.
			 */
			std::vector<std::byte> SerializeContainer() const noexcept {
				const std::uint64_t size = static_cast<std::uint64_t>(m_data.size());
				std::vector<std::byte> buffer = Serializable<std::uint64_t>(size).Serialize();
				buffer.reserve(buffer.size() + SizeContainer(m_data));
				for (const auto& element : m_data) {
					Serializable<std::decay_t<decltype(element)>> element_serial(element);
					append_vector(buffer, element_serial.Serialize());
				}
				return buffer;
			}

			/**
			 * @brief Encodes a pair: first, then second. No separator.
			 * @return Blob.
			 */
			std::vector<std::byte> SerializePair() const noexcept {
				Serializable<std::decay_t<typename T::first_type>> first_serial(m_data.first);
				Serializable<std::decay_t<typename T::second_type>> second_serial(m_data.second);
				std::vector<std::byte> buffer;
				buffer.reserve(SizePair(m_data));
				append_vector(buffer, first_serial.Serialize());
				append_vector(buffer, second_serial.Serialize());
				return buffer;
			}

			/**
			 * @brief Encodes an optional: `bool has_value`, then the value if set.
			 *
			 * Never copies the `optional` object representation (padding).
			 *
			 * @return Blob.
			 */
			std::vector<std::byte> SerializeOptional() const noexcept {
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

			/**
			 * @brief Serialized size of a container.
			 * @param[in] data Container to measure.
			 * @return `8` plus the sum of element sizes.
			 */
			static std::size_t SizeContainer(const DecayedT& data) noexcept {
				std::size_t size = sizeof(std::uint64_t);
				for (const auto& element : data) {
					size += Serializable<std::decay_t<decltype(element)>>::Size(element);
				}
				return size;
			}

			/**
			 * @brief Serialized size of a pair.
			 * @param[in] data Pair to measure.
			 * @return Sum of member sizes.
			 */
			static std::size_t SizePair(const DecayedT& data) noexcept {
				return
					Serializable<std::decay_t<typename T::first_type>>::Size(data.first) +
					Serializable<std::decay_t<typename T::second_type>>::Size(data.second);
			}

			/**
			 * @brief Serialized size of an optional.
			 * @param[in] data Optional to measure.
			 * @return `sizeof(bool)` plus the value size when engaged.
			 */
			static std::size_t SizeOptional(const DecayedT& data) noexcept {
				std::size_t size = sizeof(bool);
				if (data.has_value()) {
					size += Serializable<std::decay_t<decltype(data.value())>>::Size(data.value());
				}
				return size;
			}

			/**
			 * @brief Decodes a trivially copyable value.
			 *
			 * `bool` only accepts the bytes `0` and `1`. Any other value is
			 * rejected (loading it into a `bool` is undefined).
			 *
			 * @param[in] data Input span.
			 * @return Value, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> DeserializeTrivial(std::span<const std::byte> data) noexcept {
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

			/**
			 * @brief Decodes a container: count, then that many elements.
			 * @param[in] data Input span.
			 * @return Container, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> DeserializeContainer(std::span<const std::byte> data) noexcept {
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

				T container;
				for (std::uint64_t i = 0; i < size; ++i) {
					using ElementT = std::decay_t<typename T::value_type>;

					if (offset >= data.size())
						return Unexpected<DeserializeError>("Insufficient data for container element");

					auto expected_element = Serializable<ElementT>::Deserialize(data.subspan(offset));
					if (!expected_element)
						return Unexpected(expected_element.error());

					const std::size_t element_size = Serializable<ElementT>::Size(expected_element.value());
					container.insert(container.end(), std::move(expected_element.value()));
					offset += element_size;
				}
				return container;
			}

			/**
			 * @brief Decodes a pair: first, then second.
			 * @param[in] data Input span.
			 * @return Pair, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> DeserializePair(std::span<const std::byte> data) noexcept {
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

			/**
			 * @brief Decodes an optional: `bool`, then the value if set.
			 * @param[in] data Input span.
			 * @return Optional, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> DeserializeOptional(std::span<const std::byte> data) noexcept {
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
	};
}
