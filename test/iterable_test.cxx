#include <StormByte/iterable.hxx>
#include <StormByte/test_handlers.h>

#include <vector>
#include <numeric>
#include <iostream>

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

int main() {
	int result = 0;

	result += test_add_and_index();
	result += test_forward_iteration();
	result += test_reverse_iteration();
	result += test_const_iteration();
	result += test_out_of_bounds();
	
	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}

