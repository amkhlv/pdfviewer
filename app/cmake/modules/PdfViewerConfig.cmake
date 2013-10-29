set(BIN_INSTALL_DIR /usr/bin)
set(DATA_INSTALL_DIR /usr/share)
set(PDFVIEWER_DATA_INSTALL_DIR ${DATA_INSTALL_DIR}/pdfviewer)
set(PDFVIEWER_TRANSLATIONS_INSTALL_DIR ${PDFVIEWER_DATA_INSTALL_DIR}/locale)
set(XDG_APPS_INSTALL_DIR ${DATA_INSTALL_DIR}/applications)
set(ICON_INSTALL_DIR ${DATA_INSTALL_DIR}/icons/hicolor)

set(PDFVIEWER_TEX_EDITOR "texila --line %l %f")
set(PDFVIEWER_FILE_INFO_CACHE_MAX_SIZE "50e6")
set(PDFVIEWLIB_PRINT_PROGRAM lp)

set(USE_QDBUS TRUE) # this is needed for not opening new windows when "forward search" using synctex is triggered
set(USE_DESKTOP_ICONS TRUE) # unhide this to use the icons from the KDE or Gnome icon theme in this app
set(USE_SYNCTEX TRUE) # hide this to disable synctex support; this variable only has an effect when building the standalone version
