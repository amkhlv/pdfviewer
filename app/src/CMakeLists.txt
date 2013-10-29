### Definitions and includes

add_definitions(-DPDFVIEWER_TRANSLATIONS_INSTALL_DIR=\"${PDFVIEWER_TRANSLATIONS_INSTALL_DIR}\")
add_definitions(-DPDFVIEWLIB_PRINT_PROGRAM=\"${PDFVIEWLIB_PRINT_PROGRAM}\")

add_definitions(${QT_DEFINITIONS})
add_definitions(${POPPLER_QT4_DEFINITIONS})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDES}
	${POPPLER_QT4_INCLUDE_DIR}
	${PDFVIEWLIB_INCLUDES}
)
if(BUILD_STANDALONE)
	include_directories(
		${ZLIB_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/../../lib/src
	)
endif(BUILD_STANDALONE)

if(USE_QDBUS)
	add_definitions(-DUSE_QDBUS)
endif(USE_QDBUS)

if(USE_DESKTOP_ICONS)
	add_definitions(-DUSE_DESKTOP_ICONS)
endif(USE_DESKTOP_ICONS)

if(BUILD_STANDALONE AND USE_SYNCTEX)
	add_definitions(-DUSE_SYNCTEX)
endif(BUILD_STANDALONE AND USE_SYNCTEX)

### Sources

set(pdfviewer_SRCS
	shortcuthandler/shortcutconfigdialog.cpp
	shortcuthandler/shortcutconfigwidget.cpp
	shortcuthandler/shortcuthandler.cpp
	utils/gotopageaction.cpp
	utils/lineedit.cpp
	utils/messagebox.cpp
	utils/recentfilesaction.cpp
	aboutdialog.cpp
	abstractinfodock.cpp
	configdialog.cpp
	documentobserver.cpp
	embeddedfilesdock.cpp
	findwidget.cpp
	fontsdock.cpp
	infodock.cpp
	label.cpp
	main.cpp
	metadatadock.cpp
	optcontentdock.cpp
	permissionsdock.cpp
	presentationwidget.cpp
	thumbnailsdock.cpp
	tocdock.cpp
	viewer.cpp
)
set(pdfviewer_MOC_HDRS
	shortcuthandler/shortcutconfigdialog.h
	shortcuthandler/shortcutconfigwidget.h
	shortcuthandler/shortcuthandler.h
	utils/gotopageaction.h
	utils/lineedit.h
	utils/recentfilesaction.h
	aboutdialog.h
	abstractinfodock.h
	configdialog.h
	embeddedfilesdock.h
	findwidget.h
	fontsdock.h
	infodock.h
	label.h
	metadatadock.h
	optcontentdock.h
	permissionsdock.h
	presentationwidget.h
	thumbnailsdock.h
	tocdock.h
	viewer.h
)
set(pdfviewer_UI_FILES
	shortcuthandler/shortcutconfigwidget.ui
	configdialog.ui
	findwidget.ui
)

if(USE_QDBUS)
	set(pdfviewer_SRCS
		${pdfviewer_SRCS}
		pdfviewer_adaptor.cpp
		pdfviewer_interface.cpp
	)
	set(pdfviewer_MOC_HDRS
		${pdfviewer_MOC_HDRS}
		pdfviewer_adaptor.h
		pdfviewer_interface.h
	)
endif(USE_QDBUS)

if(BUILD_STANDALONE)
	set(libpwd ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/src)
	set(pdfviewer_SRCS
		${pdfviewer_SRCS}
		${libpwd}/utils/bookmarkshandler.cpp
		${libpwd}/utils/filesettings.cpp
		${libpwd}/utils/globallocale.cpp
		${libpwd}/utils/selectaction.cpp
		${libpwd}/utils/selectpageaction.cpp
		${libpwd}/utils/zoomaction.cpp
		${libpwd}/actionhandler.cpp
		${libpwd}/pageitem.cpp
		${libpwd}/pdfview.cpp
		${libpwd}/printhandler.cpp
	)
	set(pdfviewer_MOC_HDRS
		${pdfviewer_MOC_HDRS}
		${libpwd}/utils/bookmarkshandler.h
		${libpwd}/utils/filesettings.h
		${libpwd}/utils/selectaction.h
		${libpwd}/utils/selectpageaction.h
		${libpwd}/utils/zoomaction.h
		${libpwd}/actionhandler.h
		${libpwd}/pageitem.h
		${libpwd}/pdfview.h
		${libpwd}/pdfview_p.h
		${libpwd}/printhandler.h
	)
	if(USE_SYNCTEX)
		set(pdfviewer_SRCS
			${pdfviewer_SRCS}
			${libpwd}/synctex/synctex_parser.c
			${libpwd}/synctex/synctex_parser_utils.c
			${libpwd}/synctexhandler.cpp
		)
		set(pdfviewer_MOC_HDRS
			${pdfviewer_MOC_HDRS}
			${libpwd}/synctexhandler.h
		)
	endif(USE_SYNCTEX)
endif(BUILD_STANDALONE)

qt4_wrap_cpp(pdfviewer_SRCS ${pdfviewer_MOC_HDRS})
qt4_wrap_ui(pdfviewer_SRCS ${pdfviewer_UI_FILES})
qt4_add_resources(pdfviewer_SRCS pdfviewer.qrc)

### Compile and install

add_executable(pdfviewer ${pdfviewer_SRCS})
if(BUILD_STANDALONE)
	target_link_libraries(pdfviewer ${QT_LIBRARIES} ${POPPLER_QT4_LIBRARIES} ${ZLIB_LIBRARIES})
else(BUILD_STANDALONE)
	target_link_libraries(pdfviewer ${QT_LIBRARIES} ${POPPLER_QT4_LIBRARIES} ${PDFVIEWLIB_LIBRARIES})
endif(BUILD_STANDALONE)
set_target_properties(pdfviewer PROPERTIES OUTPUT_NAME pdfviewer)

install(TARGETS pdfviewer DESTINATION ${BIN_INSTALL_DIR})
