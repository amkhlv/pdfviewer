#include(../PdfViewerConfig.cmake)

### Install desktop file

install(FILES pdfviewer.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

### Install icon

set(ICON_SIZES 16 22 24 32 36 48 64 96 128 192)
foreach(_iconSize ${ICON_SIZES})
	install(FILES ${_iconSize}x${_iconSize}/pdfviewer.png DESTINATION ${ICON_INSTALL_DIR}/${_iconSize}x${_iconSize}/apps)
endforeach(_iconSize ${ICON_SIZES})
