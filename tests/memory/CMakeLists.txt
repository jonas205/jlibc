set(TESTS
    default
    test_malloc
    test_free
    test_realloc
    test_canary
)

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.c memory.c ../../jc_memory.h ../../jc_log.h memory.h)
    add_test(${TEST} ${TEST})
endforeach()

foreach(TEST ${TESTS})
    if (${TEST} STREQUAL "test_canary")
        continue()
    endif()
    add_executable(${TEST}_DISABLE_CANARY ${TEST}.c memory.c ../../jc_memory.h ../../jc_log.h memory.h)
    target_compile_definitions(${TEST}_DISABLE_CANARY PUBLIC JC_MEMORY_DISABLE_CANARY)
    add_test(${TEST}_DISABLE_CANARY ${TEST}_DISABLE_CANARY)
endforeach()

foreach(TEST ${TESTS})
    if (${TEST} STREQUAL "test_canary")
        continue()
    endif()
    add_executable(${TEST}_DISABLE ${TEST}.c memory.c ../../jc_memory.h ../../jc_log.h memory.h)
    target_compile_definitions(${TEST}_DISABLE PUBLIC JC_MEMORY_DISABLE)
    add_test(${TEST}_DISABLE ${TEST}_DISABLE)
endforeach()
