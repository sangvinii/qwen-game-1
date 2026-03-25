cmake_minimum_required(VERSION 3.10)
project(EvolutionWorld)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем все исходники
add_executable(evolution src/main.cpp)

# Добавляем пути для заголовков
target_include_directories(evolution PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/world
    ${CMAKE_SOURCE_DIR}/src/entity
    ${CMAKE_SOURCE_DIR}/src/simulation
)

# Потоки
find_package(Threads REQUIRED)
target_link_libraries(evolution Threads::Threads)
