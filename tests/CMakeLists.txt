add_test(
    NAME hampr_cli_no_args
	COMMAND $<TARGET_FILE:hampr>
)

set_tests_properties(
    hampr_cli_no_args
    PROPERTIES WILL_FAIL TRUE
)

add_test(
    NAME hampr_cli_invalid_daemon_command
    COMMAND $<TARGET_FILE:hampr> daemon invalid
)

set_tests_properties(
    hampr_cli_invalid_daemon_command
    PROPERTIES WILL_FAIL TRUE
)

add_executable(test_config 
	test_config.cpp
    ${PROJECT_SOURCE_DIR}/src/config.cpp
)

add_test(
    NAME config_valid_task
    COMMAND $<TARGET_FILE:test_config> valid
)

add_test(
    NAME config_invalid_task
    COMMAND $<TARGET_FILE:test_config> invalid
)

add_test(
    NAME config_missing_file
    COMMAND $<TARGET_FILE:test_config> missing
)

target_include_directories(test_config PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
