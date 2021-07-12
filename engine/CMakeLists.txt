project(engine)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(engine
        src/graphics/multiframe_atlas_entry.c
        src/graphics/tilemap_renderer.c
        src/math/ivec2.c
        src/math/vec2.c
        src/stb/stb_image.c
        src/game.c
        src/main.c
        src/player.c
        src/deferred_tile_update_list.c
        src/tilemap.c)

target_link_libraries(engine glfw OpenGL::GL OpenGL::GLU)