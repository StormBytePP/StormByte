#include <StormByte/config/config.hxx>
#include <StormByte/config/list.hxx>

#include <regex>

using namespace StormByte::Config;

Config& Config::operator<<(const Config& source) {
	// We will not use serialize for performance reasons
	for (ConstIterator it = source.Begin(); it != source.End(); it++)
		Add(*it);
	return *this;
}

void Config::operator<<(std::istream& istream) { // 1
	StartParse(istream);
}

void Config::operator<<(const std::string& str) { // 2
	std::istringstream istream(str);
	*this << istream;
}

Config& StormByte::Config::operator>>(std::istream& istream, Config& config) { // 3
	config << istream;
	return config;
}

Config& StormByte::Config::operator>>(const std::string& str, Config& config) { // 4
	config << str;
	return config;
}

Config& Config::operator>>(Config& dest) const {
	dest << *this;
	return dest;
}

std::ostream& Config::operator>>(std::ostream& ostream) const { // 5
	ostream << (std::string)*this;
	return ostream;
}

std::string& Config::operator>>(std::string& str) const { // 6
	str += *this; // Conversion should be done automatically by operator std::string()
	return str;
}

std::ostream& StormByte::Config::operator<<(std::ostream& ostream, const Config& config) { // 7
	ostream << (std::string)config;
	return ostream;
}

std::string& operator<<(std::string& str, const Config& config) { // 8
	str += config;
	return str;
}

Config::operator std::string() const {
	std::string serialized = "";
	for (auto it = Begin(); it != End(); it++) {
		serialized += it->Serialize(0);// + "\n";
	}
	return serialized;
}

void Config::StartParse(std::istream& istream) {
	m_container_level = 0;
	for (auto it = m_before_read_hooks.begin(); it != m_before_read_hooks.end(); it++) {
		(*it)(*this);
	}
	Parse(istream, *this, ParseMode::Named);
	for (auto it = m_after_read_hooks.begin(); it != m_after_read_hooks.end(); it++) {
		(*it)(*this);
	}
}

template<> double Config::ParseValue<double>(std::istream& istream) {
	std::string buffer;
	istream >> std::skipws;
	istream >> buffer;

	// std::stod just ignore extra characters so we better check
	std::regex double_regex(R"(^[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?$)");
	if (!std::regex_match(buffer, double_regex))
		throw ParseError("Failed to parse double value '" + buffer + "'");
	try {
		double result = std::stod(buffer);
		istream >> std::noskipws;
		return result;
	}
	catch (std::invalid_argument&) {
		istream.seekg(-(int)buffer.size(), std::ios_base::cur); // Cast needed to avoid MSVC warning
		std::string line = GetCurrentLine(istream);
		throw Exception("Failed to parse double value near " + line);
	}
	catch (std::out_of_range&) {
		istream.seekg(-(int)buffer.size(), std::ios_base::cur); // Cast needed to avoid MSVC warning
		std::string line = GetCurrentLine(istream);
		throw Exception("Double value out of range near " + line);
	}
}

template<> int Config::ParseValue<int>(std::istream& istream) {
	std::string buffer;
	istream >> std::skipws;
	istream >> buffer;

	// stoi will ignore extra characters so we force check
	std::regex integer_regex(R"(^[+-]?\d+$)");
	if (!std::regex_match(buffer, integer_regex))
		throw ParseError("Failed to parse integer value '" + buffer + '"');
	try {
		int result = std::stoi(buffer);
		istream >> std::noskipws;
		return result;
	}
	catch (std::invalid_argument&) {
		istream.seekg(-(int)buffer.size(), std::ios_base::cur); // Cast needed to avoid MSVC warning
		std::string line = GetCurrentLine(istream);
		throw Exception("Failed to parse integer value near " + line);
	}
	catch (std::out_of_range&) {
		istream.seekg(-(int)buffer.size(), std::ios_base::cur); // Cast needed to avoid MSVC warning
		std::string line = GetCurrentLine(istream);
		throw Exception("Integer value out of range near " + line);
	}
}

