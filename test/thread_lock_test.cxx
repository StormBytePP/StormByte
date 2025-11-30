#include <StormByte/exception.hxx>
#include <StormByte/thread_lock.hxx>
#include <StormByte/test_handlers.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include <vector>
#include <string>

using namespace StormByte;

// Test that the owning thread can call Lock() multiple times (reentrant behavior)
int test_threadlock_reentrant() {
    int result = 0;
    try {
        ThreadLock lock;

        // Owner acquires lock twice
        lock.Lock();
        lock.Lock();

        std::atomic<bool> other_acquired(false);

        std::thread t([&]() {
            // This will block until the owner releases
            lock.Lock();
            other_acquired.store(true);
            lock.Unlock();
        });

        // Give the spawned thread time to attempt acquisition
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Other thread should not have acquired the lock while owner still holds it
        ASSERT_FALSE("test_threadlock_reentrant", other_acquired.load());

        // Owner releases once; per design a single Unlock() fully releases ownership
        // even if Lock() was called multiple times by the same thread.
        lock.Unlock();

        t.join();

        ASSERT_TRUE("test_threadlock_reentrant", other_acquired.load());

    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        result++;
    }
    RETURN_TEST("test_threadlock_reentrant", result);
}

// Test that a thread attempting to Lock() will block until the owning thread Unlocks()
int test_threadlock_blocking() {
    int result = 0;
    try {
        ThreadLock lock;
        lock.Lock();

        std::atomic<bool> acquired(false);

        std::thread t([&]() {
            lock.Lock();
            acquired.store(true);
            lock.Unlock();
        });

        // Ensure the spawned thread has time to attempt acquisition and block
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_FALSE("test_threadlock_blocking", acquired.load());

        // Release lock: the other thread should then acquire it
        lock.Unlock();
        t.join();

        ASSERT_TRUE("test_threadlock_blocking", acquired.load());

    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        result++;
    }
    RETURN_TEST("test_threadlock_blocking", result);
}

// Test multiple threads appending fixed-size tokens while using ThreadLock to ensure
// each append is atomic (no interleaved writes). We verify final string length
// and that each chunk is homogeneous (all characters in the chunk equal), which
// demonstrates the lock provided sequential (non-interleaved) access.
int test_threadlock_many_writers() {
    int result = 0;
    try {
        ThreadLock lock;
        std::string shared;
        const int thread_count = 8;
        const int iterations = 200;
        const int token_size = 8; // fixed-size token per write

        std::vector<std::thread> threads;
        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([t, iterations, token_size, &lock, &shared]() {
                // token is a repetition of a single character, different per thread
                char c = static_cast<char>('A' + (t % 26));
                std::string token(token_size, c);
                for (int i = 0; i < iterations; ++i) {
                    lock.Lock();
                    shared.append(token);
                    lock.Unlock();
                }
            });
        }

        for (auto &th : threads) th.join();

        const std::size_t expected_len = static_cast<std::size_t>(thread_count) * iterations * token_size;
        ASSERT_EQUAL("test_threadlock_many_writers", expected_len, shared.size());

        // Verify each token-sized chunk is homogeneous
        for (std::size_t pos = 0; pos < shared.size(); pos += token_size) {
            char first = shared[pos];
            for (std::size_t k = 1; k < static_cast<std::size_t>(token_size); ++k) {
                if (shared[pos + k] != first) {
                    std::cerr << "Chunk mismatch at pos " << pos << std::endl;
                    return 1;
                }
            }
        }

    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        result++;
    }
    RETURN_TEST("test_threadlock_many_writers", result);
}

int main() {
    int result = 0;

    result += test_threadlock_reentrant();
    result += test_threadlock_blocking();
    result += test_threadlock_many_writers();

    if (result == 0) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << result << " tests failed." << std::endl;
    }
    return result;
}
