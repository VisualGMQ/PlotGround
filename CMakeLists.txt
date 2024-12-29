cmake_minimum_required(VERSION 3.20)

project(PlotGround
    LANGUAGES C CXX
    VERSION 0.1.0)

file(GLOB_RECURSE HEADERS ./src/*.hpp)

aux_source_directory(src GAME_SRC)
aux_source_directory(src/3rdlib/imgui GAME_SRC)
aux_source_directory(src/3rdlib/tinyxml2 GAME_SRC)
aux_source_directory(src/3rdlib/implot GAME_SRC)
aux_source_directory(src/input GAME_SRC)
aux_source_directory(src/controller GAME_SRC)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE GAME_HEADERS src/*.hpp src/*.h)

    find_package(SDL2 REQUIRED CONFIG)
    find_package(SDL2_image REQUIRED CONFIG)
    find_package(SDL2_ttf REQUIRED CONFIG)
    find_package(SDL2_mixer REQUIRED CONFIG)

    add_executable(plot)
    target_link_libraries(plot
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )
    target_link_libraries(plot PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
    target_link_libraries(plot PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
    target_link_libraries(plot PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)


target_sources(plot PRIVATE ${GAME_SRC} ${GAME_HEADERS})
target_sources(plot PRIVATE ${GAME_ASSETS})

target_compile_features(plot PRIVATE cxx_std_17)
target_include_directories(plot PRIVATE src)
target_compile_definitions(plot PRIVATE _CRT_SECURE_NO_WARNINGS)
target_precompile_headers(plot PRIVATE src/pch.hpp)
