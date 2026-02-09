cmake_minimum_required(VERSION 3.18)

project(Valeriya LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all .cpp files recursively from Sources
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lexing2023/*.cpp
)

# Define the executable
add_executable(Valeriya_target ${SRC_FILES})

# Function to recursively add include directories
function(add_include_dirs base_dir)
    file(GLOB children RELATIVE ${base_dir} ${base_dir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${base_dir}/${child})
            target_include_directories(Valeriya_target PRIVATE ${base_dir}/${child})
        endif()
    endforeach()
    target_include_directories(Valeriya_target PRIVATE ${base_dir})
endfunction()

# Add include directories for Sources
add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/Sources)
add_include_dirs(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)

# Output configuration
set_target_properties(Valeriya_target PROPERTIES
    OUTPUT_NAME "vri"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Refer to Project's Root as PROJECT_ROOT_DIR MACRO
add_compile_definitions(PROJECT_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
# define this to RUN TESTS
# add_compile_definitions(RUN_TESTS)

# For Linux: 

# mkdir build
# cd build
# cmake ..
# make -j$(nproc)