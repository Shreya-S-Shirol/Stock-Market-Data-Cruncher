cmake_minimum_required(VERSION 3.10)
project(StockMarketDataCruncher CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp")

include_directories(include)

file(GLOB SOURCES src/*.cpp)

add_executable(stock_cruncher ${SOURCES})
