cmake_minimum_required(VERSION 3.26)

set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

include_directories(Headers)

add_executable(${MAIN_EXECUTABLE_NAME}
        Sources/main.cpp
        Sources/GameApp.cpp
        Headers/GameApp.h

        Sources/LevelManager.cpp
        Headers/LevelManager.h

        Sources/Entity.cpp
        Headers/Entity.h

        Sources/Player.cpp
        Headers/Player.h

        Sources/Enemy.cpp
        Headers/Enemy.h

        Sources/Obstacle.cpp
        Headers/Obstacle.h

        Headers/Utils.h
        Headers/GameExceptions.h

        Sources/EntityFactory.cpp
        Headers/EntityFactory.h
)

set_compiler_flags(RUN_SANITIZERS TRUE TARGET_NAMES ${MAIN_EXECUTABLE_NAME})

install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

# Fisierul tastatura.txt ramane in radacina proiectului (langa CMakeLists.txt)
copy_files(FILES tastatura.txt COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})