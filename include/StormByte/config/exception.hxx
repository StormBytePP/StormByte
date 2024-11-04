#pragma once

#include <StormByte/config/item.hxx>
#include <StormByte/exception.hxx>

namespace StormByte::Config {
	class STORMBYTE_PUBLIC WrongValueTypeConversion final: public Exception {
		public:
			WrongValueTypeConversion(const Item&, const std::string&);
			WrongValueTypeConversion(const WrongValueTypeConversion&)				= default;
			WrongValueTypeConversion& operator=(const WrongValueTypeConversion&)	= default;
			~WrongValueTypeConversion() noexcept override							= default;
	};

	class STORMBYTE_PUBLIC ValueFailure final: public Exception {
		public:
			ValueFailure(const Item&, const Item::Type&);
			ValueFailure(const ValueFailure&)				= default;
			ValueFailure& operator=(const ValueFailure&)	= default;
			~ValueFailure() noexcept override				= default;
	};

	class STORMBYTE_PUBLIC InvalidName final: public Exception {
		public:
			InvalidName(const std::string&);
			InvalidName(const InvalidName&)				= default;
			InvalidName& operator=(const InvalidName&)	= default;
			~InvalidName() noexcept override			= default;
	};

	class STORMBYTE_PUBLIC FileIOError final: public Exception {
		public:
			FileIOError(const std::string&, const std::string&);
			FileIOError(const FileIOError&)				= default;
			FileIOError& operator=(const FileIOError&)	= default;
			~FileIOError() noexcept override			= default;
	};

	class STORMBYTE_PUBLIC ParseError final: public Exception {
		public:
			ParseError(const std::string&);
			ParseError(const std::string&, const std::string&);
			ParseError(const std::string&, const std::string&, const std::string&);
			ParseError(const ParseError&)				= default;
			ParseError& operator=(const ParseError&)	= default;
			~ParseError() noexcept override				= default;
	};

	class STORMBYTE_PUBLIC ItemNotFound final: public Exception {
		public:
			ItemNotFound(const std::string&);
			ItemNotFound(const ItemNotFound&)				= default;
			ItemNotFound& operator=(const ItemNotFound&)	= default;
			~ItemNotFound() noexcept override				= default;
	};
}