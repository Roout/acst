cmake_minimum_required (VERSION 3.16)

set(This asio-http-client)
project(${This})

set(CMAKE_CXX_STANDARD 17)

set(headers)
set(sources)

list(APPEND headers
)

list(APPEND sources 
  "main.cpp"
)

add_executable (${This} ${sources} ${headers})

add_definitions(-DBOOST_DATE_TIME_NO_LIB -DBOOST_REGEX_NO_LIB)

find_package(Boost REQUIRED)

message(STATUS "Common perform search")
message(STATUS "BOOST_ROOT ${BOOST_ROOT}")
message(STATUS "Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")

if(Boost_FOUND)
	target_include_directories(${This} 
		PUBLIC ${Boost_INCLUDE_DIRS} 
	)
endif()
