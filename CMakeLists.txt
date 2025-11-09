cmake_minimum_required(VERSION 3.5)
project(SIOVI CXX)

include(CheckIncludeFileCXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always") #подсветка вывода 
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(Qt5Widgets CONFIG REQUIRED)

qt5_wrap_cpp(MOC_SOURCES 
                main_window.h 
                noise_generator.h
                picture_generator.h
)

set(SOURCES
    main.cpp
    main_window.cpp
    noise_generator.cpp
    picture_generator.cpp
    ${MOC_SOURCES}
)

add_executable(${PROJECT_NAME} ${SOURCES})
# Линковка библиотек и зависимостей
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

#в дальнейшем директория для хранения изображений для преобразования 
# qt5_add_resources(RESOURCE_FILES resources.qrc)

target_sources(${PROJECT_NAME} PRIVATE ${RESOURCE_FILES})



