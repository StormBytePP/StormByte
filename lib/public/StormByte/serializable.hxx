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


#include <StormByte/exception.hxx>
#include <StormByte/expected.hxx>
#include <StormByte/helpers.hxx>
#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <type_traits>
#include <tuple>
#include <utility>
#include <vector>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
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
		 * @brief Identifies `std::array` so fixed-size containers can be decoded by index.
		 * @tparam T Candidate type.
		 */
		template<typename T>
		constexpr bool is_std_array_v = false;

		template<typename U, std::size_t N>
		constexpr bool is_std_array_v<std::array<U, N>> = true;

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
			std::vector<std::byte> Serialize() const noexcept;

			/**
			 * @brief Decodes one @p T from the start of @p data.
			 * @param[in] data Input span; may be longer than one value.
			 * @return Value, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> Deserialize(std::span<const std::byte> data) noexcept;

			/**
			 * @brief Decodes one @p T from a vector.
			 * @param[in] data Input blob.
			 * @return Value, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> Deserialize(const std::vector<std::byte>& data) noexcept;

			/**
			 * @brief Serialized size of @p data.
			 * @param[in] data Value to measure.
			 * @return Size in bytes of @ref Serialize for the same value.
			 */
			static std::size_t Size(const DecayedT& data) noexcept;

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
			std::vector<std::byte> SerializeTrivial() const noexcept
			requires std::is_trivially_copyable_v<T>;

			/**
			 * @brief Encodes a container: `uint64` count (LE) then each element.
			 * @return Blob.
			 */
			std::vector<std::byte> SerializeContainer() const noexcept
			requires Type::Container<T>;

			/**
			 * @brief Encodes a pair: first, then second. No separator.
			 * @return Blob.
			 */
			std::vector<std::byte> SerializePair() const noexcept
			requires Type::Pair<T>;

			/**
			 * @brief Encodes an optional: `bool has_value`, then the value if set.
			 *
			 * Never copies the `optional` object representation (padding).
			 *
			 * @return Blob.
			 */
			std::vector<std::byte> SerializeOptional() const noexcept
			requires Type::Optional<T>;

			/**
			 * @brief Serialized size of a container.
			 * @param[in] data Container to measure.
			 * @return `8` plus the sum of element sizes.
			 */
			static std::size_t SizeContainer(const DecayedT& data) noexcept
			requires Type::Container<T>;

			/**
			 * @brief Serialized size of a pair.
			 * @param[in] data Pair to measure.
			 * @return Sum of member sizes.
			 */
			static std::size_t SizePair(const DecayedT& data) noexcept
			requires Type::Pair<T>;

			/**
			 * @brief Serialized size of an optional.
			 * @param[in] data Optional to measure.
			 * @return `sizeof(bool)` plus the value size when engaged.
			 */
			static std::size_t SizeOptional(const DecayedT& data) noexcept
			requires Type::Optional<T>;

			/**
			 * @brief Decodes a trivially copyable value.
			 *
			 * `bool` only accepts the bytes `0` and `1`. Any other value is
			 * rejected (loading it into a `bool` is undefined).
			 *
			 * @param[in] data Input span.
			 * @return Value, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> DeserializeTrivial(std::span<const std::byte> data) noexcept
			requires std::is_trivially_copyable_v<T>;

			/**
			 * @brief Decodes a container: count, then that many elements.
			 * @param[in] data Input span.
			 * @return Container, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> DeserializeContainer(std::span<const std::byte> data) noexcept
			requires Type::Container<T>;

			/**
			 * @brief Decodes a pair: first, then second.
			 * @param[in] data Input span.
			 * @return Pair, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> DeserializePair(std::span<const std::byte> data) noexcept
			requires Type::Pair<T>;

			/**
			 * @brief Decodes an optional: `bool`, then the value if set.
			 * @param[in] data Input span.
			 * @return Optional, or @ref DeserializeError.
			 */
			static Expected<T, DeserializeError> DeserializeOptional(std::span<const std::byte> data) noexcept
			requires Type::Optional<T>;
	};

	// Explicit-instantiation declarations: suppress implicit instantiation of
	// Serializable<T> for these T in every consumer TU. The matching
	// `template class` definitions live in serializable.cxx, so the generated
	// code ships once inside the shared library instead of being duplicated
	// (and re-emitted as weak symbols) by each consumer.
	//
	// This block MUST come before `#include <StormByte/serializable.txx>`:
	// `SerializeContainer`/`SerializeOptional`/etc. name `Serializable<bool>`
	// and `Serializable<std::uint64_t>` as non-dependent types, so GCC
	// implicitly instantiates those two specializations the moment the
	// member bodies are parsed. If that happens before these `extern
	// template` declarations, the visibility attribute below is silently
	// discarded (see `-Wattributes`) and those two symbols stay hidden.
	extern template class STORMBYTE_PUBLIC Serializable<bool>;
	extern template class STORMBYTE_PUBLIC Serializable<char>;
	extern template class STORMBYTE_PUBLIC Serializable<signed char>;
	extern template class STORMBYTE_PUBLIC Serializable<unsigned char>;
	extern template class STORMBYTE_PUBLIC Serializable<wchar_t>;
	extern template class STORMBYTE_PUBLIC Serializable<char8_t>;
	extern template class STORMBYTE_PUBLIC Serializable<char16_t>;
	extern template class STORMBYTE_PUBLIC Serializable<char32_t>;
	extern template class STORMBYTE_PUBLIC Serializable<short>;
	extern template class STORMBYTE_PUBLIC Serializable<unsigned short>;
	extern template class STORMBYTE_PUBLIC Serializable<int>;
	extern template class STORMBYTE_PUBLIC Serializable<unsigned int>;
	extern template class STORMBYTE_PUBLIC Serializable<long>;
	extern template class STORMBYTE_PUBLIC Serializable<unsigned long>;
	extern template class STORMBYTE_PUBLIC Serializable<long long>;
	extern template class STORMBYTE_PUBLIC Serializable<unsigned long long>;
	extern template class STORMBYTE_PUBLIC Serializable<float>;
	extern template class STORMBYTE_PUBLIC Serializable<double>;
	extern template class STORMBYTE_PUBLIC Serializable<long double>;
	extern template class STORMBYTE_PUBLIC Serializable<std::string>;
	extern template class STORMBYTE_PUBLIC Serializable<std::wstring>;
	extern template class STORMBYTE_PUBLIC Serializable<std::u16string>;
	extern template class STORMBYTE_PUBLIC Serializable<std::u32string>;
}

#include <StormByte/serializable.txx>

