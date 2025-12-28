#pragma once

#include <StormByte/exception.hxx>
#include <StormByte/type_traits.hxx>

#include <iterator>
#include <utility>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @class Iterable
	 * @brief A generic iterable container wrapper
	 * @tparam Container The underlying container type
	 *
	 * The `Iterable` class template provides a generic wrapper around standard container types,
	 * enabling iteration capabilities with custom iterator and const iterator classes.
	 */
	template <typename Container>
	class Iterable {
		protected:
			Container m_data;									///< Underlying container
		public:
			using value_type									= typename Container::value_type;
			using size_type										= typename Container::size_type;
			using difference_type								= typename Container::difference_type;
			using reference										= typename Container::reference;
			using const_reference								= typename Container::const_reference;
			using pointer										= typename Container::pointer;
			using const_pointer									= typename Container::const_pointer;

			/**
			 * @class Iterator
			 * @brief Wrapper iterator for Iterable
			 */
			class Iterator {
				friend class Iterable;
				public:
					using iterator_category						= std::random_access_iterator_tag;
					using value_type							= typename Container::value_type;
					using difference_type						= typename Container::difference_type;
					using pointer								= typename Container::pointer;
					using reference								= typename Container::reference;

					/**
					 * @brief Dereference operator
					 * @return Reference to the current element
					 */
					reference 									operator*() { return *m_it; }

					/**
					 * @brief Arrow operator
					 * @return Pointer to the current element
					 */
					pointer 									operator->() { return m_it.operator->(); }

					/**
					 * @brief Pre-increment operator
					 * @return Reference to the incremented iterator
					 */
					Iterator& 									operator++() { ++m_it; return *this; }

					/**
					 * @brief Post-increment operator
					 * @return Iterator before increment
					 */
					Iterator 									operator++(int) { Iterator tmp = *this; ++m_it; return tmp; }

					/**
					 * @brief Pre-decrement operator
					 * @return Reference to the decremented iterator
					 */
					Iterator& 									operator--() { --m_it; return *this; }

					/**
					 * @brief Post-decrement operator
					 * @return Iterator before decrement
					 */
					Iterator 									operator--(int) { Iterator tmp = *this; --m_it; return tmp; }

					/**
					 * @brief Compound addition operator
					 * @param n Number of positions to advance
					 * @return Reference to the advanced iterator
					 */
					Iterator& 									operator+=(difference_type n) { m_it += n; return *this; }

					/**
					 * @brief Compound subtraction operator
					 * @param n Number of positions to retreat
					 * @return Reference to the retreated iterator
					 */
					Iterator& 									operator-=(difference_type n) { m_it -= n; return *this; }

					/**
					 * @brief Addition operator
					 * @param n Number of positions to advance
					 * @return New iterator advanced by n positions
					 */
					Iterator 									operator+(difference_type n) const { return Iterator(m_it + n); }

					/**
					 * @brief Subtraction operator
					 * @param n Number of positions to retreat
					 * @return New iterator retreated by n positions
					 */
					Iterator 									operator-(difference_type n) const { return Iterator(m_it - n); }

					/**
					 * @brief Subtraction operator between two iterators
					 * @param other The other iterator to subtract
					 * @return Difference in positions between the two iterators
					 */
					difference_type 							operator-(const Iterator& other) const { return m_it - other.m_it; }

					/**
					 * @brief Subscript operator
					 * @param n Index to access
					 * @return Reference to the element at index n
					 */
					bool 										operator==(const Iterator& other) const { return m_it == other.m_it; }

					/**
					 * @brief Inequality operator
					 * @param other The other iterator to compare
					 * @return True if the iterators are not equal, false otherwise
					 */
					bool 										operator!=(const Iterator& other) const { return m_it != other.m_it; }

				private:
					typename Container::iterator m_it;			///< Underlying container iterator

					/**
					 * @brief Constructor for Iterator
					 * @param it The underlying container iterator
					 */
					Iterator(typename Container::iterator it): m_it(it) {}
			};

			/**
			 * @class ConstIterator
			 * @brief Wrapper const iterator for Iterable
			 */
			class ConstIterator {
				friend class Iterable;
				public:
					using iterator_category						= std::random_access_iterator_tag;
					using value_type							= typename Container::value_type;
					using difference_type						= typename Container::difference_type;
					using pointer								= typename Container::const_pointer;
					using reference								= typename Container::const_reference;

					/**
					 * @brief Dereference operator
					 * @return Const reference to the current element
					 */
					reference 									operator*() const { return *m_it; }

					/**
					 * @brief Arrow operator
					 * @return Const pointer to the current element
					 */
					pointer 									operator->() const { return m_it.operator->(); }

					/**
					 * @brief Pre-increment operator
					 * @return Reference to the incremented const iterator
					 */
					ConstIterator& 								operator++() { ++m_it; return *this; }

					/**
					 * @brief Post-increment operator
					 * @return Const iterator before increment
					 */
					ConstIterator 								operator++(int) { ConstIterator tmp = *this; ++m_it; return tmp; }

					/**
					 * @brief Pre-decrement operator
					 * @return Reference to the decremented const iterator
					 */
					ConstIterator& 								operator--() { --m_it; return *this; }

					/**
					 * @brief Post-decrement operator
					 * @return Const iterator before decrement
					 */
					ConstIterator 								operator--(int) { ConstIterator tmp = *this; --m_it; return tmp; }

					/**
					 * @brief Compound addition operator
					 * @param n Number of positions to advance
					 * @return Reference to the advanced const iterator
					 */
					ConstIterator& 								operator+=(difference_type n) { m_it += n; return *this; }

					/**
					 * @brief Compound subtraction operator
					 * @param n Number of positions to retreat
					 * @return Reference to the retreated const iterator
					 */
					ConstIterator& 								operator-=(difference_type n) { m_it -= n; return *this; }

					/**
					 * @brief Addition operator
					 * @param n Number of positions to advance
					 * @return New const iterator advanced by n positions
					 */
					ConstIterator								operator+(difference_type n) const { return ConstIterator(m_it + n); }

					/**
					 * @brief Subtraction operator
					 * @param n Number of positions to retreat
					 * @return New const iterator retreated by n positions
					 */
					ConstIterator								operator-(difference_type n) const { return ConstIterator(m_it - n); }

					/**
					 * @brief Subtraction operator between two const iterators
					 * @param other The other const iterator to subtract
					 * @return Difference in positions between the two const iterators
					 */
					difference_type 							operator-(const ConstIterator& other) const { return m_it - other.m_it; }

					/**
					 * @brief Subscript operator
					 * @param n Index to access
					 * @return Const reference to the element at index n
					 */
					bool 										operator==(const ConstIterator& other) const { return m_it == other.m_it; }

					/**
					 * @brief Inequality operator
					 * @param other The other const iterator to compare
					 * @return True if the const iterators are not equal, false otherwise
					 */
					bool 										operator!=(const ConstIterator& other) const { return m_it != other.m_it; }

				private:
					typename Container::const_iterator m_it;

					/**
					 * @brief Constructor for ConstIterator
					 * @param it The underlying container const iterator
					 */
					ConstIterator(typename Container::const_iterator it): m_it(it) {}
			};

			using iterator 										= Iterator;
			using const_iterator 								= ConstIterator;
			using reverse_iterator 								= std::reverse_iterator<iterator>;
			using const_reverse_iterator 						= std::reverse_iterator<const_iterator>;

			/**
			 * @brief Default constructor
			 */
			Iterable() 											= default;

			/**
			 * @brief Constructor from existing container
			 * @param data The container to initialize the Iterable with
			 */
			explicit Iterable(const Container& data): m_data(data) {}

			/**
			 * @brief Move constructor from existing container
			 * @param data The container to initialize the Iterable with
			 */
			explicit Iterable(Container&& data): m_data(std::move(data)) {}

			/**
			 * @brief Copy constructor
			 */
			Iterable(const Iterable&) 							= default;

			/**
			 * @brief Move constructor
			 */
			Iterable(Iterable&&) 								= default;

			/**
			 * @brief Virtual destructor
			 */
			virtual ~Iterable() 								= default;

			/**
			 * @brief Copy assignment operator
			 * @param other The Iterable to copy from
			 * @return Reference to this Iterable
			 */
			Iterable& 											operator=(const Iterable&) = default;

			/**
			 * @brief Move assignment operator
			 * @param other The Iterable to move from
			 * @return Reference to this Iterable
			 */
			Iterable& 											operator=(Iterable&&) = default;

			/**
			 * @brief Gets begin iterator
			 * @return Iterator to the beginning of the container
			 */
			iterator 											begin() noexcept { return iterator(m_data.begin()); }

			/**
			 * @brief Gets const begin iterator
			 * @return Const iterator to the beginning of the container
			 */
			const_iterator 										begin() const noexcept { return const_iterator(m_data.begin()); }

			/**
			 * @brief Gets end iterator
			 * @return Iterator to the end of the container
			 */
			iterator 											end() noexcept { return iterator(m_data.end()); }

			/**
			 * @brief Gets const end iterator
			 * @return Const iterator to the end of the container
			 */
			const_iterator 										end() const noexcept { return const_iterator(m_data.end()); }

			/**
			 * @brief Gets const begin iterator
			 * @return Const iterator to the beginning of the container
			 */
			const_iterator 										cbegin() const noexcept { return const_iterator(m_data.cbegin()); }

			/**
			 * @brief Gets const end iterator
			 * @return Const iterator to the end of the container
			 */
			const_iterator 										cend() const noexcept { return const_iterator(m_data.cend()); }

			/**
			 * @brief Gets reverse begin iterator
			 * @return Reverse iterator to the beginning of the container
			 */
			reverse_iterator 									rbegin() noexcept { return reverse_iterator(end()); }

			/**
			 * @brief Gets reverse end iterator
			 * @return Reverse iterator to the end of the container
			 */
			reverse_iterator 									rend() noexcept { return reverse_iterator(begin()); }

			/**
			 * @brief Gets const reverse begin iterator
			 * @return Const reverse iterator to the beginning of the container
			 */
			const_reverse_iterator 								rbegin() const noexcept { return const_reverse_iterator(end()); }

			/**
			 * @brief Gets const reverse end iterator
			 * @return Const reverse iterator to the end of the container
			 */
			const_reverse_iterator 								rend() const noexcept { return const_reverse_iterator(begin()); }

			/**
			 * @brief Gets const reverse begin iterator
			 * @return Const reverse iterator to the beginning of the container
			 */
			const_reverse_iterator 								crbegin() const noexcept { return const_reverse_iterator(cend()); }

			/**
			 * @brief Gets const reverse end iterator
			 * @return Const reverse iterator to the end of the container
			 */
			const_reverse_iterator 								crend() const noexcept { return const_reverse_iterator(cbegin()); }

			/**
			 * @brief Gets the size of the container
			 * @return Number of elements in the container
			 */
			size_type 											size() const noexcept { return m_data.size(); }

			/**
			 * @brief Checks if the container is empty
			 * @return True if the container is empty, false otherwise
			 */
			bool 												empty() const noexcept { return m_data.empty(); }

			/**
			 * @brief Access element at given index
			 * @param i Index of the element to access
			 * @return Reference to the element at index i
			 */
			reference 											operator[](size_type i)
			requires Type::HasSubscript<Container, size_type> {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				return m_data[i];
			}

			reference 											operator[](size_type i)
			requires (!Type::HasSubscript<Container, size_type>) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				auto it = m_data.begin();
				std::advance(it, i);
				return *it;
			}

			/**
			 * @brief Access element at given index (non-const version)
			 * @param i Index of the element to access
			 * @return Reference to the element at index i
			 * @note This overload is for when container does not support operator[] or mapped_type
			 */
			auto 												operator[](size_type i) -> decltype(auto)
			requires (!Type::HasSubscript<Container, size_type> && !Type::HasMappedType<Container>) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				auto it = m_data.begin();
				std::advance(it, i);
				return *it;
			}


			/*
			* @brief Key-based access for associative containers (non-const)
			* @param key The key to access in the underlying container
			* @return Reference to the mapped value (forward to container's operator[])
			*/
			template<typename K>
			auto 												operator[](K const &key) -> decltype(auto)
			requires (Type::HasMappedType<Container>) {
				return m_data[static_cast<typename Container::key_type>(key)];
			}

			/*
			* @brief Key-based access for associative containers (const)
			* @param key The key to access in the underlying container
			* @return Const reference to the mapped value (searches and throws if not found)
			*/
			template<typename K>
			auto 												operator[](K const &key) const -> decltype(auto)
			requires (Type::HasMappedType<const Container>) {
				auto k = static_cast<typename Container::key_type>(key);
				auto it = m_data.find(k);
				if (it == m_data.cend())
					throw OutOfBoundsError("Key not found in Iterable::operator[]");
				return it->second;
			}

			/**
			 * @brief Access element at given index (const version)
			 * @param i Index of the element to access
			 * @return Const reference to the element at index i
			 */
			const_reference 									operator[](size_type i) const
			requires Type::HasSubscript<const Container, size_type> {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				return m_data[i];
			}

			/**
			 * @brief Access element at given index (const version)
			 * @param i Index of the element to access
			 * @return Const reference to the element at index i
			 * @note This overload is for when container does not support operator[] or mapped_type
			 */
			const_reference 									operator[](size_type i) const
			requires (!Type::HasSubscript<const Container, size_type>) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				auto it = m_data.cbegin();
				std::advance(it, i);
				return *it;
			}

			/**
			 * @brief Access element at given index (const version)
			 * @param i Index of the element to access
			 * @return Const reference to the element at index i
			 * @note This overload is for when container does not support operator[] or mapped_type
			 */
			auto 												operator[](size_type i) const -> decltype(auto)
			requires (!Type::HasSubscript<const Container, size_type> && !Type::HasMappedType<const Container>) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				auto it = m_data.cbegin();
				std::advance(it, i);
				return *it;
			}

			/**
			 * @brief Adds an element to the container
			 * @param value The element to add
			 */
			void 												add(const value_type& value) requires Type::HasPushBack<decltype(m_data)> {
				m_data.push_back(value);
			}

			/**
			 * @brief Adds an element to the container
			 * @param value The element to add
			 */
			void 												add(const value_type& value) requires (!Type::HasPushBack<decltype(m_data)> and Type::HasPushFront<decltype(m_data)>) {
				m_data.push_front(value);
			}

			/**
			 * @brief Adds an element to associative containers via `insert`
			 */
			void 												add(const value_type& value) requires (!Type::HasPushBack<decltype(m_data)> and !Type::HasPushFront<decltype(m_data)> and Type::HasInsert<decltype(m_data)>) {
				m_data.insert(value);
			}

			/**
			 * @brief Adds an element to the container (move version)
			 * @param value The element to add
			 */
			void 												add(value_type&& value) requires Type::HasPushBack<decltype(m_data)> {
				m_data.push_back(std::move(value));
			}

			/**
			 * @brief Adds an element to the container (move version)
			 * @param value The element to add
			 */
			void 												add(value_type&& value) requires (!Type::HasPushBack<decltype(m_data)> and Type::HasPushFront<decltype(m_data)>) {
				m_data.push_front(std::move(value));
			}

			/**
			 * @brief Adds an element to associative containers via `insert` (move version)
			 */
			void 												add(value_type&& value) requires (!Type::HasPushBack<decltype(m_data)> and !Type::HasPushFront<decltype(m_data)> and Type::HasInsert<decltype(m_data)>) {
				m_data.insert(std::move(value));
			}

			/**
			 * @brief Checks if the container has a specific item
			 * @param value The item to check for
			 * @return True if the item exists in the container, false otherwise
			 */
			bool 												has_item(const value_type& value) const {
				for (const auto& item : m_data) {
					if (item == value) {
						return true;
					}
				}
				return false;
			}

			/**
			 * @brief Checks if the container has a specific item (for associative containers)
			 * @param value The item to check for
			 * @return True if the item exists in the container, false otherwise
			 */
			template<typename M>
			bool has_item(M const &value) const
			requires Type::HasMappedType<const Container> && std::convertible_to<M, typename Container::mapped_type> {
				for (const auto &item : m_data) {
					if (item.second == value) return true;
				}
				return false;
			}

			/**
			 * @brief Checks if the container has a specific key (for associative containers)
			 * @param key The key to check for
			 * @return True if the key exists in the container, false otherwise
			 */
			template<typename K>
			bool 												has_key(const K& key) const
			requires Type::HasMappedType<const Container> {
				auto k = static_cast<typename Container::key_type>(key);
				return m_data.find(k) != m_data.cend();

			}
	};
}