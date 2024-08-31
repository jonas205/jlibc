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
