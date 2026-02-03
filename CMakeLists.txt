cmake_minimum_required(VERSION 3.26)

# Numele proiectului si executabilului
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(${MAIN_PROJECT_NAME})

# Setam standardul C++ la 20 (e mai sigur decat 23 pentru compatibilitate, dar suficient de modern)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Includem fisierele ajutatoare din folderul cmake/
include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

###############################################################################

# AICI ESTE PARTEA IMPORTANTA PE CARE AM MODIFICAT-O
# Lista cu toate fisierele din proiectul tau modularizat
add_executable(${MAIN_EXECUTABLE_NAME}
        main.cpp
        GameApp.cpp
        GameApp.h
        LevelManager.cpp
        LevelManager.h
        Entity.cpp
        Entity.h
        Player.cpp
        Player.h
        Enemy.cpp
        Enemy.h
        Obstacle.cpp
        Obstacle.h
        Utils.h
        GameExceptions.h
        EntityFactory.h
        EntityFactory.cpp
)

###############################################################################

# Activam Sanitizers (te ajuta sa iei punctaj maxim la memory leaks)
set_compiler_flags(RUN_SANITIZERS TRUE TARGET_NAMES ${MAIN_EXECUTABLE_NAME})

###############################################################################

# Instalare si copiere binare (standard din sablon)
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

# COPIEREA FISIERULUI DE INPUT PENTRU TESTE
copy_files(FILES tastatura.txt COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})