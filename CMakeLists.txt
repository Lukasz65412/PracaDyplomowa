cmake_minimum_required(VERSION 4.3)
project(PracaDyplomowa)

set(CMAKE_CXX_STANDARD 23)

add_executable(PracaDyplomowa main.cpp
        BloomFilter.h
        BloomFilterTree.h
        BloomFilter.cpp
        BloomFilterTree.cpp
        DBUtility.h
        MurmurHash3.h
        MurmurHash3.cpp
        node.h
        AVX2Search.h
        AVX2Search.cpp
        DBUtility.cpp
)

target_compile_options(PracaDyplomowa PRIVATE -march=native)

