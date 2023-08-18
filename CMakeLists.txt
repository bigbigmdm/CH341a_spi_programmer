cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(CH341SPI LANGUAGES C CXX)

# Set the CMAKE_INSTALL_PREFIX to /usr if not specified
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "The default install prefix" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt5Widgets REQUIRED)
find_package(LibUSB REQUIRED)

add_executable(${PROJECT_NAME}
main.cpp
mainwindow.cpp
ch341a.c
qhexedit.cpp
chunks.cpp
commands.cpp
dialogsp.cpp
dialogrp.cpp
searchdialog.cpp
mainwindow.h
ch341a.h
qhexedit.h
chunks.h
commands.h
dialogsp.h
dialogrp.h
searchdialog.h
mainwindow.ui
dialogsp.ui
dialogrp.ui
searchdialog.ui
recource.qrc
	)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} Qt5::Core)
#target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${LibUSB_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION /opt/CH341SPI)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/other/CH341SPI.desktop" DESTINATION /opt/CH341SPI)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/other/CH341SPI.desktop" DESTINATION /usr/share/applications)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/img/write64.png" DESTINATION /opt/CH341SPI)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/database/CH341a.Dat" DESTINATION /opt/CH341SPI)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/other/99-CH341.rules" DESTINATION /etc/udev/rules.d)


