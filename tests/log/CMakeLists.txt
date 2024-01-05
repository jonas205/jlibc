set(TESTS
    dummy
    die_sigill
)

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.c ../../jc_log.h)
    add_test(${TEST} ${TEST})
endforeach()
