# Find PdfViewLib
# Once done this will define
#
#  PDFVIEWLIB_FOUND				- system has PdfView Library
#  PDFVIEWLIB_INCLUDES			- the PdfView include directory
#  PDFVIEWLIB_LIBRARIES			- link these to use PdfView
#  PDFVIEWLIB_TRANSLATIONSDIR	- PdfView translations installation directory
#  PDFVIEWLIB_VERSION			- the version of the PdfView Library

macro(_pdfviewlib_find_version)
	set(PDFVIEWLIB_VERSION "0.6.1")
	message(STATUS "PdfView Library version: ${PDFVIEWLIB_VERSION}")
endmacro(_pdfviewlib_find_version)

### Search Qt4 libraries and headers and set some useful definitions
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
include(${QT_USE_FILE})

### Search PdfView libraries and headers and set some useful definitions
if (PDFVIEWLIB_FOUND)
	# Already found, nothing more to do except figuring out the version
	_pdfviewlib_find_version()
else (PDFVIEWLIB_FOUND)
	if (PDFVIEWLIB_INCLUDE_DIR AND PDFVIEWLIB_LIBRARY)
		set(PDFVIEWLIB_FIND_QUIETLY TRUE)
	endif (PDFVIEWLIB_INCLUDE_DIR AND PDFVIEWLIB_LIBRARY)

	# Find the location of the library
	find_library(PDFVIEWLIB_LIBRARY
	             NAMES pdfview
	             PATHS ${QT_LIBRARY_DIR} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
	find_library(PDFVIEWLIB_LIBRARY
	             NAMES pdfview)

	# Find the location of the headers
	find_path(PDFVIEWLIB_INCLUDE_DIR
	          NAMES pdfview.h
	          PATHS ${QT_INCLUDE_DIR} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
	find_path(PDFVIEWLIB_INCLUDE_DIR
	          NAMES pdfview.h)

	# Set some useful variables
	if (PDFVIEWLIB_INCLUDE_DIR AND PDFVIEWLIB_LIBRARY)
		set(PDFVIEWLIB_LIBRARIES ${PDFVIEWLIB_LIBRARY})
		set(PDFVIEWLIB_INCLUDES ${PDFVIEWLIB_INCLUDE_DIR})
		get_filename_component(PDFVIEWLIB_LIB_DIR ${PDFVIEWLIB_LIBRARY} PATH)
		set(PDFVIEWLIB_TRANSLATIONSDIR ${PDFVIEWLIB_INCLUDE_DIR}/../share/pdfview)
		set(PDFVIEWLIB_FOUND TRUE)
		_pdfviewlib_find_version()
	else (PDFVIEWLIB_INCLUDE_DIR AND PDFVIEWLIB_LIBRARY)
		set(PDFVIEWLIB_FOUND FALSE)
	endif (PDFVIEWLIB_INCLUDE_DIR AND PDFVIEWLIB_LIBRARY)

	# Tell the user what we have achieved
	if (PDFVIEWLIB_FOUND)
		if (NOT PDFVIEWLIB_FIND_QUIETLY)
			message(STATUS "Found PdfView library: ${PDFVIEWLIB_LIBRARY}")
			message(STATUS "Found PdfView includes: ${PDFVIEWLIB_INCLUDES}")
		endif (NOT PDFVIEWLIB_FIND_QUIETLY)
	else (PDFVIEWLIB_FOUND)
		if (PdfViewLib_FIND_REQUIRED)
			if (NOT PDFVIEWLIB_INCLUDE_DIR)
				message(STATUS "PdfView includes NOT found!")
			endif (NOT PDFVIEWLIB_INCLUDE_DIR)
			if (NOT PDFVIEWLIB_LIBRARY)
				message(STATUS "PdfView library NOT found!")
			endif (NOT PDFVIEWLIB_LIBRARY)
			message(FATAL_ERROR "PdfView library or includes NOT found!")
		else (PdfViewLib_FIND_REQUIRED)
			message(STATUS "Unable to find PdfView")
		endif (PdfViewLib_FIND_REQUIRED)
	endif (PDFVIEWLIB_FOUND)

	mark_as_advanced(PDFVIEWLIB_INCLUDE_DIR PDFVIEWLIB_LIBRARY PDFVIEWLIB_INCLUDES)
endif (PDFVIEWLIB_FOUND)

### Uninstall
# We can create the uninstall rule only once, so we attach a boolean property
# rule_created to the uninstall rule when the latter is created. Before
# creating the uninstall rule we check whether there exists an uninstall rule
# with the property rule_created set to TRUE and we test whether KDE4 has
# already created an uninstall rule.
get_target_property(pdfviewlib_uninstall_rule_created uninstall rule_created)
if (_kde4_uninstall_rule_created) # boolean defined in /usr/share/kde4/apps/cmake/modules/FindKDE4Internal.cmake
	set(pdfviewlib_uninstall_rule_created TRUE)
endif (_kde4_uninstall_rule_created)
if (NOT pdfviewlib_uninstall_rule_created)
	configure_file("${CMAKE_ROOT}/Modules/pdfviewlib_cmake_uninstall.cmake.in"
	               "${CMAKE_CURRENT_BINARY_DIR}/pdfviewlib_cmake_uninstall.cmake"
	               IMMEDIATE @ONLY)
	add_custom_target(uninstall
	                  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/pdfviewlib_cmake_uninstall.cmake")
	set_target_properties(uninstall PROPERTIES rule_created TRUE)
endif(NOT pdfviewlib_uninstall_rule_created)
