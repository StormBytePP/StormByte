#pragma once

#include <StormByte/config/item.hxx>
#include <StormByte/config/exception.hxx>
#include <StormByte/util/iterator.hxx>
#include <StormByte/util/string/string.hxx>

/**
 * @namespace Config
 * @brief All the classes for handling configuration files and items
 */
namespace StormByte::Config {
	/**
	 * @class Container
	 * @brief Base class for a container of configuration items
	 */
	class STORMBYTE_PUBLIC Container {
		public:
			/**
			 * Shortcut alias for internal storage
			 */
			using Storage = std::vector<Item>;

			/**
			 * @class Iterator
			 * @brief Iterator for Container
			 */
			using Iterator = Util::Iterator<Storage>;

			/**
			 * @class ConstIterator
			 * @brief Const iterator for Container
			 */
			using ConstIterator = Util::ConstIterator<Storage>;

			/**
			 * @enum OnExistingAction
			 * @brief Action to take when a name is already in use
			 */
			enum class OnExistingAction: unsigned short {
				Keep,
				Overwrite,
				ThrowException
			};

			/**
			 * @enum Type
			 * @brief Container type
			 */
			enum class Type: unsigned short {
				Group,
				List
			};

			/**
			 * Gets strings from Type
			 * @return string
			 */
			constexpr static const char* 						TypeAsString(const Type& t) noexcept {
				switch(t) {
					case Type::Group:	return "Group";
					case Type::List:	return "List";
					default:			return "Unknown";
				}
			}

			/**
			 * Gets Type from start character
			 * @param start start character
			 * @return Type Container type
			 */
			constexpr static Type 								TypeFromStartCharacter(const char& start) noexcept {
				switch(start) {
					case '{': 			return Type::Group;
					case '[': 			return Type::List;
					default: 			throw Exception("Unknown start character " + std::string(1, start) + " for container");
				}
			}

			/**
			 * Constructor
			 * @param type container type
			 */
			constexpr Container(const Type& type):m_type(type) {}

			/**
			 * Copy constructor
			 */
			constexpr Container(const Container&)				= default;

			/**
			 * Move constructor
			 */
			constexpr Container(Container&&) noexcept			= default;

			/**
			 * Assignment operator
			 */
			constexpr Container& operator=(const Container&)	= default;

			/**
			 * Move assignment operator
			 */
			constexpr Container& operator=(Container&&)			= default;

			/**
			 * Destructor
			 */
			constexpr virtual ~Container() noexcept				= default;

			/**
			 * Gets a reference to Item by index
			 * @throw OutOfBounds if index is out of bounds
			 * @return size_t number of items
			 */
			virtual constexpr Item& 							operator[](const size_t& index) {
				return const_cast<Item&>(static_cast<const Container&>(*this)[index]);
			}

			/**
			 * Gets a const reference to Item by index
			 * @throw OutOfBounds if index is out of bounds
			 * @return size_t number of items
			 */
			virtual constexpr const Item& 						operator[](const size_t& index) const {
				if (index >= m_items.size())
					throw OutOfBounds(index, m_items.size());
				return m_items[index];
			}

			/**
			 * Gets a reference to Item by path
			 * @param path path to item
			 * @throw InvalidPath if path is invalid
			 * @throw ItemNotFound if item is not found
			 * @return Item& item
			 */
			constexpr virtual Item& 							operator[](const std::string& path) {
				return const_cast<Item&>(static_cast<const Container&>(*this)[path]);
			}

			/**
			 * Gets a const reference to Item by path
			 * @param path path to item
			 * @throw InvalidPath if path is invalid
			 * @throw ItemNotFound if item is not found
			 * @return Item& item
			 */
			inline virtual const Item& 							operator[](const std::string& path) const {
				return LookUp(path);
			}

			/**
			 * Equality operator
			 * @param other container to compare
			 * @return bool equal?
			 */
			constexpr bool 										operator==(const Container& other) const noexcept {
				return m_items == other.m_items;
			}

			/**
			 * Inequality operator
			 * @param other container to compare
			 * @return bool not equal?
			 */
			constexpr bool 										operator!=(const Container& other) const noexcept {
				return !(*this == other);
			}

			/**
			 * Adds an item to the container
			 * @param item item to add
			 * @param on_existing action to take if item name already exists
			 * @throw InvalidName if item name is not allowed
			 * @throw ItemNameAlreadyExists if item name already exists
			 * @return reference to added item
			 */
			virtual Item&										Add(const Item& item, const OnExistingAction& on_existing = OnExistingAction::ThrowException) = 0;

			/**
			 * Adds an item to the container
			 * @param item item to add
			 * @param on_existing action to take if item name already exists
			 * @throw ItemNameAlreadyExists if item name already exists
			 * @return reference to added item
			 */
			virtual Item&										Add(Item&& item, const OnExistingAction& on_existing = OnExistingAction::ThrowException) = 0;

			/**
			 * Adds a comment to the container
			 * @param comment comment value
			 */
			inline virtual void 								AddComment(const std::string& comment) {
				Item item(comment);
				item.m_type = Item::Type::Comment;
				Add(std::move(item));
			}

