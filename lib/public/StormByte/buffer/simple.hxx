#pragma once

#include <StormByte/buffer/exception.hxx>
#include <StormByte/buffer/typedefs.hxx>
#include <StormByte/visibility.h>

/**
 * @namespace Buffer
 * @brief Namespace for buffer-related components in the StormByte library.
 *
 * The Buffer namespace provides classes and utilities for managing simple, shared, and producer/consumer
 * buffers in both single-threaded and multi-threaded environments. It supports a variety of use cases, including:
 * - **Simple Buffer**: Lightweight, non-thread-safe buffers for single-threaded environments.
 * - **Shared Buffer**: Flexible and efficient storage for byte data with concurrent access support.
 * - **External Buffer**: An enhancement of the `Shared` buffer allowing external reader function be executed before every read.
 * - **Producer/Consumer Buffer**: Advanced models for managing data flow between producers and consumers
 *   with status tracking (e.g., `Ready`, `EoF`, `Error`).
 * - **Thread Safety**: Shared and producer/consumer buffers are designed to ensure consistent behavior in multi-threaded environments.
 */
namespace StormByte::Buffer {
	/**
	 * @class Simple
	 * @brief A lightweight class for storing and manipulating simple byte buffers.
	 *
	 * The `Simple` class provides a lightweight and efficient way to store and manipulate byte data.
	 * It supports various operations such as appending, extracting, reading, and seeking within the buffer.
	 *
	 * **Key Features:**
	 * - **Lightweight**: Designed for single-threaded environments with minimal overhead.
	 * - **Flexible Data Management**: Supports appending data from various sources (e.g., strings, vectors, spans),
	 *   extracting data destructively, and reading data non-destructively.
	 * - **Position Management**: Provides methods to manage the read position, including seeking and resetting.
	 *
	 * This class is ideal for scenarios where performance is critical, and thread safety is not required.
	 */
	class STORMBYTE_PUBLIC Simple {
		public:
			/**
			 * @brief Default constructor
			 * Initializes an empty simple buffer.
			 */
			Simple() noexcept;

			/**
			 * @brief Constructor reserving initial size
			 * @param size Size of the buffer to reserve.
			 */
			explicit Simple(const std::size_t& size);

			/**
			 * @brief Constructor
			 * @param data Pointer to the data to set.
			 * @param length Length of the data.
			 * @note The data is copied into the buffer.
			 */
			Simple(const char* data, const std::size_t& length);

			/**
			 * @brief Constructor
			 * @param data String to set as buffer content.
			 */
			Simple(const std::string& data);

			/**
			 * @brief Constructor
			 * @param data Vector of bytes to set as buffer content.
			 */
			Simple(const Data& data);

			/**
			 * @brief Constructor
			 * @param data Vector of bytes to move into the buffer.
			 */
			Simple(Data&& data);

			/**
			 * @brief Constructor
			 * @param data Span of bytes to set as buffer content.
			 */
			Simple(const std::span<const Byte>& data);

			/**
			 * @brief Copy constructor
			 * @param other Simple buffer to copy from.
			 */
			Simple(const Simple& other)												= default;

			/**
			 * @brief Move constructor
			 * @param other Simple buffer to move from.
			 */
			Simple(Simple&& other) noexcept											= default;

			/**
			 * @brief Destructor
			 * Cleans up the simple buffer.
			 */
			virtual ~Simple() noexcept 												= default;

			/**
			 * @brief Copy assignment operator
			 * @param other Simple buffer to copy from.
			 * @return Reference to the updated simple buffer.
			 */
			Simple& operator=(const Simple& other)									= default;

			/**
			 * @brief Move assignment operator
			 * @param other Simple buffer to move from.
			 * @return Reference to the updated simple buffer.
			 */
			Simple& operator=(Simple&& other) noexcept								= default;

			/**
			 * @brief Appends a byte vector to the current simple buffer
			 * @param data Byte vector to append.
			 * @return Reference to the updated simple buffer.
			 * @note This operation does not modify the read position.
			 */
			virtual Simple& 														operator<<(const Data& data);

			/**
			 * @brief Moves a byte vector and appends to the current simple buffer
			 * @param data Byte vector to append.
			 * @return Reference to the updated simple buffer.
			 */
			virtual Simple& 														operator<<(Data&& data);

			/**
			 * @brief Appends a simple buffer to the current simple buffer
			 * @param buffer Simple buffer to append.
			 * @return Reference to the updated simple buffer.
			 */
			virtual Simple& 														operator<<(const Simple& buffer);

			/**
			 * @brief Moves a simple buffer and appends to the current simple buffer
			 * @param buffer Simple buffer to append.
			 * @return Reference to the updated simple buffer.
			 */
			virtual Simple& 														operator<<(Simple&& buffer);

			/**
			 * @brief Appends a string to the current simple buffer
			 * @param data String to append.
			 * @return Reference to the updated simple buffer.
			 */
			virtual Simple& 														operator<<(const std::string& data);