template<> std::string Config::ParseValue<std::string>(std::istream& istream) {
	istream >> std::ws;
	std::string accumulator;
	// Guesser already detected the opened " so we skip it
	istream.seekg(1, std::ios::cur);
	bool string_closed = false;

	if (istream.eof())
		throw ParseError("String content was expected but found EOF");

	// Do not skip space characters
	istream >> std::noskipws;
	bool escape_next = false;
	char c;
	while (istream.get(c)) {
		if (escape_next) {
			// Handle escaped characters
			switch (c) {
				case '"':
				case '\\':
					accumulator += c;
					break;
				case 'n':
					accumulator += '\n';
					break;
				case 'r':
					accumulator += '\r';
					break;
				case 't':
					accumulator += '\t';
					break;
				default:
					throw ParseError(std::string("Invalid escape sequence: \\") + std::string(1, c) + " near: " + GetCurrentLine(istream, -20));
					break;
			}
			escape_next = false;
		} else {
			if (c == '\\') {
				// Indicate that the next character should be escaped
				escape_next = true;
			} else if (c == '"') {
				string_closed = true;
				break;
			} else {
				accumulator += c;
			}
		}
	}

	if (!string_closed)
		throw ParseError("Expected string closure but got EOF");

	return accumulator;
}

template<> bool Config::ParseValue<bool>(std::istream& istream) {
	bool result;
	istream >> std::skipws >> std::boolalpha;
	istream >> result;
	if (istream.fail()) {
		throw Exception("Failed to parse boolean value near " + GetCurrentLine(istream, -20));
	}
	istream >> std::noskipws >> std::noboolalpha;
	return result;
}

template<class C> bool Config::FindAndParseComment(std::istream& istream, C& container) {
	istream >> std::ws;
	if (istream.eof() || istream.fail())
		return false;
	std::string line;
	bool result = false;
	auto start_pos = istream.tellg();
	if (std::getline(istream, line)) {
		if (line[0] == '#') {
			line.erase(line.begin());
			container.AddComment(line);
			result = true;
		}
		else {
			istream.seekg(start_pos);
			result = false;
		}
	}
	else {
		istream.clear();
		istream.seekg(start_pos);
	}
	return result;
}

template<class C> void Config::FindAndParseComments(std::istream& istream, C& container) {
	while (FindAndParseComment(istream, container));
}

std::unique_ptr<Item> Config::ParseItem(std::istream& istream, const Item::Type& type) {
	switch(type) {
		case Item::Type::Container:
			m_container_level++;
			switch (ParseContainerType(istream)) {
				case Container::Type::Group: {
					Group group;
					Parse(istream, group, ParseMode::Named);
					return std::make_unique<Item>(std::move(group));
				}
				case Container::Type::List: {
					List list;
					Parse(istream, list, ParseMode::Unnamed);
					return std::make_unique<Item>(std::move(list));
				}
			}
			break;
		case Item::Type::Comment: // Not handled here but make compiler not emit any warning
		case Item::Type::String:
			return std::make_unique<Item>(ParseValue<std::string>(istream));
		case Item::Type::Integer:
			return std::make_unique<Item>(ParseValue<int>(istream));
		case Item::Type::Double:
			return std::make_unique<Item>(ParseValue<double>(istream));
		case Item::Type::Bool:
			return std::make_unique<Item>(ParseValue<bool>(istream));
	}
	return nullptr;
}

std::string Config::GetCurrentLine(std::istream& istream) {
	std::string line = "";
	auto start_pos = istream.tellg();
	if (std::getline(istream, line)) {
		istream.seekg(start_pos);
	}
	else {
		istream.clear();
		istream.seekg(start_pos);
	}
	return line;
}

std::string Config::GetCurrentLine(std::istream& istream, const int& offset) {
	istream.clear();
	istream.seekg(offset, std::ios_base::cur);
	return GetCurrentLine(istream);
}

