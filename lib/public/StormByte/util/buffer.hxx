#pragma once

#include <StormByte/expected.hxx>
#include <StormByte/util/exception.hxx>

#include <cstddef>
#include <string>
#include <span>
#include <vector>

/**
 * @namespace Util
 * @brief Contains utility classes and functions.
 */
namespace StormByte::Util {
	/**
	 * @class Buffer
	 * @brief The class to store and manipulate byte buffers.
	 */
	class STORMBYTE_PUBLIC Buffer {
		public:
			using Byte 		= std::byte;										///< The type of the byte.
			using DataType 	= std::vector<Byte>;								///< The type of the data stored in the buffer.

			/**
			 * @enum ReadPosition
			 * @brief The enumeration to define the read position.
			 */
			enum ReadPosition: unsigned char {
				Begin 		= 0x01,															///< The read position is at the beginning of the buffer.
				End			= 0x02,															///< The read position is at the end of the buffer.
				Relative	= 0x04,															///< The read position is relative to the current position.
				Absolute	= 0x08															///< The read position is absolute.
			};

			/**
			 * Default constructor
			 */
			inline Buffer() noexcept: m_data(), m_position(0) {};

			/**
			 * Constructor reserving initial size
			 * @param size size of the buffer
			 */
			inline Buffer(const std::size_t& size): m_data(size), m_position(0) {};

			/**
			 * Constructor
			 * @param data data to set
			 * @param length length of the data
			 */
			Buffer(const char* data, const std::size_t& length);

			/**
			 * Constructor
			 * @param data data to set
			 */
			Buffer(const std::string& data);

			/**
			 * Constructor
			 * @param data data to set
			 */
			Buffer(const DataType& data);

			/**
			 * Constructor
			 * @param data data to set
			 */
			Buffer(DataType&& data);

			/**
			 * Constructor
			 * @param data data to set
			 */
			Buffer(const std::span<const Byte>& data);

			/**
			 * Copy constructor
			 * @param value value to set
			 */
			Buffer(const Buffer& other)											= default;

			/**
			 * Move constructor
			 * @param value value to set
			 */
			Buffer(Buffer&& other) noexcept										= default;

			/**
			 * Copy assignment operator
			 * @param value value to set
			 */
			Buffer& operator=(const Buffer& other)								= default;

			/**
			 * Move assignment operator
			 * @param value value to set
			 */
			Buffer& operator=(Buffer&& other) noexcept							= default;

			/**
			 * Destructor
			 */
			~Buffer() noexcept													= default;

			/**
			 * Checks if two buffer are equal
			 * @param other buffer to compare
			 * @return true if buffers are equal
			 */
			inline bool 														operator==(const Buffer& other) const {
				return m_data == other.m_data;
			}

			/**
			 * Checks if two buffer are not equal
			 * @param other buffer to compare
			 * @return true if buffers are not equal
			 */
			inline bool operator!=(const Buffer& other) const {
				return !(*this == other);
			}

			/**
			 * Gets a buffer character at a specific index
			 * @param index index of the character
			 * @return buffer character at the index
			 */
			StormByte::Expected<const Byte&, BufferOverflow>					operator[](const std::size_t& index) const;

			/**
			 * Appends a buffer to the current buffer
			 * @param other buffer to append
			 * @return new buffer
			 */
			Buffer&																operator<<(const Buffer& buffer);

			/**
			 * Moves a buffer and appends to the current buffer
			 * @param other buffer to append
			 * @return new buffer
			 */
			Buffer&																operator<<(Buffer&& buffer);

			/**
			 * Appends a string to the current buffer
			 * @param data byte to append
			 * @return new buffer
			 */
			Buffer&																operator<<(const std::string& data);

			/**
			 * Appends a byte vector to the current buffer
			 * @param data byte vector to append
			 * @return new buffer
			 */
			Buffer&																operator<<(const DataType& data);

			/**
			 * Moves a byte vector and appends to the current buffer
			 * @param data byte vector to append
			 * @return new buffer
			 */
			Buffer&																operator<<(DataType&& data);

			/**
			 * Appends current buffer to target buffer
			 * @param other buffer to append
			 * @return new buffer
			 */
			Buffer&																operator>>(Buffer& buffer);

			/**
			 * Clears buffer
			 */
			constexpr void 														Clear() {
				m_data.clear();
				m_position = 0;
			}

			/**
			 * Retrieves the stored value
			 * @return stored value
			 */
			inline const std::span<const Byte>									Data() const {
				return std::span<const Byte>(m_data);
			}

			/**
			 * Retrieves the stored value as a hexadecimal string
			 * @param column_size size of the column
			 * @return stored value as a hexadecimal string
			 */
			std::string 														HexData(const std::size_t& column_size = 16) const;

			/**
			 * Checks if buffer is empty
			 */
			constexpr bool														IsEmpty() const {
				return m_data.empty();
			}

			/**
			 * Retrieves the read position
			 * @return read position
			 */
			constexpr std::size_t 												Position() const {
				return m_position;
			}

			/**
			 * Gets a buffer of a specific length since current read position
			 * @param length length of the buffer
			 * @return buffer of a specific length since current read position
			 */
			StormByte::Expected<std::span<const Byte>, BufferOverflow>			Read(const size_t& length) const;

			/**
			 * Reserves buffer size (if it is not empty, it will reserve extra space)
			 * @return length of the buffer to reserve
			 */
			constexpr void 														Reserve(const std::size_t& size) {
				m_data.reserve(size);
			}

			/**
			 * Resets the read position
			 */
			constexpr void 														ResetPosition() const {
				m_position = 0;
			}

			/**
			 * Moves the read pointer without checking bounds
			 * @param position position to move to
			 */
			void																Seek(const std::ptrdiff_t& position, const ReadPosition& mode) const;

			/**
			 * Retrieves the length of the buffer
			 * @return length of the buffer
			 */
			constexpr std::size_t 												Size() const {
				return m_data.size();
			}

		private:
			std::vector<std::byte> m_data;										///< Stored value
			mutable std::size_t m_position;										///< Read position
	};
}