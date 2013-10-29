### Definitions and includes

add_definitions(-DPDFVIEWLIB_PRINT_PROGRAM=\"${PDFVIEWLIB_PRINT_PROGRAM}\")

add_definitions(
	${QT_DEFINITIONS}
	${POPPLER_QT4_DEFINITIONS}
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDES}
	${POPPLER_QT4_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRS}
)

if(USE_SYNCTEX)
	add_definitions(-DUSE_SYNCTEX)
endif(USE_SYNCTEX)

### Sources

set(pdfviewlib_SRCS
	utils/bookmarkshandler.cpp
	utils/filesettings.cpp
	utils/globallocale.cpp
	utils/selectaction.cpp
	utils/selectpageaction.cpp
	utils/zoomaction.cpp
	actionhandler.cpp
	pageitem.cpp
	pdfview.cpp
	printhandler.cpp
)
set(pdfviewlib_MOC_HDRS
	utils/bookmarkshandler.h
	utils/filesettings.h
	utils/selectaction.h
	utils/selectpageaction.h
	utils/zoomaction.h
	pageitem.h
	pdfview.h
	pdfview_p.h
	printhandler.h
)
if(USE_SYNCTEX)
	set(pdfviewlib_SRCS
		${pdfviewlib_SRCS}
		synctex/synctex_parser.c
		synctex/synctex_parser_utils.c
		synctexhandler.cpp
	)
	set(pdfviewlib_MOC_HDRS
		${pdfviewlib_MOC_HDRS}
		synctexhandler.h
	)
endif(USE_SYNCTEX)
qt4_wrap_cpp(pdfviewlib_SRCS ${pdfviewlib_MOC_HDRS})

### Compile and install

add_library(pdfview SHARED ${pdfviewlib_SRCS})
target_link_libraries(pdfview ${QT_LIBRARIES} ${POPPLER_QT4_LIBRARIES} ${ZLIB_LIBRARIES})
set_property(TARGET pdfview PROPERTY VERSION ${PDFVIEWLIB_VERSION})
string(REGEX REPLACE "\\..*" "" PDFVIEWLIB_VERSION_MAJOR ${PDFVIEWLIB_VERSION})
set_property(TARGET pdfview PROPERTY SOVERSION ${PDFVIEWLIB_VERSION_MAJOR})

install(TARGETS pdfview LIBRARY DESTINATION ${LIB_INSTALL_DIR})
install(FILES pdfview.h DESTINATION ${INCLUDES_INSTALL_DIR})
