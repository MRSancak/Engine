cmake_minimum_required(VERSION 3.20)
project(DonderEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DonderEngine WIN32
    Launcher/main.cpp
    Engine/Engine.cpp
    Engine/Engine.h
    Engine/Math.h
    Engine/GameObject.cpp
    Engine/GameObject.h
    Editor/Editor.cpp
    Editor/Editor.h
)

target_compile_definitions(DonderEngine PRIVATE UNICODE _UNICODE)

if(WIN32)
    target_link_libraries(DonderEngine PRIVATE
        user32
        gdi32
        shell32
        comdlg32
    )
endif()

set_target_properties(DonderEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