			/**
			 * Gets a direct child by name
			 * @param name comment value
			 * @throw ItemNotFound if item is not found
			 * @return reference to found Item
			 */
			inline Item& 										Child(const std::string& name) {
				return const_cast<Item&>(static_cast<const Container&>(*this).Child(name));
			}

			/**
			 * Gets a direct child by name
			 * @param name comment value
			 * @throw ItemNotFound if item is not found
			 * @return const reference to found Item
			 */
			const Item& 										Child(const std::string& name) const;

			/**
			 * Clears all items
			 */
			constexpr void 										Clear() noexcept {
				m_items.clear();
			}

			/**
			 * Clones the container
			 * @return shared pointer to cloned container
			 */
			virtual std::shared_ptr<Container> 					Clone() const = 0;

			/**
			 * Moves the container
			 * @return shared pointer to moved container
			 */
			virtual std::shared_ptr<Container>					Move() = 0;

			/**
			 * Checks if item exists by path
			 * @param path path to item
			 * @return bool exists?
			 */
			bool 												Exists(const std::string& path) const;

			/**
			 * Removes an item by index
			 * @param index index to item
			 * @throw OutOfBounds if index is out of bounds
			 */
			constexpr void 										Remove(const size_t& index) {
				if (index >= m_items.size())
					throw OutOfBounds(index, m_items.size());
				m_items.erase(m_items.begin() + index);
			}

			/**
			 * Removes an item by path
			 * @param path path to item
			 * @throw InvalidPath if path is invalid
			 * @throw ItemNotFound if item is not found
			 */
			inline void											Remove(const std::string& path) {
				auto path_queue = Util::String::Explode(path, '/');
				Remove(path_queue);
			}

			/**
			 * Returns a string representation of the container
			 * @return string representation
			 */
			virtual std::string 								Serialize(const int& indent_level) const noexcept;

			/**
			 * Gets the number of items in the container
			 * @return size_t number of items
			 */
			constexpr size_t 									Size() const noexcept {
				return m_items.size();
			}

			/**
			 * Gets the start character for the container type
			 * @param type container type
			 * @return start character
			 */
			static constexpr std::pair<const char, const char>	EnclosureCharacters(const Type& type) noexcept {
				switch(type) {
					case Type::Group:	return std::make_pair<const char, const char>('{', '}');
					case Type::List:	return std::make_pair<const char, const char>('[', ']');
				}
			}

			/**
			 * Gets the end character for the container type
			 * @param type container type
			 * @return start character
			 */
			static constexpr const char							EndCharacter(const Type& type) noexcept {
				switch(type) {
					case Type::Group:	return '}';
					case Type::List:	return ']';
				}
			}

			/**
			 * Gets an Iterator pointing to first element
			 * @return Iterator
			 */
			virtual constexpr Iterator 							Begin() noexcept {
				return Util::Iterator<Storage>::Begin(m_items);
			}

			/**
			 * Gets a ConstIterator pointing to first element
			 * @return ConstIterator
			 */
			virtual constexpr ConstIterator						Begin() const noexcept {
				return Util::ConstIterator<Storage>::Begin(m_items);
			}

			/**
			 * Gets an Iterator pointing to past last element
			 * @return Iterator
			 */
			virtual constexpr Iterator 							End() noexcept {
				return Util::Iterator<Storage>::End(m_items);
			}

			/**
			 * Gets a ConstIterator pointing to past last element
			 * @return ConstIterator
			 */
			virtual constexpr ConstIterator						End() const noexcept {
				return Util::ConstIterator<Storage>::End(m_items);
			}

			/**
			 * Gets the container type
			 * @return Container type
			 */
			constexpr const Type& 								GetType() const noexcept {
				return m_type;
			}

			/**
			 * Gets the container type
			 * @return container type
			 */
			constexpr std::string 								TypeAsString() const noexcept {
				return TypeAsString(m_type);
			}

		protected:
			/**
			 * Container type
			 */
			Type 												m_type;

			/**
			 * Internal items storage
			 */
			std::vector<Item> 									m_items;

			/**
			 * Constructor
			 */
			constexpr Container()								= default;

			/**
			 * Internal function to get item contents as string
			 * @return item contents as std::string
			 */
			virtual std::string 								ContentsToString(const int& level) const noexcept;

			/**
			 * Internal function for checking if path name is valid
			 * @param bool is path valid?
			 */
			static bool 										IsPathValid(const std::string& name) noexcept;

			/**
			 * Looks up a child by path
			 * @param path path to child
			 * @throw ItemNotFound if not found
			 * @return const reference to found Item
			 */
			inline const Item& 									LookUp(const std::string& path) const {
				auto path_queue = Util::String::Explode(path, '/');
				return LookUp(path_queue);
			}

			/**
			 * Looks up a child by path
			 * @param path path to child
			 * @throw ItemNotFound if not found
			 * @return const reference to found Item
			 */
			virtual const Item& 								LookUp(std::queue<std::string>& path) const;

			/**
			 * Removes an item by path
			 * @param path path to item
			 * @throw ItemNotFound if item is not found
			 */
			void 												Remove(std::queue<std::string>& path);
	};
}