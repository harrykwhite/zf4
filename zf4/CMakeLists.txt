get_filename_component(PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)

project(zf4)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W2 /WX")
endif()

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

add_library(zf4 STATIC
    src/zf4_game.cpp
    src/zf4_window.cpp
    src/zf4_assets.cpp
    src/zf4_rendering.cpp
    src/zf4_pathfinding.cpp
    src/zf4_rand.cpp
    ${PARENT_DIR}/vendor/glad/src/glad.c

    include/zf4.h
    include/zf4_game.h
    include/zf4_window.h
    include/zf4_assets.h
    include/zf4_rendering.h
    include/zf4_pathfinding.h
    include/zf4_rand.h
    include/zf4_utils.h
    ${PARENT_DIR}/vendor/glad/include/glad/glad.h
    ${PARENT_DIR}/vendor/glad/include/KHR/khrplatform.h
)

target_include_directories(zf4 PRIVATE
	include
	${PARENT_DIR}/zf4_common/include
	${PARENT_DIR}/vendor/glad/include
)

target_link_libraries(zf4 PRIVATE zf4_common glfw OpenAL::OpenAL)

target_compile_definitions(zf4 PRIVATE GLFW_INCLUDE_NONE)