			/**
			 * @brief Appends a numeric value to the current simple buffer.
			 * 
			 * This templated method allows appending numeric values (e.g., integers, floating-point numbers)
			 * to the buffer. The numeric value is serialized into its binary representation and appended
			 * to the buffer.
			 * 
			 * @tparam NumericType The type of the numeric value to append.
			 * @param value The numeric value to append.
			 * @return Reference to the updated simple buffer.
			 */
			template <typename NumericType, typename = std::enable_if_t<std::is_arithmetic_v<std::decay_t<NumericType>>>>
			Simple& operator<<(const NumericType& value) {
				const auto* raw_data = reinterpret_cast<const std::byte*>(&value);
				m_data.insert(m_data.end(), raw_data, raw_data + sizeof(NumericType));
				return *this;
			}

			/**
			 * @brief Appends current simple buffer to target simple buffer
			 * @param buffer Target simple buffer to append to.
			 * @return Reference to the updated simple buffer.
			 */
			virtual Simple& 														operator>>(Simple& buffer);

			/**
			 * @brief Gets the available bytes to read from current position
			 * @return Number of bytes available to read.
			 */
			virtual size_t 															AvailableBytes() const noexcept;

			/**
			 * @brief Retrieves the capacity of the simple buffer
			 * @return The total capacity of the buffer.
			 */
			virtual size_t 															Capacity() const noexcept;

			/**
			 * @brief Clears the simple buffer
			 * Removes all data and resets the read position.
			 */
			virtual void 															Clear();

			/**
			 * @brief Retrieves a copy of the buffer data.
			 * 
			 * This method returns a copy of the data stored in the buffer.
			 * It is suitable for scenarios where the caller needs to work
			 * with an independent copy of the buffer's contents.
			 * 
			 * @return A copy of the buffer data as a vector of bytes.
			 */
			virtual Buffer::Data 													Data() const noexcept;

			/**
			 * @brief Discards data from the buffer.
			 * 
			 * Removes data from the buffer starting from the beginning up to the current read position.
			 * The discard operation can be performed in different modes:
			 * - **Relative**: Discards data relative to the current read position.
			 * - **Absolute**: Discards data up to the specified absolute position.
			 * 
			 * @param length The number of bytes to discard.
			 * @param mode The mode to use for discarding (default is `Read::Position::Relative`).
			 * @note If the specified length exceeds the buffer size, the entire buffer is discarded.
			 */
			virtual void 															Discard(const std::size_t& length, const Read::Position& mode = Read::Position::Relative) noexcept;

			/**
			 * @brief Checks if the simple buffer is empty
			 * @return True if the simple buffer is empty, false otherwise.
			 */
			virtual bool 															Empty() const noexcept;

			/**
			 * @brief Checks if the read position is at the end
			 * @return True if the read position is at the end, false otherwise.
			 */
			virtual bool 															End() const noexcept;

			/**
			 * @brief Extracts a specific size of data, taking ownership of the read data and removing it from the simple buffer.
			 * @param length Length of the data to read and remove from the simple buffer.
			 * @return `ExpectedDataType` containing the requested data, or an `Unexpected` with a `BufferOverflow` error if
			 *         insufficient data exists.
			 */
			virtual ExpectedData<BufferOverflow> 									Extract(const size_t& length);

			/**
			 * @brief Extracts a specific size of data and moves it directly into the provided buffer.
			 * 
			 * Thread-safe version of @see Extract.
			 * This function is a more efficient alternative to `Extract`, as it avoids copying data
			 * by moving it directly into the target buffer. The read position is advanced by the
			 * specified length.
			 * 
			 * @param length Length of the data to extract.
			 * @param output Buffer where the extracted data will be moved.
			 * @return `Read::Status` indicating the success or failure of the operation.
			 */
			virtual Read::Status 													ExtractInto(const size_t& length, Simple& output) noexcept;

			/**
			 * @brief Checks if the simple buffer has enough data starting from the current read position.
			 * @param length Length of the data to check.
			 * @return True if the simple buffer has enough data starting from the current position, false otherwise.
			 */
			virtual bool 															HasEnoughData(const std::size_t& length);

			/**
			 * @brief Retrieves the stored value as a hexadecimal string.
			 *
			 * Converts the buffer's contents into a human-readable hexadecimal string representation.
			 * This is useful for debugging or logging purposes.
			 *
			 * @param column_size Number of bytes per column in the output. Defaults to 16.
			 * @return Hexadecimal string representation of the stored value.
			 */
			virtual std::string 													HexData(const std::size_t& column_size = 16) const;

			/**
			 * @brief Checks if the buffer is at the end of the file/data
			 * @return True if the buffer is at the end, false otherwise.
			 */
			virtual bool 															IsEoF() const noexcept;

			/**
			 * @brief Retrieves the next byte without incrementing the read position.
			 * 
			 * This method allows peeking at the next byte in the buffer without
			 * advancing the read position. It is useful for inspecting the next
			 * value without modifying the state of the buffer.
			 * 
			 * @return `ExpectedByte` containing the next byte, or an `Unexpected`
			 *         with a `BufferOverflow` error if there is no more data.
			 */
			virtual ExpectedByte<BufferOverflow>									Peek() const;

