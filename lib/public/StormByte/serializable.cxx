#include <StormByte/serializable.hxx>
#include <string>

namespace StormByte {
	// Explicit template specialization for std::string serialization
	template<> STORMBYTE_PUBLIC
	std::vector<std::byte> Serializable<std::string>::SerializeComplex() const noexcept {
		std::size_t size = m_data.size();
		Serializable<std::size_t> size_serial(size);
		std::vector<std::byte> buffer = size_serial.Serialize();
		buffer.reserve(buffer.size() + size * sizeof(char));
		const std::byte* data_ptr = reinterpret_cast<const std::byte*>(m_data.data());
		buffer.insert(buffer.end(), data_ptr, data_ptr + size * sizeof(char));
		return buffer;
	}

	// Explicit template specialization for std::string deserialization
	template<> STORMBYTE_PUBLIC
	StormByte::Expected<std::string, DeserializeError> Serializable<std::string>::DeserializeComplex(std::span<const std::byte> data) noexcept {
		std::size_t offset = 0;
		
		// Deserialize the size
		if (offset + sizeof(std::size_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for string size");
		
		auto expected_size = Serializable<std::size_t>::Deserialize(data.subspan(offset, sizeof(std::size_t)));
		if (!expected_size)
			return StormByte::Unexpected(expected_size.error());
		
		std::size_t size = expected_size.value();
		offset += sizeof(std::size_t);
		
		// Check if we have enough data
		if (offset + size > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for string content");
		
		// Construct string directly from the data (contiguous memory)
		std::string result(reinterpret_cast<const char*>(data.data() + offset), size);
		
		return result;
	}

	// Explicit template specialization for std::string size calculation
	template<> STORMBYTE_PUBLIC
	std::size_t Serializable<std::string>::SizeComplex(const std::string& data) noexcept {
		return sizeof(std::size_t) + data.size();
	}

	// Explicit template specialization for std::wstring serialization
	template<> STORMBYTE_PUBLIC
	std::vector<std::byte> Serializable<std::wstring>::SerializeComplex() const noexcept {
		std::size_t size = m_data.size();
		Serializable<std::size_t> size_serial(size);
		std::vector<std::byte> buffer = size_serial.Serialize();
		
		buffer.reserve(buffer.size() + size * sizeof(wchar_t));
		
		const std::byte* data_ptr = reinterpret_cast<const std::byte*>(m_data.data());
		buffer.insert(buffer.end(), data_ptr, data_ptr + size * sizeof(wchar_t));
		
		return buffer;
	}

	template<> STORMBYTE_PUBLIC
	StormByte::Expected<std::wstring, DeserializeError> Serializable<std::wstring>::DeserializeComplex(std::span<const std::byte> data) noexcept {
		std::size_t offset = 0;
		
		if (offset + sizeof(std::size_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for wstring size");
		
		auto expected_size = Serializable<std::size_t>::Deserialize(data.subspan(offset, sizeof(std::size_t)));
		if (!expected_size)
			return StormByte::Unexpected(expected_size.error());
		
		std::size_t size = expected_size.value();
		offset += sizeof(std::size_t);
		
		if (offset + size * sizeof(wchar_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for wstring content");
		
		std::wstring result(reinterpret_cast<const wchar_t*>(data.data() + offset), size);
		
		return result;
	}

	template<>
	std::size_t Serializable<std::wstring>::SizeComplex(const std::wstring& data) noexcept {
		return sizeof(std::size_t) + data.size() * sizeof(wchar_t);
	}

	// Explicit template specialization for std::u16string serialization
	template<>
	std::vector<std::byte> Serializable<std::u16string>::SerializeComplex() const noexcept {
		std::size_t size = m_data.size();
		Serializable<std::size_t> size_serial(size);
		std::vector<std::byte> buffer = size_serial.Serialize();
		
		buffer.reserve(buffer.size() + size * sizeof(char16_t));
		
		const std::byte* data_ptr = reinterpret_cast<const std::byte*>(m_data.data());
		buffer.insert(buffer.end(), data_ptr, data_ptr + size * sizeof(char16_t));
		
		return buffer;
	}

	template<> STORMBYTE_PUBLIC
	StormByte::Expected<std::u16string, DeserializeError> Serializable<std::u16string>::DeserializeComplex(std::span<const std::byte> data) noexcept {
		std::size_t offset = 0;
		
		if (offset + sizeof(std::size_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for u16string size");
		
		auto expected_size = Serializable<std::size_t>::Deserialize(data.subspan(offset, sizeof(std::size_t)));
		if (!expected_size)
			return StormByte::Unexpected(expected_size.error());
		
		std::size_t size = expected_size.value();
		offset += sizeof(std::size_t);
		
		if (offset + size * sizeof(char16_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for u16string content");
		
		std::u16string result(reinterpret_cast<const char16_t*>(data.data() + offset), size);
		
		return result;
	}

	template<> STORMBYTE_PUBLIC
	std::size_t Serializable<std::u16string>::SizeComplex(const std::u16string& data) noexcept {
		return sizeof(std::size_t) + data.size() * sizeof(char16_t);
	}

	// Explicit template specialization for std::u32string serialization
	template<> STORMBYTE_PUBLIC
	std::vector<std::byte> Serializable<std::u32string>::SerializeComplex() const noexcept {
		std::size_t size = m_data.size();
		Serializable<std::size_t> size_serial(size);
		std::vector<std::byte> buffer = size_serial.Serialize();
		
		buffer.reserve(buffer.size() + size * sizeof(char32_t));
		
		const std::byte* data_ptr = reinterpret_cast<const std::byte*>(m_data.data());
		buffer.insert(buffer.end(), data_ptr, data_ptr + size * sizeof(char32_t));
		
		return buffer;
	}

	template<> STORMBYTE_PUBLIC
	StormByte::Expected<std::u32string, DeserializeError> Serializable<std::u32string>::DeserializeComplex(std::span<const std::byte> data) noexcept {
		std::size_t offset = 0;
		
		if (offset + sizeof(std::size_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for u32string size");
		
		auto expected_size = Serializable<std::size_t>::Deserialize(data.subspan(offset, sizeof(std::size_t)));
		if (!expected_size)
			return StormByte::Unexpected(expected_size.error());
		
		std::size_t size = expected_size.value();
		offset += sizeof(std::size_t);
		
		if (offset + size * sizeof(char32_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for u32string content");
		
		std::u32string result(reinterpret_cast<const char32_t*>(data.data() + offset), size);
		
		return result;
	}

	template<> STORMBYTE_PUBLIC
	std::size_t Serializable<std::u32string>::SizeComplex(const std::u32string& data) noexcept {
		return sizeof(std::size_t) + data.size() * sizeof(char32_t);
	}
}
