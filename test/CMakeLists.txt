option(ENABLE_TEST "Enable Unit Tests" OFF)
if(ENABLE_TEST AND NOT STORMBYTE_AS_DEPENDENCY)
	add_executable(ArithmeticMutexedTests arithmetic_mutexed_test.cxx)
	target_link_libraries(ArithmeticMutexedTests StormByte)
	add_test(NAME ArithmeticMutexedTests COMMAND ArithmeticMutexedTests)

	add_executable(MutexedTests mutexed_test.cxx)
	target_link_libraries(MutexedTests StormByte)
	add_test(NAME MutexedTests COMMAND MutexedTests)

	add_executable(SerializationTests serialization_test.cxx)
	target_link_libraries(SerializationTests StormByte)
	add_test(NAME SerializationTests COMMAND SerializationTests)

	add_executable(StringTests string_test.cxx)
	target_link_libraries(StringTests StormByte)
	add_test(NAME StringTests COMMAND StringTests)

	add_executable(SystemTests system_test.cxx)
	target_link_libraries(SystemTests StormByte)
	add_test(NAME SystemTests COMMAND SystemTests)

	add_executable(ThreadLockTests thread_lock_test.cxx)
	target_link_libraries(ThreadLockTests StormByte)
	add_test(NAME ThreadLockTests COMMAND ThreadLockTests)

	# Github special test
	add_executable(GithubTests github_test.cxx)
	target_link_libraries(GithubTests StormByte)
	add_test(NAME GithubTests COMMAND GithubTests)
endif()