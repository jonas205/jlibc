set(TESTS
    read_lines
    util
    str2num
)

if( WIN32 )
    set(TESTS ${TESTS} num2str)
endif()

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.c ../../jc_util.h ../../jc_log.h)
    add_test(${TEST} ${TEST})
endforeach()
