TEMPLATE = lib
TARGET = pdfview
VERSION = 0.6.1
QT += core gui xml

include(qmake/pdfviewlibconfig.pri)
include(qmake/pdfviewlibdefaults.pri)
#DEFINES += QT_NO_CONTEXTMENU QT_NO_CURSOR QT_NO_SHORTCUT QT_NO_STATUSTIP QT_NO_TOOLTIP QT_NO_WHATSTHIS QT_NO_WHEELEVENT
#DEFINES += QT_NO_CURSOR QT_NO_SHORTCUT QT_NO_STATUSTIP QT_NO_TOOLTIP QT_NO_WHATSTHIS
#DEFINES += QT_STL QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_STRICT_ITERATORS QT_NO_URL_CAST_FROM_STRING QT_NO_KEYWORDS
#DEFINES -= QT_NO_STL QT3_SUPPORT

#CONFIG -= debug_and_release build_all warn_on
CONFIG -= debug
CONFIG += release
unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += poppler-qt4
}
use_synctex {
	DEFINES += USE_SYNCTEX
}

include(src/src.pri)
include(translations/translations.pri)

target.path = $$LIB_INSTALL_DIR
INSTALLS += target

headers.path = $${INCLUDES_INSTALL_DIR}
headers.files += $${PWD}/src/pdfview.h
INSTALLS += headers

license.path = $${PDFVIEWLIB_DATA_INSTALL_DIR}
license.files += LICENSE.GPL2
INSTALLS += license

message(*** PdfView Library v$${VERSION} ***)
message(Qt version: $$[QT_VERSION])
message(The library will be installed in)
message("  $${LIB_INSTALL_DIR}")
message(The header file will be installed in)
message("  $${INCLUDES_INSTALL_DIR}")
message(The translations will be installed in)
message("  $${PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR}")
message()
message("If you would like to change these paths, please")
message("adjust pdfviewlibconfig.pri to your needs and rerun qmake.")
