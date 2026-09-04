/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * or later, as published by the Free Software Foundation.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 */

#include <StormByte/iterable.hxx>
#include <StormByte/test_handlers.h>
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace StormByte;
// ---------------------------------------------------------------------------
// Test wrappers (expose protected m_data only for construction helpers)
// ---------------------------------------------------------------------------
class MyVector : public Iterable<std::vector<int>> {
public:
	using base = Iterable<std::vector<int>>;
	MyVector() = default;
	MyVector(std::initializer_list<int> init) { m_data = std::vector<int>(init); }
	using base::add;
	using base::operator[];
	using base::size;
	using base::empty;
	using base::begin;
	using base::end;
	using base::rbegin;
	using base::rend;
	using base::cbegin;
	using base::cend;
	using base::has_item;
};
class MyQueue : public Iterable<std::deque<int>> {
public:
	using base = Iterable<std::deque<int>>;
	MyQueue() = default;
	MyQueue(std::initializer_list<int> init) { m_data = std::deque<int>(init); }
	using base::add;
	using base::operator[];
	using base::size;
	using base::empty;
	using base::begin;
	using base::end;
	using base::rbegin;
	using base::rend;
	using base::cbegin;
	using base::cend;
	using base::has_item;
};
class MyMap : public Iterable<std::map<std::string, int>> {
public:
	using base = Iterable<std::map<std::string, int>>;
	MyMap() = default;
	MyMap(std::initializer_list<std::pair<const std::string, int>> init) {
		m_data = std::map<std::string, int>(init);
	}
	using base::add;
	using base::operator[];
	using base::size;
	using base::empty;
	using base::begin;
	using base::end;
	using base::rbegin;
	using base::rend;
	using base::cbegin;
	using base::cend;
	using base::has_item;
	using base::has_key;
};
class MySet : public Iterable<std::set<int>> {
public:
	using base = Iterable<std::set<int>>;
	MySet() = default;
	MySet(std::initializer_list<int> init) { m_data = std::set<int>(init); }
	using base::add;
	using base::size;
	using base::empty;
	using base::begin;
	using base::end;
	using base::has_item;
};
// ---------------------------------------------------------------------------
// Compile-time guards: wrong add() overload must not be viable
// (catches the clang-cl / MSVC STL regression without running code)
// ---------------------------------------------------------------------------
static_assert(Type::HasPushBack<std::vector<int>>);
static_assert(!Type::HasPushFront<std::vector<int>>);
static_assert(!Type::HasInsert<std::vector<int>>);
static_assert(Type::HasPushBack<std::deque<int>>);
static_assert(Type::HasPushFront<std::deque<int>>);
static_assert(!Type::HasInsert<std::deque<int>>);
static_assert(!Type::HasPushBack<std::map<std::string, int>>);
static_assert(!Type::HasPushFront<std::map<std::string, int>>);
static_assert(Type::HasInsert<std::map<std::string, int>>);
static_assert(!Type::HasPushBack<std::set<int>>);
static_assert(!Type::HasPushFront<std::set<int>>);
static_assert(Type::HasInsert<std::set<int>>);
// ---------------------------------------------------------------------------
// Vector: add / index / empty
// ---------------------------------------------------------------------------
int test_vector_add_and_index() {
	int result = 0;
	try {
		MyVector v;
		ASSERT_TRUE("test_vector_add_and_index", v.empty());
		v.add(10);
		v.add(20);
		v.add(30);
		ASSERT_FALSE("test_vector_add_and_index", v.empty());
		ASSERT_EQUAL("test_vector_add_and_index", 3, static_cast<int>(v.size()));
		ASSERT_EQUAL("test_vector_add_and_index", 10, v[0]);
		ASSERT_EQUAL("test_vector_add_and_index", 20, v[1]);
		ASSERT_EQUAL("test_vector_add_and_index", 30, v[2]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_vector_add_and_index", result);
}
int test_vector_add_move() {
	int result = 0;
	try {
		MyVector v;
		int a = 1;
		int b = 2;
		v.add(std::move(a));
		v.add(std::move(b));
		v.add(3);
		ASSERT_EQUAL("test_vector_add_move", 3, static_cast<int>(v.size()));
		ASSERT_EQUAL("test_vector_add_move", 1, v[0]);
		ASSERT_EQUAL("test_vector_add_move", 2, v[1]);
		ASSERT_EQUAL("test_vector_add_move", 3, v[2]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_vector_add_move", result);
}
int test_vector_add_preserves_order() {
	int result = 0;
	try {
		// push_back path: successive adds append, never prepend
		MyVector v;
		for (int i = 0; i < 5; ++i)
			v.add(i);
		ASSERT_EQUAL("test_vector_add_preserves_order", 5, static_cast<int>(v.size()));
		for (int i = 0; i < 5; ++i)
			ASSERT_EQUAL("test_vector_add_preserves_order", i, v[i]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_vector_add_preserves_order", result);
}
// ---------------------------------------------------------------------------
// Iteration
// ---------------------------------------------------------------------------
int test_forward_iteration() {
	int result = 0;
	try {
		MyVector v{1, 2, 3, 4, 5};
		int sum = 0;
		for (auto it = v.begin(); it != v.end(); ++it)
			sum += *it;
		ASSERT_EQUAL("test_forward_iteration", 15, sum);
		sum = 0;
		for (const auto& x : v)
			sum += x;
		ASSERT_EQUAL("test_forward_iteration", 15, sum);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_forward_iteration", result);
}
int test_reverse_iteration() {
	int result = 0;
	try {
		MyVector v{1, 2, 3};
		std::vector<int> rev;
		for (auto it = v.rbegin(); it != v.rend(); ++it)
			rev.push_back(*it);
		ASSERT_EQUAL("test_reverse_iteration", 3, static_cast<int>(rev.size()));
		ASSERT_EQUAL("test_reverse_iteration", 3, rev[0]);
		ASSERT_EQUAL("test_reverse_iteration", 2, rev[1]);
		ASSERT_EQUAL("test_reverse_iteration", 1, rev[2]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_reverse_iteration", result);
}
int test_const_iteration() {
	int result = 0;
	try {
		MyVector v{5, 6, 7};
		const MyVector cv = v;
		int sum = 0;
		for (auto it = cv.begin(); it != cv.end(); ++it)
			sum += *it;
		ASSERT_EQUAL("test_const_iteration", 18, sum);
		sum = 0;
		for (auto it = cv.cbegin(); it != cv.cend(); ++it)
			sum += *it;
		ASSERT_EQUAL("test_const_iteration", 18, sum);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_const_iteration", result);
}
int test_iterator_arithmetic() {
	int result = 0;
	try {
		MyVector v{10, 20, 30, 40};
		auto it = v.begin();
		ASSERT_EQUAL("test_iterator_arithmetic", 10, *it);
		it += 2;
		ASSERT_EQUAL("test_iterator_arithmetic", 30, *it);
		it -= 1;
		ASSERT_EQUAL("test_iterator_arithmetic", 20, *it);
		auto it2 = it + 2;
		ASSERT_EQUAL("test_iterator_arithmetic", 40, *it2);
		ASSERT_EQUAL("test_iterator_arithmetic", 2, static_cast<int>(it2 - it));
		ASSERT_TRUE("test_iterator_arithmetic", it != it2);
		ASSERT_TRUE("test_iterator_arithmetic", v.begin() == v.begin());
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_iterator_arithmetic", result);
}
// ---------------------------------------------------------------------------
// Bounds
// ---------------------------------------------------------------------------
int test_vector_out_of_bounds() {
	int result = 0;
	MyVector v{1, 2, 3};
	try {
		(void)v[3];
		std::cerr << "Expected OutOfBoundsError not thrown (index 3)" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError&) {
		// expected
	}
	try {
		(void)v[100];
		std::cerr << "Expected OutOfBoundsError not thrown (index 100)" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError&) {
		// expected
	}
	try {
		MyVector empty;
		(void)empty[0];
		std::cerr << "Expected OutOfBoundsError not thrown (empty)" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError&) {
		// expected
	}
	RETURN_TEST("test_vector_out_of_bounds", result);
}
int test_const_vector_out_of_bounds() {
	int result = 0;
	const MyVector v{1, 2};
	try {
		(void)v[2];
		std::cerr << "Expected OutOfBoundsError not thrown (const)" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError&) {
		// expected
	}
	RETURN_TEST("test_const_vector_out_of_bounds", result);
}
// ---------------------------------------------------------------------------
// Deque (push_back preferred over push_front)
// ---------------------------------------------------------------------------
int test_queue_add_and_index() {
	int result = 0;
	try {
		MyQueue q;
		q.add(100);
		q.add(200);
		q.add(300);
		ASSERT_EQUAL("test_queue_add_and_index", 3, static_cast<int>(q.size()));
		// Must be append order (push_back), not front-insert order
		ASSERT_EQUAL("test_queue_add_and_index", 100, q[0]);
		ASSERT_EQUAL("test_queue_add_and_index", 200, q[1]);
		ASSERT_EQUAL("test_queue_add_and_index", 300, q[2]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_queue_add_and_index", result);
}
int test_queue_out_of_bounds() {
	int result = 0;
	MyQueue q;
	try {
		(void)q[0];
		std::cerr << "Expected OutOfBoundsError not thrown" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError&) {
		// expected
	}
	RETURN_TEST("test_queue_out_of_bounds", result);
}
// ---------------------------------------------------------------------------
// Map (associative insert)
// ---------------------------------------------------------------------------
int test_map_add_and_key_access() {
	int result = 0;
	try {
		MyMap m;
		m.add({"one", 100});
		m.add({"two", 200});
		m.add({"three", 300});
		ASSERT_EQUAL("test_map_add_and_key_access", 3, static_cast<int>(m.size()));
		ASSERT_EQUAL("test_map_add_and_key_access", 100, m["one"]);
		ASSERT_EQUAL("test_map_add_and_key_access", 200, m["two"]);
		ASSERT_EQUAL("test_map_add_and_key_access", 300, m["three"]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_map_add_and_key_access", result);
}
int test_map_add_move_pair() {
	int result = 0;
	try {
		MyMap m;
		std::pair<const std::string, int> p{"moved", 42};
		m.add(std::move(p));
		ASSERT_EQUAL("test_map_add_move_pair", 1, static_cast<int>(m.size()));
		ASSERT_EQUAL("test_map_add_move_pair", 42, m["moved"]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_map_add_move_pair", result);
}
int test_map_subscript_inserts() {
	int result = 0;
	try {
		MyMap m;
		m["nonexistent"] = 9;
		ASSERT_TRUE("test_map_subscript_inserts", m.has_item(9));
		ASSERT_TRUE("test_map_subscript_inserts", m.has_key("nonexistent"));
		ASSERT_EQUAL("test_map_subscript_inserts", 9, m["nonexistent"]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_map_subscript_inserts", result);
}
int test_map_const_missing_key_throws() {
	int result = 0;
	const MyMap m{{"a", 1}};
	try {
		(void)m["missing"];
		std::cerr << "Expected OutOfBoundsError not thrown for missing key" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError&) {
		// expected
	}
	try {
		ASSERT_EQUAL("test_map_const_missing_key_throws", 1, m["a"]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_map_const_missing_key_throws", result);
}
// ---------------------------------------------------------------------------
// Set (HasInsert via key_type, no mapped_type)
// ---------------------------------------------------------------------------
int test_set_add() {
	int result = 0;
	try {
		MySet s;
		s.add(3);
		s.add(1);
		s.add(2);
		s.add(1); // duplicate ignored by set
		ASSERT_EQUAL("test_set_add", 3, static_cast<int>(s.size()));
		ASSERT_TRUE("test_set_add", s.has_item(1));
		ASSERT_TRUE("test_set_add", s.has_item(2));
		ASSERT_TRUE("test_set_add", s.has_item(3));
		ASSERT_FALSE("test_set_add", s.has_item(99));
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_set_add", result);
}
// ---------------------------------------------------------------------------
// has_item / has_key
// ---------------------------------------------------------------------------
int test_vector_has_item() {
	int result = 0;
	MyVector v{10, 20, 30, 40, 50};
	ASSERT_TRUE("test_vector_has_item", v.has_item(30));
	ASSERT_FALSE("test_vector_has_item", v.has_item(99));
	ASSERT_FALSE("test_vector_has_item", MyVector{}.has_item(0));
	RETURN_TEST("test_vector_has_item", result);
}
int test_queue_has_item() {
	int result = 0;
	MyQueue q{10, 20, 30, 40, 50};
	ASSERT_TRUE("test_queue_has_item", q.has_item(30));
	ASSERT_FALSE("test_queue_has_item", q.has_item(99));
	RETURN_TEST("test_queue_has_item", result);
}
int test_map_has_item_and_key() {
	int result = 0;
	MyMap m{{"a", 1}, {"b", 2}, {"c", 3}};
	ASSERT_TRUE("test_map_has_item_and_key", m.has_item(2));
	ASSERT_FALSE("test_map_has_item_and_key", m.has_item(4));
	ASSERT_TRUE("test_map_has_item_and_key", m.has_key("b"));
	ASSERT_FALSE("test_map_has_item_and_key", m.has_key("z"));
	RETURN_TEST("test_map_has_item_and_key", result);
}
// ---------------------------------------------------------------------------
// Equality
// ---------------------------------------------------------------------------
int test_equality() {
	int result = 0;
	MyVector a{1, 2, 3};
	MyVector b{1, 2, 3};
	MyVector c{1, 2, 4};
	ASSERT_TRUE("test_equality", a == b);
	ASSERT_FALSE("test_equality", a != b);
	ASSERT_TRUE("test_equality", a != c);
	ASSERT_FALSE("test_equality", a == c);
	RETURN_TEST("test_equality", result);
}
// ---------------------------------------------------------------------------
// Copy / move of wrapper
// ---------------------------------------------------------------------------
int test_copy_and_move() {
	int result = 0;
	try {
		MyVector a{1, 2, 3};
		MyVector b = a;
		ASSERT_EQUAL("test_copy_and_move", 3, static_cast<int>(b.size()));
		ASSERT_EQUAL("test_copy_and_move", 2, b[1]);
		MyVector c = std::move(b);
		ASSERT_EQUAL("test_copy_and_move", 3, static_cast<int>(c.size()));
		ASSERT_EQUAL("test_copy_and_move", 1, c[0]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_copy_and_move", result);
}
int main() {
	int result = 0;
	result += test_vector_add_and_index();
	result += test_vector_add_move();
	result += test_vector_add_preserves_order();
	result += test_forward_iteration();
	result += test_reverse_iteration();
	result += test_const_iteration();
	result += test_iterator_arithmetic();
	result += test_vector_out_of_bounds();
	result += test_const_vector_out_of_bounds();
	result += test_queue_add_and_index();
	result += test_queue_out_of_bounds();
	result += test_map_add_and_key_access();
	result += test_map_add_move_pair();
	result += test_map_subscript_inserts();
	result += test_map_const_missing_key_throws();
	result += test_set_add();
	result += test_vector_has_item();
	result += test_queue_has_item();
	result += test_map_has_item_and_key();
	result += test_equality();
	result += test_copy_and_move();
	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}
