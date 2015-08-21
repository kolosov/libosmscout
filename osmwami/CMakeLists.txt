cmake_minimum_required(VERSION 3.0)

project(osmwami)

#check for xulrunner dir
if(NOT OSMSCOUT_SDK_PATH)
 message(SEND_ERROR "OSMSCOUT_SDK_PATH is not defined")
 set(OSMSCOUT_SDK_PATH "/usr")
else()
 message("OSMSCOUT_SDK_PATH: ${OSMSCOUT_SDK_PATH}")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_definitions( -std=c++11)

#add_subdirectory (src)

# Find the Qt library
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Sql REQUIRED)

#Find OSM libraries
find_library(osmscout_lib osmscout PATHS ${OSMSCOUT_SDK_PATH}/lib)
find_library(osmscoutmap_lib osmscoutmap PATHS ${OSMSCOUT_SDK_PATH}/lib)
find_library(osmscoutmapqt_lib osmscoutmapqt PATHS ${OSMSCOUT_SDK_PATH}/lib)

include_directories(${OSMSCOUT_SDK_PATH}/include)

include_directories ("${PROJECT_SOURCE_DIR}/src")

set(osmwami_src src/OSMScout.cpp src/Settings.cpp src/Theme.cpp 
 src/DBThread.cpp src/MapWidget.cpp src/MainWindow.cpp 
 src/SearchLocationModel.cpp src/RoutingModel.cpp src/POILoader.cpp src/OwnPOI.cpp src/POICategoryModel.cpp)

set(osmwami_headers src/Settings.h src/Theme.h src/DBThread.h 
 src/MapWidget.h src/MainWindow.h src/SearchLocationModel.h 
 src/RoutingModel.h src/POILoader.h src/OwnPOI.h src/POICategoryModel.h)

add_executable(osmwami ${osmwami_src} res.qrc)
target_link_libraries(osmwami Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Quick Qt5::Sql
 ${osmscout_lib} ${osmscoutmap_lib} ${osmscoutmapqt_lib})

