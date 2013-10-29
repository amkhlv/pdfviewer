project(pdfviewlib)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

### Application specific definitions

set(PDFVIEWLIB_VERSION "0.6.1")

include(PdfViewLibConfig)
add_definitions(-DQT_NO_DEBUG -DQT_SHARED)
#add_definitions(-DQT_STL -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII -DQT_STRICT_ITERATORS -DQT_NO_URL_CAST_FROM_STRING -DQT_NO_KEYWORDS)
#remove_definitions(-DQT_NO_STL -DQT3_SUPPORT)

### Load Qt4, Poppler and zlib

find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
include(${QT_USE_FILE})

find_package(PopplerQt4 0.16 REQUIRED)

find_package(ZLIB) # Optional
if(NOT ZLIB_FOUND)
	message(WARNING "ZLib could not be found. Synctex support will not be compiled.")
	set(USE_SYNCTEX FALSE)
endif(NOT ZLIB_FOUND)

### Do the hard work

add_subdirectory(src)
add_subdirectory(translations)

### Tell the user where we will install everything

message("--------------------------------------------------------------------")
message("   PdfView Library v${PDFVIEWLIB_VERSION}:")
message("   The library will be installed in:\n      ${LIB_INSTALL_DIR}")
message("   The header files will be installed in:\n      ${INCLUDES_INSTALL_DIR}")
message("   The translation files will be installed in:\n      ${PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR}")
message("   If any of the above paths is relative (i.e. does not start with /),\n   then it is relative to ${CMAKE_INSTALL_PREFIX}")
message("--------------------------------------------------------------------")

### Install

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindPdfView.cmake DESTINATION ${CMAKE_ROOT}/Modules)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/pdfviewlib_cmake_uninstall.cmake.in DESTINATION ${CMAKE_ROOT}/Modules)

### Uninstall (must be the same as in FindPdfViewLib.cmake except for the path to pdfviewlib_cmake_uninstall.cmake.in which is not installed yet)

get_target_property(pdfviewlib_uninstall_rule_created uninstall rule_created)
if (NOT pdfviewlib_uninstall_rule_created)
	set(pdfviewlib_uninstall_rule_created TRUE)
	if (pdfviewlib_compile_not_standalone)
		set(CMAKE_CURRENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/..") # dirty hack to let the uninstall find install_manifest.txt
	endif (pdfviewlib_compile_not_standalone)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/pdfviewlib_cmake_uninstall.cmake.in"
	               "${CMAKE_CURRENT_BINARY_DIR}/pdfviewlib_cmake_uninstall.cmake"
	               IMMEDIATE @ONLY)
	add_custom_target(uninstall
	                  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/pdfviewlib_cmake_uninstall.cmake")
	set_target_properties(uninstall PROPERTIES rule_created TRUE)
endif(NOT pdfviewlib_uninstall_rule_created)

### Make packages

include(PdfViewLibCPackOptions)
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
