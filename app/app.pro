TEMPLATE = app
ORGNAME = "glad"
APPNAME = "PdfViewer"
APPVERSION = 0.6.1
TARGET = pdfviewer
QT += core gui xml
qdbus {
	QT += dbus
}

include(qmake/pdfviewerconfig.pri)
include(qmake/pdfviewerdefaults.pri)
#DEFINES += QT_NO_CONTEXTMENU QT_NO_CURSOR QT_NO_SHORTCUT QT_NO_STATUSTIP QT_NO_TOOLTIP QT_NO_WHATSTHIS QT_NO_WHEELEVENT
#DEFINES += QT_NO_CURSOR QT_NO_SHORTCUT QT_NO_STATUSTIP QT_NO_TOOLTIP QT_NO_WHATSTHIS
LIBS += -L$${PWD}/../lib/buildqt -L$${OUT_PWD}/../lib

#CONFIG -= debug_and_release build_all warn_on
CONFIG -= debug
CONFIG += release
unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += poppler-qt4
}

INCLUDEPATH += $${PWD}/../lib/src

include(src/src.pri)
include(translations/translations.pri)
include(data/data.pri)
include(win32/win32.pri)

license.path = $${PDFVIEWER_DATA_INSTALL_DIR}
license.files += LICENSE.GPL2
INSTALLS += license

message(*** PdfViewer v$${APPVERSION} ***)
message(Qt version: $$[QT_VERSION])
message(The program will be installed in)
message("  $${BIN_INSTALL_DIR}")
message(The resource files will be installed in)
message("  $${PDFVIEWER_DATA_INSTALL_DIR}")
message(The translations will be installed in)
message("  $${PDFVIEWER_TRANSLATIONS_INSTALL_DIR}")
unix:!macx {
    message(The desktop file will be installed in)
    message("  $${XDG_APPS_INSTALL_DIR}")
    message(The application icon will be installed in)
    message("  $${ICON_INSTALL_DIR}")
}
message()
message("If you would like to change these paths, please")
message("adjust pdfviewerconfig.pri to your needs and rerun qmake.")
