#include <StormByte/serializable.hxx>
#include <string>

namespace StormByte {
	// -------------------------------------------------------------------------
	// std::string
	// -------------------------------------------------------------------------
	template<> STORMBYTE_PUBLIC
	std::size_t Serializable<std::string>::SizeComplex(const std::string& data) noexcept {
		return sizeof(std::uint64_t) + data.size();
	}

	template<> STORMBYTE_PUBLIC
	std::vector<std::byte> Serializable<std::string>::SerializeComplex() const noexcept {
		const std::uint64_t size = static_cast<std::uint64_t>(m_data.size());
		std::vector<std::byte> buffer;
		buffer.reserve(SizeComplex(m_data));
		append_vector(buffer, Serializable<std::uint64_t>(size).Serialize());
		const std::byte* data_ptr = reinterpret_cast<const std::byte*>(m_data.data());
		buffer.insert(buffer.end(), data_ptr, data_ptr + static_cast<std::size_t>(size));
		return buffer;
	}

	template<> STORMBYTE_PUBLIC
	StormByte::Expected<std::string, DeserializeError> Serializable<std::string>::DeserializeComplex(std::span<const std::byte> data) noexcept {
		std::size_t offset = 0;

		if (offset + sizeof(std::uint64_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for string size");

		auto expected_size = Serializable<std::uint64_t>::Deserialize(data.subspan(offset, sizeof(std::uint64_t)));
		if (!expected_size)
			return StormByte::Unexpected(expected_size.error());

		std::uint64_t size = expected_size.value();
		offset += sizeof(std::uint64_t);

		if (size > static_cast<std::uint64_t>(data.size() - offset))
			return StormByte::Unexpected<DeserializeError>("Insufficient data for string content");

		return std::string(reinterpret_cast<const char*>(data.data() + offset), static_cast<std::size_t>(size));
	}

	// -------------------------------------------------------------------------
	// std::wstring
	// -------------------------------------------------------------------------
	template<> STORMBYTE_PUBLIC
	std::size_t Serializable<std::wstring>::SizeComplex(const std::wstring& data) noexcept {
		return sizeof(std::uint64_t) + data.size() * sizeof(wchar_t);
	}

	template<> STORMBYTE_PUBLIC
	std::vector<std::byte> Serializable<std::wstring>::SerializeComplex() const noexcept {
		const std::uint64_t size = static_cast<std::uint64_t>(m_data.size());
		std::vector<std::byte> buffer;
		buffer.reserve(SizeComplex(m_data));
		append_vector(buffer, Serializable<std::uint64_t>(size).Serialize());
		const std::byte* data_ptr = reinterpret_cast<const std::byte*>(m_data.data());
		buffer.insert(buffer.end(), data_ptr, data_ptr + static_cast<std::size_t>(size) * sizeof(wchar_t));
		return buffer;
	}

	template<> STORMBYTE_PUBLIC
	StormByte::Expected<std::wstring, DeserializeError> Serializable<std::wstring>::DeserializeComplex(std::span<const std::byte> data) noexcept {
		std::size_t offset = 0;

		if (offset + sizeof(std::uint64_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for wstring size");

		auto expected_size = Serializable<std::uint64_t>::Deserialize(data.subspan(offset, sizeof(std::uint64_t)));
		if (!expected_size)
			return StormByte::Unexpected(expected_size.error());

		std::uint64_t size = expected_size.value();
		offset += sizeof(std::uint64_t);

		if (size > static_cast<std::uint64_t>((data.size() - offset) / sizeof(wchar_t)))
			return StormByte::Unexpected<DeserializeError>("Insufficient data for wstring content");

		return std::wstring(reinterpret_cast<const wchar_t*>(data.data() + offset), static_cast<std::size_t>(size));
	}

	// -------------------------------------------------------------------------
	// std::u16string
	// -------------------------------------------------------------------------
	template<> STORMBYTE_PUBLIC
	std::size_t Serializable<std::u16string>::SizeComplex(const std::u16string& data) noexcept {
		return sizeof(std::uint64_t) + data.size() * sizeof(char16_t);
	}

	template<> STORMBYTE_PUBLIC
	std::vector<std::byte> Serializable<std::u16string>::SerializeComplex() const noexcept {
		const std::uint64_t size = static_cast<std::uint64_t>(m_data.size());
		std::vector<std::byte> buffer;
		buffer.reserve(SizeComplex(m_data));
		append_vector(buffer, Serializable<std::uint64_t>(size).Serialize());
		const std::byte* data_ptr = reinterpret_cast<const std::byte*>(m_data.data());
		buffer.insert(buffer.end(), data_ptr, data_ptr + static_cast<std::size_t>(size) * sizeof(char16_t));
		return buffer;
	}

	template<> STORMBYTE_PUBLIC
	StormByte::Expected<std::u16string, DeserializeError> Serializable<std::u16string>::DeserializeComplex(std::span<const std::byte> data) noexcept {
		std::size_t offset = 0;

		if (offset + sizeof(std::uint64_t) > data.size())
			return StormByte::Unexpected<DeserializeError>("Insufficient data for u16string size");

		auto expected_size = Serializable<std::uint64_t>::Deserialize(data.subspan(offset, sizeof(std::uint64_t)));
		if (!expected_size)
			return StormByte::Unexpected(expected_size.error());

		std::uint64_t size = expected_size.value();
		offset += sizeof(std::uint64_t);

		if (size > static_cast<std::uint64_t>((data.size() - offset) / sizeof(char16_t)))
			return StormByte::Unexpected<DeserializeError>("Insufficient data for u16string content");

		return std::u16string(reinterpret_cast<const char16_t*>(data.data() + offset), static_cast<std::size_t>(size));
	}
}
