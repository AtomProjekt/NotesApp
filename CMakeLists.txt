cmake_minimum_required(VERSION 3.16)
project(NotesApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

add_executable(NotesApp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    resources.qrc
)

if(WIN32)
    target_sources(NotesApp PRIVATE app.rc)
endif()

target_link_libraries(NotesApp
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

set_target_properties(NotesApp PROPERTIES
    OUTPUT_NAME "NotesApp"
)

# Windows — деплой DLL рядом с .exe
if(WIN32)
    add_custom_command(TARGET NotesApp POST_BUILD
        COMMAND windeployqt --release "$<TARGET_FILE:NotesApp>"
        COMMENT "Deploying Qt dependencies..."
    )
endif()
