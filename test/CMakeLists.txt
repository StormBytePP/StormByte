option(ENABLE_TEST "Enable Unit Tests" OFF)
if(ENABLE_TEST AND NOT STORMBYTE_AS_DEPENDENCY)
	enable_testing()

	add_executable(IterableTests iterable_test.cxx)
	target_link_libraries(IterableTests StormByte)
	add_test(NAME IterableTests COMMAND IterableTests)
	
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

	add_executable(UUIDTest uuid_test.cxx)
	target_link_libraries(UUIDTest StormByte)
	add_test(NAME UUIDTest COMMAND UUIDTest)
endif()