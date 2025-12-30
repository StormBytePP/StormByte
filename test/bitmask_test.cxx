#include <StormByte/bitmask.hxx>
#include <StormByte/test_handlers.h>

using namespace StormByte;

enum class MyFlags : uint8_t {
	FlagA	= 0x01,
	FlagB	= 0x02,
	FlagC	= 0x04,
	FlagD	= 0x08
};

class MyBitmask: public Bitmask<MyBitmask, MyFlags> {
	public:
	using Bitmask<MyBitmask, MyFlags>::Bitmask;
};

int test_bitmask_operations() {
	int result = 0;

	MyBitmask a(MyFlags::FlagA);
	MyBitmask b(MyFlags::FlagB);
	MyBitmask c = a | b;

	ASSERT_TRUE("test_bitmask_operations", (c.Value() == (MyFlags::FlagA | MyFlags::FlagB)));

	c |= MyBitmask(MyFlags::FlagC);
	ASSERT_TRUE("test_bitmask_operations", (c.Value() == (MyFlags::FlagA | MyFlags::FlagB | MyFlags::FlagC)));

	c &= MyBitmask(MyFlags::FlagA | MyFlags::FlagC);
	ASSERT_TRUE("test_bitmask_operations", (c.Value() == (MyFlags::FlagA | MyFlags::FlagC)));

	c ^= MyBitmask(MyFlags::FlagC);
	ASSERT_TRUE("test_bitmask_operations", (c.Value() == MyFlags::FlagA));

	MyBitmask d = ~a;
	ASSERT_TRUE("test_bitmask_operations", ((d.Value() & MyFlags::FlagA) == static_cast<MyFlags>(0)));

	RETURN_TEST("test_bitmask_operations", result);
}

int main() {
	int result = 0;
	
	result += test_bitmask_operations();

	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}

