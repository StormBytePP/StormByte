#pragma once

#include <StormByte/visibility.h>

#include <string>
#include <variant>

/**
 * @namespace StormByte::Config
 * @brief All the classes for handling configuration files and items
 */
namespace StormByte::Config {
	class Group;
	class List;
	/**
	 * @class Item
	 * @brief Class for a configuration item
	 */
	class STORMBYTE_PUBLIC Item {
		friend class Group;
		friend class List;
		friend class Config;
		public:
			/**
			 * @enum Type
			 * @brief Item type
			 */
			enum class Type: unsigned short {
				Group = 0,
				String,
				Integer,
				Double,
				Comment,
				Bool,
				List
			};
			/**
			 * Gets strings from Type
			 * @return string
			 */
			static constexpr const char* GetTypeAsString(const Type& t) noexcept {
				switch(t) {
					case Type::Group:  	return "Group";
					case Type::String: 	return "String";
					case Type::Integer:	return "Integer";
					case Type::Double: 	return "Double";
					case Type::Comment:	return "Comment";
					case Type::Bool:	return "Bool";
					case Type::List:	return "List";
					default:			return "Unknown";
				}
			}

			/**
			 * Creates an item with a group value
			 * @param name item name
			 * @param value item value
			 */
			Item(const Group& value);
			/**
			 * Creates an item moving the group value
			 * @param name item name
			 * @param value item value
			 */
			Item(Group&& value);
			/**
			 * Creates an item with a list value
			 * @param name item name
			 * @param value item value
			 */
			Item(const List& value);
			/**
			 * Creates an item moving the list value
			 * @param name item name
			 * @param value item value
			 */
			Item(List&& value);
			/**
			 * Creates an item with a string value
			 * @param name item name
			 * @param value item value
			 */
			Item(const std::string& value);
			/**
			 * Creates an item with a string value
			 * @param name item name
			 * @param value item value
			 */
			Item(const char* value);
			/**
			 * Creates an item with a int value
			 * @param name item name
			 * @param value item value
			 */
			Item(const int& value);
			/**
			 * Creates an item with a double value
			 * @param name item name
			 * @param value item value
			 */
			explicit Item(const double& value);
			/**
			 * Creates an item with a bool value
			 * @param name item name
			 * @param value item value
			 */
			explicit Item(bool value);
			/**
			 * Copy constructor
			 */
			Item(const Item&);
			/**
			 * Move constructor
			 */
			Item(Item&&) noexcept;
			/**
			 * Assignment operator
			 */
			Item& operator=(const Item&);
			/**
			 * Move operator
			 */
			Item& operator=(Item&&) noexcept;
			/**
			 * Destructor
			 */
			virtual ~Item() noexcept;

			/**
			 * Gets item type
			 * @return item Type
			 */
			constexpr const Type&				GetType() const noexcept {
				return m_type;
			}
			/**
			 * Gets item type as string
			 * @return item type string
			 */
			constexpr const std::string			GetTypeAsString() const noexcept {
				return GetTypeAsString(m_type);
			}
			
			/**
			 * Value getter as reference
			 */
			template<typename T> T&				Value();
			/**
			 * Value getter as const reference
			 */
			template<typename T> const T&		Value() const;

			#ifdef MSVC
			template<> Group&					Value<Group>();
			template<> const Group&				Value<Group>() const;
			template<> std::string&				Value<std::string>();
			template<> const std::string&		Value<std::string>() const;
			template<> int&						Value<int>();
			template<> const int&				Value<int>() const;
			template<> double&					Value<double>();
			template<> const double&			Value<double>() const;
			template<> bool&					Value<bool>();
			template<> const bool&				Value<bool>() const;
			template<> List&					Value<List>();
			template<> const List&				Value<List>() const;
			#endif

			/**
			 * Serializes the boolean item
			 * @param indent_level intentation level
			 * @return serialized string
			 */
			virtual std::string					Serialize(const int& indent_level) const noexcept;

		protected:
			/**
			 * Item type
			 */
			Type m_type;

			/**
			 * Add indentation
			 * @param level
			 */
			constexpr static std::string		Indent(const int& level) noexcept {
				return level == 0 ? std::string() : std::string(level, '\t');
			}

			std::string 						ContentsToString(const int& level) const noexcept;
		
		private:
			/**
			 * Shortcut alias for internal storage
			 */
			using ItemStorage = std::variant<std::string, int, double, bool, Group, List>;
			/**
			 * Internal value (it is a regular pointer to avoid cyclic references Item->Group) so it is enough with a forward declaration of Group
			 */
			ItemStorage* m_value;
	};
}