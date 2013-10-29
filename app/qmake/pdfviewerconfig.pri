BIN_INSTALL_DIR = /usr/local/bin
DATA_INSTALL_DIR = /usr/local/share
PDFVIEWER_DATA_INSTALL_DIR = $${DATA_INSTALL_DIR}/pdfviewer
PDFVIEWER_TRANSLATIONS_INSTALL_DIR = $${PDFVIEWER_DATA_INSTALL_DIR}/locale
PDFVIEWLIB_DATA_INSTALL_DIR = $${DATA_INSTALL_DIR}/pdfviewlib # this variable only has an effect when building the standalone version
PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR = $${PDFVIEWLIB_DATA_INSTALL_DIR}/locale # idem
XDG_APPS_INSTALL_DIR = $${DATA_INSTALL_DIR}/applications
ICON_INSTALL_DIR = $${DATA_INSTALL_DIR}/icons/hicolor

PDFVIEWLIB_PRINT_PROGRAM = lpr

CONFIG += qdbus # this is needed for not opening new windows when "forward search" using synctex is triggered
#DEFINES += USE_DESKTOP_ICONS # unhide this to use the icons from the KDE or Gnome icons theme in this app
CONFIG += use_synctex # hide this to disable synctex support; this variable only has an effect when building the standalone version
