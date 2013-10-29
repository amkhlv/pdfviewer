project(pdfviewer)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
if(BUILD_STANDALONE)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cmake/modules)
endif(BUILD_STANDALONE)

### Application specific definitions

set(PDFVIEWER_APPNAME "PdfViewer")
set(PDFVIEWER_VERSION "0.6.1")

include(PdfViewerConfig)
add_definitions(-DORGNAME="glad" -DAPPNAME="${PDFVIEWER_APPNAME}")
add_definitions(-DAPPVERSION="${PDFVIEWER_VERSION}")
add_definitions(-DPDFVIEWER_TEX_EDITOR="${PDFVIEWER_TEX_EDITOR}")
add_definitions(-DPDFVIEWER_FILE_INFO_CACHE_MAX_SIZE="${PDFVIEWER_FILE_INFO_CACHE_MAX_SIZE}")
#add_definitions(-Wold-style-cast -Wfloat-equal -Woverloaded-virtual -Wshadow -Wundef -Weffc++ -Wdouble-promotion)
#add_definitions(-Wold-style-cast -Woverloaded-virtual -Wundef -Wdouble-promotion)
#add_definitions(-Wall -Wextra)

### Load Qt4, Poppler and zlib

if(USE_QDBUS)
	find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtDBus REQUIRED)
else(USE_QDBUS)
	find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
endif(USE_QDBUS)
include(${QT_USE_FILE})

find_package(PopplerQt4 0.16 REQUIRED) # This is part of the kdelibs development package

if(BUILD_STANDALONE)
	find_package(ZLIB) # Optional
	if(NOT ZLIB_FOUND)
		message(WARNING "ZLib could not be found. Synctex support will not be compiled.")
		set(USE_SYNCTEX FALSE)
	endif(NOT ZLIB_FOUND)
else(BUILD_STANDALONE)
	find_package(PdfView ${PDFVIEWER_VERSION} REQUIRED)
endif(BUILD_STANDALONE)

### Do the hard work

add_subdirectory(src)
add_subdirectory(translations)
add_subdirectory(data)

### Tell the user where we will install everything

message("--------------------------------------------------------------------")
message("   PdfViewer:")
message("   The application binary will be installed in:\n      ${BIN_INSTALL_DIR}")
message("   The application desktop file will be installed in:\n      ${XDG_APPS_INSTALL_DIR}")
message("   The application icons will be installed in:\n      ${ICON_INSTALL_DIR}")
message("   The translation files will be installed in:\n      ${PDFVIEWER_TRANSLATIONS_INSTALL_DIR}")
message("--------------------------------------------------------------------")

### Make packages

include(PdfViewerCPackOptions)
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
