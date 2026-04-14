cmake_minimum_required(VERSION 3.20)
project(solar LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# ── GLFW ──────────────────────────────────────────────────────────────
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# ── GLM ───────────────────────────────────────────────────────────────
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

# ── GLAD (generated source, small enough to vendor via FetchContent) ─
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    SOURCE_SUBDIR  cmake
)

FetchContent_MakeAvailable(glfw glm glad)

glad_add_library(glad_gl REPRODUCIBLE API gl:core=3.3)

# ── Sources ───────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/rendering/shader.cpp
    src/rendering/sphere_mesh.cpp
    src/rendering/camera.cpp
    src/rendering/renderer.cpp
    src/rendering/trail.cpp
    src/simulation/physics.cpp
)

add_executable(solar ${SOURCES})

target_include_directories(solar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(solar PRIVATE glfw glad_gl glm::glm)

# ── Copy shaders to build directory ──────────────────────────────────
file(GLOB SHADER_FILES ${CMAKE_SOURCE_DIR}/src/shaders/*.vert ${CMAKE_SOURCE_DIR}/src/shaders/*.frag)
foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    configure_file(${SHADER} ${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME} COPYONLY)
endforeach()

# macOS framework linking
if(APPLE)
    target_link_libraries(solar PRIVATE "-framework OpenGL" "-framework Cocoa" "-framework IOKit")
endif()
