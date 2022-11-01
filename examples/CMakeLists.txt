# Examples variables
set( EXAMPLES_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/examples )

# Utility function
function(add_example example_name)
    add_executable(${example_name}.example ${example_name}.cpp)
    target_link_libraries(${example_name}.example networking_db)
    set_target_properties(
        ${example_name}.example
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_RUNTIME_OUTPUT_DIRECTORY}
        EXCLUDE_FROM_ALL 1
    )
    set( examples_suit ${examples_suit} ${example_name}.example PARENT_SCOPE )
endfunction(add_example)

# Examples
add_example( server )

# Creates a target flag for the example executables
add_custom_target ( examples
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EXAMPLES_RUNTIME_OUTPUT_DIRECTORY}/databases
)
add_dependencies ( examples ${examples_suit} )
