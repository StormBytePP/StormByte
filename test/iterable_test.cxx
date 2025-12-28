#include <StormByte/iterable.hxx>
#include <StormByte/test_handlers.h>

#include <map>
#include <vector>
#include <numeric>
#include <iostream>
#include <deque>

using namespace StormByte;

class MyVector : public Iterable<std::vector<int>> {
public:
	using base = Iterable<std::vector<int>>;
	MyVector() = default;
	MyVector(std::initializer_list<int> init) { m_data = std::vector<int>(init); }
	using base::add;
	using base::operator[];
	using base::size;
	using base::begin;
	using base::end;
	using base::rbegin;
	using base::rend;
	using base::cbegin;
	using base::cend;
};

class MyQueue : public Iterable<std::deque<int>> {
public:
	using base = Iterable<std::deque<int>>;
	MyQueue() = default;
	MyQueue(std::initializer_list<int> init) { m_data = std::deque<int>(init); }
	using base::add;
	using base::operator[];
	using base::size;
	using base::begin;
	using base::end;
	using base::rbegin;
	using base::rend;
	using base::cbegin;
	using base::cend;
};

class MyMap : public Iterable<std::map<std::string, int>> {
public:
	using base = Iterable<std::map<std::string, int>>;
	MyMap() = default;
	MyMap(std::initializer_list<std::pair<const std::string, int>> init) { m_data = std::map<std::string, int>(init); }
	int& operator[](const std::string &key) {
		return m_data[key];
	}

	const int& operator[](const std::string &key) const {
		auto it = m_data.find(key);
		if (it == m_data.end())
			throw OutOfBoundsError("Key not found in MyMap::operator[]: {}", key);
		return it->second;
	}
	using base::add;
	using base::operator[];
	using base::size;
	using base::begin;
	using base::end;
	using base::rbegin;
	using base::rend;
	using base::cbegin;
	using base::cend;
};

