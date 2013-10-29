unix {
	### Install desktop file
	desktop.files = $${PWD}/pdfviewer.desktop
	desktop.path = $$XDG_APPS_INSTALL_DIR
	INSTALLS += desktop

	### Install application icon in different sizes
	icon16.files = $${PWD}/16x16/pdfviewer.png
	icon16.path = $$ICON_INSTALL_DIR/16x16/apps
	INSTALLS += icon16
	icon22.files = $${PWD}/22x22/pdfviewer.png
	icon22.path = $$ICON_INSTALL_DIR/22x22/apps
	INSTALLS += icon22
	icon24.files = $${PWD}/24x24/pdfviewer.png
	icon24.path = $$ICON_INSTALL_DIR/24x24/apps
	INSTALLS += icon24
	icon32.files = $${PWD}/32x32/pdfviewer.png
	icon32.path = $$ICON_INSTALL_DIR/32x32/apps
	INSTALLS += icon32
	icon36.files = $${PWD}/36x36/pdfviewer.png
	icon36.path = $$ICON_INSTALL_DIR/36x36/apps
	INSTALLS += icon36
	icon48.files = $${PWD}/48x48/pdfviewer.png
	icon48.path = $$ICON_INSTALL_DIR/48x48/apps
	INSTALLS += icon48
	icon64.files = $${PWD}/64x64/pdfviewer.png
	icon64.path = $$ICON_INSTALL_DIR/64x64/apps
	INSTALLS += icon64
	icon96.files = $${PWD}/96x96/pdfviewer.png
	icon96.path = $$ICON_INSTALL_DIR/96x96/apps
	INSTALLS += icon96
	icon128.files = $${PWD}/128x128/pdfviewer.png
	icon128.path = $$ICON_INSTALL_DIR/128x128/apps
	INSTALLS += icon128
	icon192.files = $${PWD}/192x192/pdfviewer.png
	icon192.path = $$ICON_INSTALL_DIR/192x192/apps
	INSTALLS += icon192
}