void Config::Parse(std::istream& istream, Container& container, const ParseMode& mode) {
	bool halt = false;
	FindAndParseComments(istream, container);
	while (!halt && !istream.eof()) {
		char c;
		std::string item_name;

		if (mode == ParseMode::Named) {
			// Item Name
			item_name = ParseItemName(istream);
			if (!Item::IsNameValid(item_name)) {
				throw ParseError("Invalid item name: " + item_name);
			}
			// Equal expected
			istream >> std::ws;
			if (!istream.get(c) || c != '=') {
				throw ParseError("Expected '=' after item name: " + item_name);
			}
		}

		// Guessing type
		Item::Type type = ParseType(istream);
		std::unique_ptr<Item> item = ParseItem(istream, type);

		if (!item)
			throw ParseError("Failed to parse item");
		else {
			if (mode == ParseMode::Named)
				item->Name() = std::move(item_name);
			container.Add(std::move(*item), m_on_existing_action);
		}

		FindAndParseComments(istream, container);
		if (FindContainerEnd(istream, container.GetType())) {
			// If it is encountered on level 0 it is a syntax error
			if (m_container_level == 0)
				throw ParseError("Unexpected container end symbol near " + GetCurrentLine(istream, -20));
			else
				m_container_level--;
			
			halt = true;
		}
		if (istream.fail()) {
			if (m_container_level > 0)
				throw ParseError("Unexpected EOF near " + GetCurrentLine(istream, -20));
			else
				halt = true;
		}
	}
}

std::string Config::ParseItemName(std::istream& istream) {
	std::string name;
	istream >> std::skipws;
	istream >> name;
	istream >> std::noskipws;
	return name;
}

Item::Type Config::ParseType(std::istream& istream) {
	/**** THIS FUNCTION ONLY DETECTS TYPE NOT CHECKS FOR VALIDITY *****/
	std::unique_ptr<Item::Type> type;
	istream >> std::ws;
	auto start_position = istream.tellg();
	std::string line;
	std::getline(istream, line);
	/** COMMENTS ARE NOT HANDLED HERE **/
	switch (line[0]) {
		case '"':
			type = std::make_unique<Item::Type>(Item::Type::String);
			break;
		case '[':
		case '{':
			type = std::make_unique<Item::Type>(Item::Type::Container);
			break;
		case '-':
		case '+':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': {
			// Possible int or double, we assume int until we find a "." (not exp notation as without a . it is allowed in integers too)
			// NOTE: We don't do sanity check here!
			type = std::make_unique<Item::Type>(Item::Type::Integer);
			bool halt = false;
			size_t pos = 1;
			while (pos < line.size() && !halt) {
				if (line[pos++] == '.') {
					type = std::make_unique<Item::Type>(Item::Type::Double);
					halt = true;
				}
			}
			break;
		}
		case 't':
		case 'f':
			type = std::make_unique<Item::Type>(Item::Type::Bool);
			break;
		default: {
			throw ParseError("Unexpected character when parsing type near " + line);
		}
	}
	istream.seekg(start_position);
	return *type;
}

Container::Type Config::ParseContainerType(std::istream& istream) {
	/**** THIS FUNCTION ONLY DETECTS TYPE NOT CHECKS FOR VALIDITY *****/
	char c = '\0';
	istream >> std::ws;
	istream.get(c);
	try {
		return Container::TypeFromStartCharacter(c);
	}
	catch (const StormByte::Exception&) {
		throw ParseError("Unknown start character " + std::string(1, c) + " for container");
	}
}

bool Config::FindContainerEnd(std::istream& istream, const Container::Type& container_type) const {
	/**** THIS FUNCTION ONLY DETECTS TYPE NOT CHECKS FOR VALIDITY *****/
	istream >> std::ws;
	char c;
	bool result = false;
	if (istream.get(c)) {
		if (Container::EnclosureCharacters(container_type).second == c)
			result = true;
		else
			istream.unget();
	}
	return result;
}