cmake_minimum_required(VERSION 3.1.0)

project(InstanceSegmentationAnnotationTool)

set(QT5_DIR "C:/Dev/Qt/Qt5.14.1/5.14.1/msvc2017_64/lib/cmake" CACHE STRING "Additional search paths for QT5")
set(OpenCV_DIR "C:/Dev/opencv/build")

set(CMAKE_AUTOMOC ON)

if(WIN32)
	if(CMAKE_CL_64)
		set(ARCH_TYPE x64)
	elseif((CMAKE_GENERATOR MATCHES "ARM") OR ("${arch_hint}" STREQUAL "ARM") OR 	(CMAKE_VS_EFFECTIVE_PLATFORMS MATCHES "ARM|arm"))
		set(ARCH_TYPE ARM)
	else()
		set(ARCH_TYPE x86)
	endif()
elseif(UNIX)
	set(ARCH_TYPE x86_64)
endif()

find_path(Qt5Widgets_DIR Qt5WidgetsConfig.cmake PATHS "${QT5_DIR}/Qt5Widgets" )
find_path(Qt5Gui_DIR Qt5GuiConfig.cmake         PATHS "${QT5_DIR}/Qt5Gui"     )

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS})

qt5_wrap_ui(UI_HEADER GUI/InstanceSegmentationAnnotationTool.ui )
qt5_add_resources(RESOURCE_FILE GUI/InstanceSegmentationAnnotationTool.qrc)
INCLUDE_DIRECTORIES( include )
add_executable(InstanceSegmentationAnnotationTool MACOSX_BUNDLE WIN32
    src/InstanceSegmentationAnnotationTool.cpp
	include/InstanceSegmentationAnnotationTool.h
    src/ISAT.cpp
	include/ISAT.h
	src/utils.cpp
	include/utils.h
	src/main.cpp
	${UI_HEADER}
	${RESOURCE_FILE}
	GUI/KITECH.rc)

target_link_libraries(InstanceSegmentationAnnotationTool Qt5::Gui Qt5::Widgets ${OpenCV_LIBS})	

set(OpenCV_BIN ${OpenCV_LIB_PATH}/../bin)