#include <StormByte/error.hxx>

using namespace StormByte;

const char* Error::Category::name() const noexcept {
	return "StormByte Error";
}

std::string Error::Category::message(int ev) const {
	switch(static_cast<Error::Code>(ev)) {
		default:
			return "Unknown StormByte error";
	}
}

std::error_condition Error::Category::default_error_condition(int ev) const noexcept {
	switch(static_cast<Error::Code>(ev)) {
		default:
			return std::error_condition(ev, *this);
	}
}

const Error::Category& Error::category() noexcept {
	static Category instance;
	return instance;
}

std::error_code StormByte::make_error_code(Error::Code e) {
	return std::error_code(static_cast<int>(e), Error::category());
}