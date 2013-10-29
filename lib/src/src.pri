DEFINES += PDFVIEWLIB_PRINT_PROGRAM=\\\"$${PDFVIEWLIB_PRINT_PROGRAM}\\\"
LIBS += -lpoppler-qt4 -lz

win32 {
	INCLUDEPATH += $${_PRO_FILE_PWD_}/win32
	LIBS += -L$${_PRO_FILE_PWD_}/win32
}

OBJECTS_DIR = obj
UI_DIR = ui
MOC_DIR = moc
RCC_DIR = rcc

HEADERS += $${PWD}/utils/bookmarkshandler.h \
	$${PWD}/utils/filesettings.h \
	$${PWD}/utils/globallocale.h \
	$${PWD}/utils/selectaction.h \
	$${PWD}/utils/selectpageaction.h \
	$${PWD}/utils/zoomaction.h \
	$${PWD}/actionhandler.h \
	$${PWD}/pageitem.h \
	$${PWD}/pdfview.h \
	$${PWD}/pdfview_p.h \
	$${PWD}/printhandler.h
SOURCES += $${PWD}/utils/bookmarkshandler.cpp \
	$${PWD}/utils/filesettings.cpp \
	$${PWD}/utils/globallocale.cpp \
	$${PWD}/utils/selectaction.cpp \
	$${PWD}/utils/selectpageaction.cpp \
	$${PWD}/utils/zoomaction.cpp \
	$${PWD}/actionhandler.cpp \
	$${PWD}/pageitem.cpp \
	$${PWD}/pdfview.cpp \
	$${PWD}/printhandler.cpp
use_synctex {
	HEADERS += $${PWD}/synctex/synctex_parser.h \
		$${PWD}/synctex/synctex_parser_utils.h \
		$${PWD}/synctexhandler.h
	SOURCES += $${PWD}/synctex/synctex_parser.c \
		$${PWD}/synctex/synctex_parser_utils.c \
		$${PWD}/synctexhandler.cpp
}
