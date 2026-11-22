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

##### EXECUTABLES #####
add_executable(test_config 
	test_config.cpp
    ${PROJECT_SOURCE_DIR}/src/config.cpp
)

add_executable(test_client
	test_client.cpp
	${PROJECT_SOURCE_DIR}/src/client.cpp
    ${PROJECT_SOURCE_DIR}/src/ipc.cpp
    ${PROJECT_SOURCE_DIR}/src/task_request.cpp
)

##### TESTS #####
##### test_config #####
add_test(NAME config_valid_task COMMAND $<TARGET_FILE:test_config> valid)
add_test(NAME config_invalid_task COMMAND $<TARGET_FILE:test_config> invalid)
add_test(NAME config_missing_file COMMAND $<TARGET_FILE:test_config> missing)

##### test_client #####
add_test(NAME client_valid_send_command COMMAND $<TARGET_FILE:test_client> valid)
add_test(NAME client_invalid_send_command COMMAND $<TARGET_FILE:test_client> invalid)

set_tests_properties(
    client_valid_send_command
    client_invalid_send_command
    PROPERTIES TIMEOUT 5
)

##### TARGET DIRECTORIES #####
target_include_directories(test_config PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_include_directories(test_client PRIVATE
	${PROJECT_SOURCE_DIR}/include
)
