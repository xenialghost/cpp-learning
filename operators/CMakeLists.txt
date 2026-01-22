add_executable(operators main.cpp
        src/sizeof_oprator.cpp
        src/alignof_operator.cpp
)

target_include_directories(operators PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