			/**
			 * @brief Retrieves the read position
			 * @return Current read position in the simple buffer.
			 */
			virtual std::size_t 													Position() const noexcept;

			/**
			 * @brief Extracts, processes, and stores the results in the provided buffer.
			 * 
			 * This method performs three operations in a single step:
			 * - Extracts a specific size of data from the buffer.
			 * - Applies a user-provided processing function to the extracted data.
			 * - Stores the processed results in the provided output buffer.
			 * 
			 * This approach is much more efficient than performing the three operations separately,
			 * as it minimizes intermediate copies and reduces overhead.
			 * 
			 * @param length The number of bytes to extract and process.
			 * @param function A user-provided processing function to apply to the extracted data.
			 * @param output The buffer where the processed results will be stored.
			 * @return `Read::Status` indicating the success or failure of the operation.
			 */
			Read::Status 															Process(const std::size_t& length, Processor function, Simple& output) noexcept;

			/**
			 * @brief Reads a specific size of data starting from the current read position.
			 * 
			 * This method retrieves a copy of the requested data from the buffer, starting
			 * at the current read position. The read position is advanced by the specified
			 * length. If the buffer does not contain enough data, a `BufferOverflow` error
			 * is returned.
			 * 
			 * @param length Length of the data to read.
			 * @return `ExpectedDataType` containing a copy of the requested data, or an 
			 *         `Unexpected` with a `BufferOverflow` error if insufficient data exists.
			 */
			virtual ExpectedData<BufferOverflow> 									Read(const size_t& length) const;

			/**
			 * @brief Reserves simple buffer size
			 * Ensures the simple buffer has enough capacity for the specified size.
			 * @param size Size to reserve.
			 */
			virtual void 															Reserve(const std::size_t& size);

			/**
			 * @brief Moves the read pointer within the simple buffer based on the specified position and mode.
			 * @param position The position to move to, interpreted based on the specified mode.
			 * @param mode The mode to use for seeking (e.g., `Begin`, `End`, `Relative`, `Absolute`).
			 */
			virtual void 															Seek(const std::ptrdiff_t& position, const Read::Position& mode) const;

			/**
			 * @brief Retrieves the length of the simple buffer
			 * @return Length of the simple buffer.
			 */
			virtual std::size_t 													Size() const noexcept;

			/**
			 * @brief Retrieves a const view (span) to the stored value.
			 * 
			 * This method provides a lightweight, read-only view of the
			 * buffer's contents without copying the data. The returned
			 * span remains valid as long as the buffer is not modified.
			 * 
			 * @return A read-only span of the stored value.
			 * @note Modifying the buffer invalidates the returned span.
			 */
			const std::span<const Byte> 											Span() const noexcept;

			/**
			 * @brief Retrieves a view (span) to the stored value.
			 * 
			 * This method provides a lightweight, mutable view of the
			 * buffer's contents without copying the data. The returned
			 * span remains valid as long as the buffer is not modified.
			 * 
			 * @return A mutable span of the stored value.
			 * @note Modifying the buffer invalidates the returned span.
			 */
			std::span<Byte> 														Span() noexcept;

			/**
			 * @brief Writes a byte vector to the current simple buffer.
			 *
			 * This function is provided for polymorphic use cases where `operator<<` cannot be used.
			 *
			 * @param data Byte vector to write.
			 * @return Write::Status of the operation.
			 */
			virtual Write::Status 													Write(const Buffer::Data& data);

			/**
			 * @brief Moves a byte vector and writes it to the current simple buffer.
			 *
			 * This function is provided for polymorphic use cases where `operator<<` cannot be used.
			 *
			 * @param data Byte vector to write.
			 * @return Write::Status of the operation.
			 */
			virtual Write::Status 													Write(Buffer::Data&& data);

			/**
			 * @brief Writes a simple buffer to the current simple buffer.
			 *
			 * This function is provided for polymorphic use cases where `operator<<` cannot be used.
			 *
			 * @param buffer Simple buffer to write.
			 * @return Write::Status of the operation.
			 */
			virtual Write::Status 													Write(const Simple& buffer);

			/**
			 * @brief Moves a simple buffer and writes it to the current simple buffer.
			 *
			 * This function is provided for polymorphic use cases where `operator<<` cannot be used.
			 *
			 * @param buffer Simple buffer to write.
			 * @return Write::Status of the operation.
			 */
			virtual Write::Status 													Write(Simple&& buffer);

			/**
			 * @brief Writes a string to the current simple buffer.
			 *
			 * This function is provided for polymorphic use cases where `operator<<` cannot be used.
			 *
			 * @param data String to write.
			 * @return Write::Status of the operation.
			 */
			virtual Write::Status 													Write(const std::string& data);

		protected:
			std::vector<std::byte> m_data; 											///< Stored value.
			mutable std::size_t m_position;											///< Read position.
			mutable std::size_t m_minimum_chunk_size;								///< Minimum chunk size for buffer operations.
	};
}