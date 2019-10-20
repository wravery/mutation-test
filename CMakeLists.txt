cmake_minimum_required(VERSION 3.8.2)
project(mutationTest VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

find_package(cppgraphqlgen REQUIRED)
find_package(pegtl REQUIRED)

add_custom_command(
  OUTPUT
    SomeSchema.cpp
    SomeSchema.h
  COMMAND cppgraphqlgen::schemagen --schema="${CMAKE_CURRENT_SOURCE_DIR}/something.graphql" --prefix="Some" --namespace="some"
  DEPENDS something.graphql
  COMMENT "Generating mock SomeSchema files"
)

add_executable(mutationTest
    mutationTest.cpp
    SomeSchema.cpp)
target_link_libraries(mutationTest PUBLIC
    cppgraphqlgen::graphqlservice
    cppgraphqlgen::graphqlpeg
    cppgraphqlgen::graphqljson)
target_include_directories(mutationTest PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})