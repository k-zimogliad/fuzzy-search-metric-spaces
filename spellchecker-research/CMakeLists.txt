cmake_minimum_required(VERSION 3.10)
project(SpellChecker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(spellchecker_lib STATIC 
    src/Distance.cpp 
    src/BaselineChecker.cpp
    src/TrieChecker.cpp
    src/BKTreeChecker.cpp
)

target_include_directories(spellchecker_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(spellcheck main.cpp)

target_link_libraries(spellcheck PRIVATE spellchecker_lib)


if(MSVC)
    # Добавили флаг /utf-8 к остальным опциям
    target_compile_options(spellchecker_lib PRIVATE /O2 /W4 /utf-8)
else()
    target_compile_options(spellchecker_lib PRIVATE -O3 -Wall -Wextra)
endif()
