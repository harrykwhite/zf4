project(zf4_common)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W2 /WX")
endif()

add_library(zf4_common STATIC
    src/zf4c_io.cpp
    src/zf4c_math.cpp
    src/zf4c_mem.cpp
    
    include/zf4c.h
    include/zf4c_io.h
    include/zf4c_math.h
    include/zf4c_assets.h
    include/zf4c_mem.h
)

target_include_directories(zf4_common PRIVATE include)