int test_add_and_index() {
	int result = 0;
	try {
		MyVector v;
		v.add(10);
		v.add(20);
		v.add(30);
		ASSERT_EQUAL("test_add_and_index", 3, v.size());
		ASSERT_EQUAL("test_add_and_index", 10, v[0]);
		ASSERT_EQUAL("test_add_and_index", 20, v[1]);
		ASSERT_EQUAL("test_add_and_index", 30, v[2]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_add_and_index", result);
}

int test_forward_iteration() {
	int result = 0;
	try {
		MyVector v{1,2,3,4,5};
		int sum = 0;
		for (auto it = v.begin(); it != v.end(); ++it) {
			sum += *it;
		}
		ASSERT_EQUAL("test_forward_iteration", 15, sum);

		sum = 0;
		for (const auto &x : v) sum += x;
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
		MyVector v{1,2,3};
		std::vector<int> rev;
		for (auto it = v.rbegin(); it != v.rend(); ++it) {
			rev.push_back(*it);
		}
		ASSERT_EQUAL("test_reverse_iteration", 3, (int)rev.size());
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
		MyVector v{5,6,7};
		const MyVector cv = v;
		int sum = 0;
		for (auto it = cv.begin(); it != cv.end(); ++it) sum += *it;
		ASSERT_EQUAL("test_const_iteration", 18, sum);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_const_iteration", result);
}

int test_out_of_bounds() {
	int result = 0;
	MyVector v{1,2,3};
	try {
		int x = v[3];
		(void)x;
		std::cerr << "Expected OutOfBoundsError not thrown" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError& ex) {
		// Expected
	}
	try {
		int x = v[100];
		(void)x;
		std::cerr << "Expected OutOfBoundsError not thrown" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError& ex) {
		// Expected
	}
	RETURN_TEST("test_out_of_bounds", result);
}

int test_queue_add_and_access_by_index() {
	int result = 0;
	try {
		MyQueue m;
		m.add(100);
		m.add(200);
		m.add(300);
		ASSERT_EQUAL("test_map_add", 3, m.size());
		ASSERT_EQUAL("test_map_add", 100, m[0]);
		ASSERT_EQUAL("test_map_add", 200, m[1]);
		ASSERT_EQUAL("test_map_add", 300, m[2]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_map_add", result);
}

int test_map_add_and_access_by_index() {
	int result = 0;
	try {
		MyMap m;
		m.add({"one", 100});
		m.add({"two", 200});
		m.add({"three", 300});
		ASSERT_EQUAL("test_map_add", 3, m.size());
		ASSERT_EQUAL("test_map_add", 100, m["one"]);
		ASSERT_EQUAL("test_map_add", 200, m["two"]);
		ASSERT_EQUAL("test_map_add", 300, m["three"]);
	} catch (const StormByte::OutOfBoundsError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_map_add", result);
}

int test_myvector_has_item() {
	int result = 0;
	MyVector v{10,20,30,40,50};
	ASSERT_TRUE("test_myvector_has_item", v.has_item(30));
	ASSERT_FALSE("test_myvector_has_item", v.has_item(99));
	RETURN_TEST("test_myvector_has_item", result);
}

int test_myqueue_has_item() {
	int result = 0;
	MyQueue q{10,20,30,40,50};
	ASSERT_TRUE("test_myqueue_has_item", q.has_item(30));
	ASSERT_FALSE("test_myqueue_has_item", q.has_item(99));
	RETURN_TEST("test_myqueue_has_item", result);
}

int test_mymap_has_item() {
	int result = 0;
	MyMap m{{"a", 1}, {"b", 2}, {"c", 3}};
	ASSERT_TRUE("test_mymap_has_item", m.has_item(2));
	ASSERT_FALSE("test_mymap_has_item", m.has_item(4));
	RETURN_TEST("test_mymap_has_item", result);
}

int test_mymap_has_key() {
	int result = 0;
	MyMap m{{"a", 1}, {"b", 2}, {"c", 3}};
	ASSERT_TRUE("test_mymap_has_key", m.has_key("b"));
	ASSERT_FALSE("test_mymap_has_key", m.has_key("z"));
	RETURN_TEST("test_mymap_has_key", result);
}

int test_myvector_subscript_operator_out_of_bounds() {
	int result = 0;
	MyVector v;
	try {
		int x = v[0]; // This should throw
		(void)x;
		std::cerr << "Expected OutOfBoundsError not thrown" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError& ex) {
		// Expected
	}
	RETURN_TEST("test_myvector_subscript_operator_out_of_bounds", result);
}

int test_myqueue_subscript_operator_out_of_bounds() {
	int result = 0;
	MyQueue q;
	try {
		int x = q[0]; // This should throw
		(void)x;
		std::cerr << "Expected OutOfBoundsError not thrown" << std::endl;
		result++;
	} catch (const StormByte::OutOfBoundsError& ex) {
		// Expected
	}
	RETURN_TEST("test_myqueue_subscript_operator_out_of_bounds", result);
}

int test_mymap_subscript_operator_adds() {
	int result = 0;
	MyMap m;
	try {
		m["nonexistent"] = 9; // This should not throw but add the element on mapped type
		ASSERT_TRUE("test_mymap_subscript_operator_adds", m.has_item(9));
		ASSERT_TRUE("test_mymap_subscript_operator_adds", m.has_key("nonexistent"));
	} catch (const StormByte::OutOfBoundsError& ex) {
		result++;
	}
	RETURN_TEST("test_mymap_subscript_operator_adds", result);
}

int main() {
	int result = 0;

	result += test_add_and_index();
	result += test_forward_iteration();
	result += test_reverse_iteration();
	result += test_const_iteration();
	result += test_out_of_bounds();
	result += test_queue_add_and_access_by_index();
	result += test_map_add_and_access_by_index();
	result += test_myvector_has_item();
	result += test_myqueue_has_item();
	result += test_mymap_has_item();
	result += test_mymap_has_key();
	result += test_myvector_subscript_operator_out_of_bounds();
	result += test_myqueue_subscript_operator_out_of_bounds();
	result += test_mymap_subscript_operator_adds();
	
	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}

