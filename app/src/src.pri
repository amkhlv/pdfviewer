DEFINES += ORGNAME=\\\"$${ORGNAME}\\\"
DEFINES += APPNAME=\\\"$${APPNAME}\\\"
DEFINES += APPVERSION=\\\"$${APPVERSION}\\\"
DEFINES += PDFVIEWER_TEX_EDITOR=\\\"$${PDFVIEWER_TEX_EDITOR}\\\"
DEFINES += PDFVIEWER_TRANSLATIONS_INSTALL_DIR=\\\"$${PDFVIEWER_TRANSLATIONS_INSTALL_DIR}\\\"
DEFINES += PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR=\\\"$${PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR}\\\"
DEFINES += PDFVIEWER_FILE_INFO_CACHE_MAX_SIZE=\\\"$${PDFVIEWER_FILE_INFO_CACHE_MAX_SIZE}\\\"
INCLUDEPATH += $${PWD} # needed because in shortcuthandler/shortcutconfigwidget.ui there is a reference to utils/lineedit.h
LIBS += -lpoppler-qt4 -lz -lpdfview

win32 {
	INCLUDEPATH += $${_PRO_FILE_PWD_}/win32
	LIBS += -L$${_PRO_FILE_PWD_}/win32
}

qdbus {
	DEFINES += USE_QDBUS
	INCLUDEPATH += $${OUT_PWD}

	QDBUSXML_INPUT += $${PWD}/pdfviewer.xml

	qdbusxml1.name = "Compile pdfviewer_adaptor.h"
	qdbusxml1.input = QDBUSXML_INPUT
	qdbusxml1.output = $${OUT_PWD}/pdfviewer_adaptor.h
	qdbusxml1.commands = qdbusxml2cpp -a ${QMAKE_FILE_OUT}: ${QMAKE_FILE_IN}
	qdbusxml1.variable_out = NEW_HEADERS
	qdbusxml1.CONFIG = target_predeps

	qdbusxml2.name = "Compile pdfviewer_interface.h"
	qdbusxml2.input = QDBUSXML_INPUT
	qdbusxml2.output = $${OUT_PWD}/pdfviewer_interface.h
	qdbusxml2.commands = qdbusxml2cpp -c PdfViewerInterface -p ${QMAKE_FILE_OUT}: ${QMAKE_FILE_IN}
	qdbusxml2.variable_out = NEW_HEADERS
	qdbusxml2.CONFIG = target_predeps

	qdbusxml3.name = "Compile pdfviewer_adaptor.cpp"
	qdbusxml3.input = QDBUSXML_INPUT
	qdbusxml3.output = $${OUT_PWD}/pdfviewer_adaptor.cpp
	qdbusxml3.commands = qdbusxml2cpp -i pdfviewer_adaptor.h -a :${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
	qdbusxml3.variable_out = SOURCES
	qdbusxml3.CONFIG = target_predeps

	qdbusxml4.name = "Compile pdfviewer_interface.cpp"
	qdbusxml4.input = QDBUSXML_INPUT
	qdbusxml4.output = $${OUT_PWD}/pdfviewer_interface.cpp
	qdbusxml4.commands = qdbusxml2cpp -c PdfViewerInterface -i pdfviewer_interface.h -p :${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
	qdbusxml4.variable_out = SOURCES
	qdbusxml4.CONFIG = target_predeps

	qdbusxml5.name = "Compile moc_pdfviewer_adaptor.cpp and moc_pdfviewer_interface.cpp"
	qdbusxml5.input = NEW_HEADERS
	qdbusxml5.output = $${OUT_PWD}/moc/moc_${QMAKE_FILE_BASE}.cpp
	qdbusxml5.commands = $${QMAKE_MOC} $(DEFINES) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
	qdbusxml5.variable_out = SOURCES
	qdbusxml5.CONFIG = target_predeps

	QMAKE_EXTRA_COMPILERS += qdbusxml1 qdbusxml2 qdbusxml3 qdbusxml4 qdbusxml5
}

OBJECTS_DIR = obj
UI_DIR = ui
MOC_DIR = moc
RCC_DIR = rcc

FORMS += $${PWD}/shortcuthandler/shortcutconfigwidget.ui \
	$${PWD}/configdialog.ui \
	$${PWD}/findwidget.ui
HEADERS += $${PWD}/shortcuthandler/shortcutconfigdialog.h \
	$${PWD}/shortcuthandler/shortcutconfigwidget.h \
	$${PWD}/shortcuthandler/shortcuthandler.h \
	$${PWD}/utils/gotopageaction.h \
	$${PWD}/utils/lineedit.h \
	$${PWD}/utils/messagebox.h \
	$${PWD}/utils/recentfilesaction.h \
	$${PWD}/aboutdialog.h \
	$${PWD}/abstractinfodock.h \
	$${PWD}/configdialog.h \
	$${PWD}/documentobserver.h \
	$${PWD}/embeddedfilesdock.h \
	$${PWD}/findwidget.h \
	$${PWD}/fontsdock.h \
	$${PWD}/infodock.h \
	$${PWD}/label.h \
	$${PWD}/metadatadock.h \
	$${PWD}/optcontentdock.h \
	$${PWD}/permissionsdock.h \
	$${PWD}/presentationwidget.h \
	$${PWD}/thumbnailsdock.h \
	$${PWD}/tocdock.h \
	$${PWD}/viewer.h
SOURCES += $${PWD}/shortcuthandler/shortcutconfigdialog.cpp \
	$${PWD}/shortcuthandler/shortcutconfigwidget.cpp \
	$${PWD}/shortcuthandler/shortcuthandler.cpp \
	$${PWD}/utils/gotopageaction.cpp \
	$${PWD}/utils/lineedit.cpp \
	$${PWD}/utils/messagebox.cpp \
	$${PWD}/utils/recentfilesaction.cpp \
	$${PWD}/aboutdialog.cpp \
	$${PWD}/abstractinfodock.cpp \
	$${PWD}/configdialog.cpp \
	$${PWD}/documentobserver.cpp \
	$${PWD}/embeddedfilesdock.cpp \
	$${PWD}/findwidget.cpp \
	$${PWD}/fontsdock.cpp \
	$${PWD}/infodock.cpp \
	$${PWD}/label.cpp \
	$${PWD}/main.cpp \
	$${PWD}/metadatadock.cpp \
	$${PWD}/optcontentdock.cpp \
	$${PWD}/permissionsdock.cpp \
	$${PWD}/presentationwidget.cpp \
	$${PWD}/thumbnailsdock.cpp \
	$${PWD}/tocdock.cpp \
	$${PWD}/viewer.cpp
RESOURCES = $${PWD}/pdfviewer.qrc

target.path = $$BIN_INSTALL_DIR
INSTALLS += target
