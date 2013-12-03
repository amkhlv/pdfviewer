/*
 * Copyright (C) 2008-2009, Pino Toscano <pino@kde.org>
 * Copyright (C) 2008, Albert Astals Cid <aacid@kde.org>
 * Copyright (C) 2009, Shawn Rutledge <shawn.t.rutledge@gmail.com>
 * Copyright (C) 2012, Glad Deschrijver <glad.deschrijver@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QDebug>
#include "viewer.h"

#include "aboutdialog.h"
#include "configdialog.h"
#include "embeddedfilesdock.h"
#include "findwidget.h"
#include "fontsdock.h"
#include "infodock.h"
#include "metadatadock.h"
#include "optcontentdock.h"
#include "permissionsdock.h"
#include "presentationwidget.h"
#include "thumbnailsdock.h"
#include "tocdock.h"
#include "shortcuthandler/shortcuthandler.h"
#include "utils/icon.h"
#include "utils/recentfilesaction.h"

#include <pdfview.h>

#include <poppler-qt4.h>

#include <QtCore/QDir>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QPointer>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
//#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QRadioButton>
#include <QtGui/QToolBar>
#include <QtGui/QWidgetAction>

PdfViewer::PdfViewer()
    : QMainWindow()
	, m_currentPage(0)
	, m_showMenuBar(false)
	, m_reloadTimer(0)
	, m_findWidget(0)
{
//QTime t = QTime::currentTime();
    setWindowTitle(QCoreApplication::applicationName());

	// set icon theme search paths
    QStringList themeSearchPaths;
    themeSearchPaths << QDir::homePath() + "/.local/share/icons/";
    themeSearchPaths << QIcon::themeSearchPaths();
    QIcon::setThemeSearchPaths(themeSearchPaths);

	// setup shortcut handler
#ifndef QT_NO_SHORTCUT
	ShortcutHandler *shortcutHandler = new ShortcutHandler(this);
#endif // QT_NO_SHORTCUT
	QSettings *settingsObject = new QSettings(this);
#ifndef QT_NO_SHORTCUT
	shortcutHandler->setSettingsObject(settingsObject);
#endif // QT_NO_SHORTCUT

	// setup recent files menu
	m_fileOpenRecentAction = new RecentFilesAction(Icon("document-open-recent"), tr("Open &Recent", "Action: open recent file"), this);
	m_fileOpenRecentAction->setSettingsObject(settingsObject);
	connect(m_fileOpenRecentAction, SIGNAL(fileSelected(QString)), this, SLOT(slotLoadDocument(QString)));

	// setup the main view
	m_pdfView = new PdfView(this);
	connect(m_pdfView, SIGNAL(scrollPositionChanged(qreal,int)), this, SLOT(slotViewScrollPositionChanged(qreal,int)));
	connect(m_pdfView, SIGNAL(openTexDocument(QString,int)), this, SLOT(slotOpenTexDocument(QString,int)));
	connect(m_pdfView, SIGNAL(mouseToolChanged(PdfView::MouseTool)), this, SLOT(slotSelectMouseTool(PdfView::MouseTool)));

	// setup the central widget
	QWidget *mainWidget = new QWidget(this);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pdfView);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);

	createActions(); // must happen after the setup of m_pdfView
	QSettings settings;
	settings.beginGroup("MainWindow");
	m_showMenuBar = settings.value("ShowMenuBar", false).toBool();
	settings.endGroup();
	if (m_showMenuBar)
	{
		createMenus();
		createToolBars();
	}
	else
		createToolBarsWhenNoMenuBar();
	createDocks();

    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->m_viewer = this;
    }

    // activate AA by default
    m_settingsTextAAAction->setChecked(true);
    m_settingsGfxAAAction->setChecked(true);

	// watch file changes
	m_watcher = new QFileSystemWatcher(this);
	connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(slotReloadWhenIdle(QString)));

	// setup presentation view (we must do this here in order to have access to the shortcuts)
	m_presentationWidget = new PresentationWidget;
	connect(m_presentationWidget, SIGNAL(pageChanged(int)), this, SLOT(slotGoToPage(int)));
	connect(m_presentationWidget, SIGNAL(doAction(Poppler::LinkAction::ActionType)), this, SLOT(slotDoAction(Poppler::LinkAction::ActionType)));

	readSettings();
	m_pdfView->setFocus();
//qCritical() << t.msecsTo(QTime::currentTime());
}

PdfViewer::~PdfViewer()
{
//	m_presentationWidget->deleteLater();
	delete m_presentationWidget;
	writeSettings();
    closeDocument();
}

QSize PdfViewer::sizeHint() const
{
    return QSize(500, 600);
}

/*******************************************************************/

void PdfViewer::createActions()
{
	// File
    m_fileOpenAction = new QAction(Icon("document-open"), tr("&Open...", "Action: open file"), this);
#ifndef QT_NO_SHORTCUT
    m_fileOpenAction->setShortcut(QKeySequence::Open);
#endif // QT_NO_SHORTCUT
	m_fileOpenAction->setObjectName("file_open");
	connect(m_fileOpenAction, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_fileOpenAction);
#endif // QT_NO_SHORTCUT

    m_fileSaveCopyAction = new QAction(Icon("document-save-as"), tr("&Save a Copy...", "Action: save a copy of the open file"), this);
#ifndef QT_NO_SHORTCUT
    m_fileSaveCopyAction->setShortcut(tr("Ctrl+Shift+S"));
#endif // QT_NO_SHORTCUT
    m_fileSaveCopyAction->setEnabled(false);
	m_fileSaveCopyAction->setObjectName("file_save_copy");
	connect(m_fileSaveCopyAction, SIGNAL(triggered()), this, SLOT(slotSaveCopy()));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_fileSaveCopyAction);
#endif // QT_NO_SHORTCUT

	m_printAction = m_pdfView->action(PdfView::Print);
	m_printAction->setIcon(Icon("document-print"));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_printAction);
#endif // QT_NO_SHORTCUT

    m_quitAction = new QAction(Icon("application-exit"), tr("&Quit", "Action: quit the application"), this);
#ifndef QT_NO_SHORTCUT
    m_quitAction->setShortcut(QKeySequence::Quit);
#endif // QT_NO_SHORTCUT
	m_quitAction->setObjectName("application_exit");
//	connect(m_quitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
	connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_quitAction);
#endif // QT_NO_SHORTCUT

	// Edit
	m_findAction = new QAction(Icon("edit-find"), tr("&Find...", "Action"), this);
#ifndef QT_NO_SHORTCUT
	m_findAction->setShortcut(QKeySequence::Find);
#endif // QT_NO_SHORTCUT
	m_findAction->setEnabled(false);
	m_findAction->setObjectName("edit_find");
	connect(m_findAction, SIGNAL(triggered()), this, SLOT(slotFind()));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_findAction);
#endif // QT_NO_SHORTCUT

	m_findNextAction = new QAction(tr("Find &Next", "Action"), this);
#ifndef QT_NO_SHORTCUT
	m_findNextAction->setShortcut(QKeySequence::FindNext);
#endif // QT_NO_SHORTCUT
	m_findNextAction->setEnabled(false);
	m_findNextAction->setObjectName("edit_find_next");
	connect(m_findNextAction, SIGNAL(triggered()), this, SLOT(slotFindNext()));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_findNextAction);
#endif // QT_NO_SHORTCUT

	m_findPreviousAction = new QAction(tr("Find &Previous", "Action"), this);
#ifndef QT_NO_SHORTCUT
	m_findPreviousAction->setShortcut(QKeySequence::FindPrevious);
#endif // QT_NO_SHORTCUT
	m_findPreviousAction->setEnabled(false);
	m_findPreviousAction->setObjectName("edit_find_previous");
	connect(m_findPreviousAction, SIGNAL(triggered()), this, SLOT(slotFindPrevious()));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_findPreviousAction);
#endif // QT_NO_SHORTCUT

	// View
	m_zoomInAction = m_pdfView->action(PdfView::ZoomIn);
	m_zoomInAction->setIcon(Icon("zoom-in"));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_zoomInAction);
#endif // QT_NO_SHORTCUT

	m_zoomOutAction = m_pdfView->action(PdfView::ZoomOut);
	m_zoomOutAction->setIcon(Icon("zoom-out"));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_zoomOutAction);
#endif // QT_NO_SHORTCUT

	m_zoomAction = m_pdfView->action(PdfView::Zoom);

	m_showPresentationAction = new QAction(Icon("view-presentation"), tr("P&resentation", "Action"), this);
#ifndef QT_NO_SHORTCUT
	m_showPresentationAction->setShortcut(tr("Ctrl+Shift+P"));
#endif // QT_NO_SHORTCUT
	m_showPresentationAction->setEnabled(false);
	m_showPresentationAction->setObjectName("view_presentation");
	connect(m_showPresentationAction, SIGNAL(triggered()), this, SLOT(slotShowPresentation()));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_showPresentationAction);
#endif // QT_NO_SHORTCUT

	// Go
	m_goToStartAction = m_pdfView->action(PdfView::GoToStartOfDocument);
	m_goToStartAction->setIcon(Icon("go-first"));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_goToStartAction);
#endif // QT_NO_SHORTCUT

	m_goToEndAction = m_pdfView->action(PdfView::GoToEndOfDocument);
	m_goToEndAction->setIcon(Icon("go-last"));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_goToEndAction);
#endif // QT_NO_SHORTCUT

	m_goToPreviousPageAction = m_pdfView->action(PdfView::GoToPreviousPage);
	m_goToPreviousPageAction->setIcon(Icon("go-previous"));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_goToPreviousPageAction);
#endif // QT_NO_SHORTCUT

	m_goToNextPageAction = m_pdfView->action(PdfView::GoToNextPage);
	m_goToNextPageAction->setIcon(Icon("go-next"));
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_goToNextPageAction);
#endif // QT_NO_SHORTCUT

	m_goToPageAction = m_pdfView->action(PdfView::GoToPage);

    m_amkhlvDnAction = m_pdfView->action(PdfView::AmkhlvDn);
    m_amkhlvUpAction = m_pdfView->action(PdfView::AmkhlvUp);
    m_amkhlvDnFAction = m_pdfView->action(PdfView::AmkhlvDnF);
    m_amkhlvUpFAction = m_pdfView->action(PdfView::AmkhlvUpF);
    m_amkhlvRtAction = m_pdfView->action(PdfView::AmkhlvRt);
    m_amkhlvLtAction = m_pdfView->action(PdfView::AmkhlvLt);
    m_amkhlvRtFAction = m_pdfView->action(PdfView::AmkhlvRtF);
    m_amkhlvLtFAction = m_pdfView->action(PdfView::AmkhlvLtF);


#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_amkhlvDnAction);
	ShortcutHandler::instance()->addAction(m_amkhlvDnFAction);
	ShortcutHandler::instance()->addAction(m_amkhlvRtAction);
	ShortcutHandler::instance()->addAction(m_amkhlvRtFAction);
#endif // QT_NO_SHORTCUT
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(m_amkhlvUpAction);
	ShortcutHandler::instance()->addAction(m_amkhlvUpFAction);
	ShortcutHandler::instance()->addAction(m_amkhlvLtAction);
	ShortcutHandler::instance()->addAction(m_amkhlvLtFAction);
#endif // QT_NO_SHORTCUT


	// Tools
	m_mouseBrowseAction = m_pdfView->action(PdfView::MouseToolBrowse);
	m_mouseBrowseAction->setIcon(Icon("input-mouse"));
#ifndef QT_NO_SHORTCUT
	m_mouseBrowseAction->setShortcut(tr("Ctrl+1"));
	ShortcutHandler::instance()->addAction(m_mouseBrowseAction);
#endif // QT_NO_SHORTCUT

	m_mouseMagnifyAction = m_pdfView->action(PdfView::MouseToolMagnify);
	m_mouseMagnifyAction->setIcon(Icon("page-zoom"));
#ifndef QT_NO_SHORTCUT
	m_mouseMagnifyAction->setShortcut(tr("Ctrl+2"));
	ShortcutHandler::instance()->addAction(m_mouseMagnifyAction);
#endif // QT_NO_SHORTCUT

	m_mouseSelectionAction = m_pdfView->action(PdfView::MouseToolSelection);
	m_mouseSelectionAction->setIcon(Icon("select-rectangular"));
#ifndef QT_NO_SHORTCUT
	m_mouseSelectionAction->setShortcut(tr("Ctrl+3"));
	ShortcutHandler::instance()->addAction(m_mouseSelectionAction);
#endif // QT_NO_SHORTCUT

	m_mouseTextSelectionAction = m_pdfView->action(PdfView::MouseToolTextSelection);
	m_mouseTextSelectionAction->setIcon(Icon("draw-text"));
#ifndef QT_NO_SHORTCUT
	m_mouseTextSelectionAction->setShortcut(tr("Ctrl+4"));
	ShortcutHandler::instance()->addAction(m_mouseTextSelectionAction);
#endif // QT_NO_SHORTCUT

	// Settings
    m_settingsTextAAAction = new QAction(tr("&Text Antialias", "Action: enable/disable antialias"), this);
    m_settingsTextAAAction->setCheckable(true);
    connect(m_settingsTextAAAction, SIGNAL(toggled(bool)), this, SLOT(slotToggleTextAA(bool)));

    m_settingsGfxAAAction = new QAction(tr("&Graphics Antialias", "Action: enable/disable antialias"), this);
    m_settingsGfxAAAction->setCheckable(true);
    connect(m_settingsGfxAAAction, SIGNAL(toggled(bool)), this, SLOT(slotToggleGfxAA(bool)));

	QMenu *settingsRenderMenu = new QMenu(tr("&Render Backend", "Menu title"), this);
	m_settingsRenderBackendGrp = new QActionGroup(settingsRenderMenu);
	m_settingsRenderBackendGrp->setExclusive(true);
	QAction *action = settingsRenderMenu->addAction(tr("&Splash", "Action: select render backend"));
	action->setCheckable(true);
	action->setChecked(true);
	action->setData(qVariantFromValue(int(Poppler::Document::SplashBackend)));
	m_settingsRenderBackendGrp->addAction(action);
	action = settingsRenderMenu->addAction(tr("&Arthur", "Action: select render backend"));
	action->setCheckable(true);
	action->setData(qVariantFromValue(int(Poppler::Document::ArthurBackend)));
	m_settingsRenderBackendGrp->addAction(action);
	connect(m_settingsRenderBackendGrp, SIGNAL(triggered(QAction*)), this, SLOT(slotRenderBackend(QAction*)));
	m_renderBackendAction = new QAction(tr("&Render Backend", "Menu title"), this);
	m_renderBackendAction->setMenu(settingsRenderMenu);

	m_configureAction = new QAction(Icon("configure"), tr("&Configure %1...", "Action: show configuration dialog").arg(QCoreApplication::applicationName()), this);
	connect(m_configureAction, SIGNAL(triggered()), this, SLOT(slotConfigure()));
}

void PdfViewer::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File", "Menu title"));
	fileMenu->addAction(m_fileOpenAction);
	fileMenu->addAction(m_fileOpenRecentAction);
    fileMenu->addSeparator();
	fileMenu->addAction(m_fileSaveCopyAction);
    fileMenu->addSeparator();
	fileMenu->addAction(m_printAction);
    fileMenu->addSeparator();
	fileMenu->addAction(m_quitAction);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit", "Menu title"));
	editMenu->addAction(m_findAction);
	editMenu->addAction(m_findNextAction);
	editMenu->addAction(m_findPreviousAction);

    m_viewMenu = menuBar()->addMenu(tr("&View", "Menu title"));
	m_viewMenu->addAction(m_zoomInAction);
	m_viewMenu->addAction(m_zoomOutAction);
	m_viewMenu->addSeparator();
	m_viewMenu->addAction(m_showPresentationAction);
	m_viewMenu->addSeparator();

    QMenu *goMenu = menuBar()->addMenu(tr("&Go", "Menu title"));
	goMenu->addAction(m_goToPreviousPageAction);
	goMenu->addAction(m_goToNextPageAction);
	goMenu->addSeparator();
	goMenu->addAction(m_goToStartAction);
	goMenu->addAction(m_goToEndAction);
//	goMenu->addSeparator();
//	goMenu->addAction(m_goToAction);

	menuBar()->addAction(m_pdfView->action(PdfView::Bookmarks));
	m_pdfView->action(PdfView::SetBookmark)->setIcon(Icon("bookmark-new"));
    m_pdfView->action(PdfView::UnSetBookmark)->setIcon(Icon("dialog-cancel"));

	m_pdfView->action(PdfView::PreviousBookmark)->setIcon(Icon("go-up"));
	m_pdfView->action(PdfView::NextBookmark)->setIcon(Icon("go-down"));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::SetBookmark));
    ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::UnSetBookmark));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::PreviousBookmark));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::NextBookmark));

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools", "Menu title"));
	toolsMenu->addAction(m_mouseBrowseAction);
	toolsMenu->addAction(m_mouseMagnifyAction);
	toolsMenu->addAction(m_mouseSelectionAction);
	toolsMenu->addAction(m_mouseTextSelectionAction);
    toolsMenu->addAction(m_amkhlvDnAction);
    toolsMenu->addAction(m_amkhlvUpAction);
    toolsMenu->addAction(m_amkhlvDnFAction);
    toolsMenu->addAction(m_amkhlvUpFAction);
    toolsMenu->addAction(m_amkhlvRtAction);
    toolsMenu->addAction(m_amkhlvLtAction);
    toolsMenu->addAction(m_amkhlvRtFAction);
    toolsMenu->addAction(m_amkhlvLtFAction);
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_pdfView->action(PdfView::ShowForms));

    QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings", "Menu title"));
#ifndef QT_NO_SHORTCUT
	settingsMenu->addAction(ShortcutHandler::instance()->shortcutConfigAction());
	ShortcutHandler::instance()->shortcutConfigAction()->setIcon(Icon("configure-shortcuts"));
#endif // QT_NO_SHORTCUT
	settingsMenu->addAction(m_settingsTextAAAction);
	settingsMenu->addAction(m_settingsGfxAAAction);
	settingsMenu->addAction(m_renderBackendAction);
	settingsMenu->addAction(m_configureAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help", "Menu title"));
    helpMenu->addAction(QIcon(":/icons/pdfviewer-22.png"), tr("&About %1").arg(QCoreApplication::applicationName()), this, SLOT(slotAbout()));
    helpMenu->addAction(QIcon(":/icons/qt-logo-22.png"), tr("About &Qt"), this, SLOT(slotAboutQt()));
}

void PdfViewer::createToolBars()
{
	m_toolBar = addToolBar(tr("Main Tool Bar"));
	m_toolBar->setObjectName("MainToolBar");
	m_toolBar->addAction(m_fileOpenAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_goToPreviousPageAction);
	m_toolBar->addAction(m_goToPageAction);
	m_toolBar->addAction(m_goToNextPageAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_zoomInAction);
	m_toolBar->addAction(m_zoomAction);
	m_toolBar->addAction(m_zoomOutAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_mouseBrowseAction);
	m_toolBar->addAction(m_mouseMagnifyAction);
	m_toolBar->addAction(m_mouseSelectionAction);
	m_toolBar->addAction(m_mouseTextSelectionAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_pdfView->action(PdfView::ShowForms));
}

void PdfViewer::createToolBarsWhenNoMenuBar()
{
#ifndef QT_NO_SHORTCUT
	m_fileOpenRecentAction->setShortcut(QKeySequence::Open);
#endif // QT_NO_SHORTCUT
	connect(m_fileOpenRecentAction, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
	m_fileOpenRecentButton = new QToolButton(this);
	m_fileOpenRecentButton->setDefaultAction(m_fileOpenRecentAction);
	m_fileOpenRecentButton->setText(tr("&Open", "Action icon text: open file"));
	m_fileOpenRecentButton->setIcon(Icon("document-open"));
	m_fileOpenRecentButton->setPopupMode(QToolButton::MenuButtonPopup);
	m_fileOpenRecentButton->installEventFilter(this);

	m_fileSaveCopyAction->setIconText(tr("&Save a Copy", "Action icon text"));
	m_findAction->setIconText(tr("&Find", "Action icon text"));

	// Tools menu
	m_toolsButton = new QToolButton(this);
	QMenu *toolsMenu = new QMenu(m_toolsButton);
	m_toolsButton->setIcon(Icon("configure"));
	m_toolsButton->setText(tr("&Tools", "Menu title"));
	m_toolsButton->setMenu(toolsMenu);
	m_toolsButton->setPopupMode(QToolButton::InstantPopup);
	toolsMenu->addAction(m_findAction);
//	toolsMenu->addAction(m_findNextAction);
//	toolsMenu->addAction(m_findPreviousAction);
    toolsMenu->addAction(m_amkhlvDnAction);
    toolsMenu->addAction(m_amkhlvUpAction);
    toolsMenu->addAction(m_amkhlvDnFAction);
    toolsMenu->addAction(m_amkhlvUpFAction);
    toolsMenu->addAction(m_amkhlvRtAction);
    toolsMenu->addAction(m_amkhlvLtAction);
    toolsMenu->addAction(m_amkhlvRtFAction);
    toolsMenu->addAction(m_amkhlvLtFAction);
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_goToStartAction);
	toolsMenu->addAction(m_goToEndAction);
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_printAction);
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_showPresentationAction);
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_pdfView->action(PdfView::ShowForms));
	toolsMenu->addAction(m_pdfView->action(PdfView::Bookmarks));
	m_pdfView->action(PdfView::Bookmarks)->setText(tr("Boo&kmarks", "Menu title"));
	m_pdfView->action(PdfView::Bookmarks)->setIcon(Icon("bookmarks"));
	m_pdfView->action(PdfView::SetBookmark)->setIcon(Icon("bookmark-new"));
    m_pdfView->action(PdfView::UnSetBookmark)->setIcon(Icon("dialog-cancel"));
	m_pdfView->action(PdfView::PreviousBookmark)->setIcon(Icon("go-up"));
	m_pdfView->action(PdfView::NextBookmark)->setIcon(Icon("go-down"));

	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::SetBookmark));
    ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::UnSetBookmark));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::AmkhlvDn));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::AmkhlvUp));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::AmkhlvDnF));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::AmkhlvUpF));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::AmkhlvRt));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::AmkhlvLt));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::AmkhlvRtF));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::AmkhlvLtF));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::PreviousBookmark));
	ShortcutHandler::instance()->addAction(m_pdfView->action(PdfView::NextBookmark));
    m_viewMenu = new QMenu(tr("&View", "Menu title"), toolsMenu);
	toolsMenu->addMenu(m_viewMenu);
	QMenu *settingsMenu = new QMenu(tr("&Settings", "Menu title"), toolsMenu);
#ifndef QT_NO_SHORTCUT
	settingsMenu->addAction(ShortcutHandler::instance()->shortcutConfigAction());
	ShortcutHandler::instance()->shortcutConfigAction()->setIcon(Icon("configure-shortcuts"));
#endif // QT_NO_SHORTCUT
	settingsMenu->addAction(m_settingsTextAAAction);
	settingsMenu->addAction(m_settingsGfxAAAction);
	settingsMenu->addAction(m_renderBackendAction);
	settingsMenu->addAction(m_configureAction);
	toolsMenu->addMenu(settingsMenu);
	toolsMenu->addSeparator();
#ifndef QT_NO_SHORTCUT
	toolsMenu->addAction(QIcon(":/icons/pdfviewer-22.png"), tr("&About %1").arg(QCoreApplication::applicationName()), this, SLOT(slotAbout()));
    toolsMenu->addAction(QIcon(":/icons/qt-logo-22.png"), tr("About &Qt"), this, SLOT(slotAboutQt()));
#endif // QT_NO_SHORTCUT

	// Add everything to the toolbar
	m_toolBar = addToolBar(tr("Main Tool Bar"));
	m_toolBar->setObjectName("MainToolBar");
	m_toolBar->addWidget(m_fileOpenRecentButton);
	m_toolBar->addAction(m_fileSaveCopyAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_goToPreviousPageAction);
	m_toolBar->addAction(m_goToPageAction);
	m_toolBar->addAction(m_goToNextPageAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_zoomInAction);
	m_toolBar->addAction(m_zoomAction);
	m_toolBar->addAction(m_zoomOutAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_mouseBrowseAction);
	m_toolBar->addAction(m_mouseMagnifyAction);
	m_toolBar->addAction(m_mouseSelectionAction);
	m_toolBar->addAction(m_mouseTextSelectionAction);
	m_toolBar->addSeparator();
//	QLabel *stretch = new QLabel(m_toolBar);
//	stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
//	m_toolBar->addWidget(stretch);
	m_toolBar->addWidget(m_toolsButton);
	m_toolBar->installEventFilter(this);
	m_pdfView->addContextMenuAction(m_toolBar->toggleViewAction());
	connect(m_toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(slotSetToolBarToggleViewActionHidden(bool)));

	// Add orphan actions
	addAction(m_findPreviousAction);
	addAction(m_findNextAction);
	addAction(m_quitAction);
}

void PdfViewer::slotSetToolBarToggleViewActionHidden(bool hidden)
{
	if (hidden)
		m_pdfView->removeContextMenuAction(m_toolBar->toggleViewAction());
	else
		m_pdfView->addContextMenuAction(m_toolBar->toggleViewAction());
}

void PdfViewer::createDocks()
{
    InfoDock *infoDock = new InfoDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, infoDock);
    infoDock->hide();
    m_viewMenu->addAction(infoDock->toggleViewAction());
    m_observers.append(infoDock);
	m_dockWidgets.append(infoDock);

    TocDock *tocDock = new TocDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, tocDock);
    tocDock->hide();
    m_viewMenu->addAction(tocDock->toggleViewAction());
    m_observers.append(tocDock);
	m_dockWidgets.append(tocDock);

    FontsDock *fontsDock = new FontsDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, fontsDock);
    fontsDock->hide();
    m_viewMenu->addAction(fontsDock->toggleViewAction());
    m_observers.append(fontsDock);
	m_dockWidgets.append(fontsDock);

    PermissionsDock *permissionsDock = new PermissionsDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, permissionsDock);
    permissionsDock->hide();
    m_viewMenu->addAction(permissionsDock->toggleViewAction());
    m_observers.append(permissionsDock);
	m_dockWidgets.append(permissionsDock);

    ThumbnailsDock *thumbnailsDock = new ThumbnailsDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, thumbnailsDock);
    thumbnailsDock->hide();
    m_viewMenu->addAction(thumbnailsDock->toggleViewAction());
    m_observers.append(thumbnailsDock);
	m_dockWidgets.append(thumbnailsDock);

    EmbeddedFilesDock *embfilesDock = new EmbeddedFilesDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, embfilesDock);
    embfilesDock->hide();
    m_viewMenu->addAction(embfilesDock->toggleViewAction());
    m_observers.append(embfilesDock);
	m_dockWidgets.append(embfilesDock);

    MetadataDock *metadataDock = new MetadataDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, metadataDock);
    metadataDock->hide();
    m_viewMenu->addAction(metadataDock->toggleViewAction());
    m_observers.append(metadataDock);
	m_dockWidgets.append(metadataDock);

    OptContentDock *optContentDock = new OptContentDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, optContentDock);
    optContentDock->hide();
    m_viewMenu->addAction(optContentDock->toggleViewAction());
    m_observers.append(optContentDock);
	m_dockWidgets.append(optContentDock);
}

/*******************************************************************/

bool PdfViewer::eventFilter(QObject *obj, QEvent *event)
{
	// show menu instead of opening a file dialog when pressing the shortcut
	if (!m_showMenuBar && obj == m_fileOpenRecentButton)
	{
#ifndef QT_NO_SHORTCUT
		if (event->type() == QEvent::Shortcut)
		{
			m_fileOpenRecentButton->showMenu();
			return true;
		}
#endif // QT_NO_SHORTCUT
	}
	else if (obj == m_toolBar)
	{
#ifndef QT_NO_CONTEXTMENU
		if (event->type() == QEvent::ContextMenu)
		{
			showToolBarStylePopupMenu(static_cast<QContextMenuEvent*>(event)->pos());
			return true;
		}
#endif // QT_NO_CONTEXTMENU
	}
	return false;
}

/*******************************************************************/

void PdfViewer::loadDocument(const QString &file, PdfView::PositionHandling keepPosition)
{
//QTime t = QTime::currentTime();
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif // QT_NO_CURSOR

	// TODO: close only when the new file fails to load
	const QString tempFileName = file; // we must copy file in a new variable because otherwise closeDocument() empties file if file == m_file (which is the case in slotReload())
	closeDocument();

	bool isLoaded = m_pdfView->load(tempFileName);
	if (!isLoaded) {
		QPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Critical,
		    tr("Open Error"), tr("Cannot open:\n") + tempFileName,
		    QMessageBox::Ok, this);
		msgBox->exec();
		delete msgBox;
		m_fileOpenRecentAction->removeFile(tempFileName);
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
		return;
	}

	Poppler::Document *doc = m_pdfView->document();
	while (doc->isLocked()) {
		bool ok = true;
		QString password = QInputDialog::getText(this, tr("Document Password"),
		                                         tr("Please insert the password of the document:"),
		                                         QLineEdit::Password, QString(), &ok);
		if (!ok) {
			m_pdfView->close();
			m_fileOpenRecentAction->removeFile(tempFileName);
#ifndef QT_NO_CURSOR
			QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
			return;
		}
		doc->unlock(password.toLatin1(), password.toLatin1());
	}

	m_file = QFileInfo(tempFileName).absoluteFilePath();

	m_fileOpenRecentAction->addFile(m_file);
	// remove previous file from m_watcher and add new file
	if (m_watcher)
	{
		const QStringList files = m_watcher->files();
		if (!files.isEmpty())
			m_watcher->removePaths(files);
		m_watcher->addPath(m_file);
	}

//qCritical() << t.msecsTo(QTime::currentTime());
    Q_FOREACH(DocumentObserver *obs, m_observers) {
		if (keepPosition == PdfView::DontKeepPosition)
			obs->documentLoaded();
        obs->pageChanged(m_currentPage, keepPosition);
//qCritical() << t.msecsTo(QTime::currentTime());
    }

	// set window title
	const QString docTitle = doc->info("Title");
	setWindowTitle((docTitle.isEmpty() ? QFileInfo(m_file).fileName() : docTitle)
	    + " - " + QCoreApplication::applicationName());

	// enable actions
    m_fileSaveCopyAction->setEnabled(true);
	m_findAction->setEnabled(true);
	m_findNextAction->setEnabled(true);
	m_findPreviousAction->setEnabled(true);
	m_showPresentationAction->setEnabled(true);

#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
//qCritical() << "close and load document" << t.msecsTo(QTime::currentTime());
}

void PdfViewer::closeDocument()
{
    if (!m_pdfView->document()) {
        return;
    }

    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->documentClosed();
    }
	m_pdfView->close();

    m_currentPage = 0;
	m_file.clear();

//	setWindowTitle(QCoreApplication::applicationName()); // this causes the title to flicker when a document is reloaded

	// disable actions
    m_fileSaveCopyAction->setEnabled(false);
	m_findAction->setEnabled(false);
	m_findNextAction->setEnabled(false);
	m_findPreviousAction->setEnabled(false);
	m_showPresentationAction->setEnabled(false);
}

QString PdfViewer::currentDocument() const
{
	return m_file;
}

Poppler::Document *PdfViewer::document()
{
	return m_pdfView->document();
}

/*******************************************************************/

void PdfViewer::slotOpenFile()
{
#ifndef QT_NO_FILEDIALOG
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open PDF Document"), m_file, tr("PDF Documents (*.pdf)"));
    if (fileName.isEmpty()) {
        return;
    }

    loadDocument(fileName);
#endif // QT_NO_FILEDIALOG
}

void PdfViewer::slotLoadDocument(const QString &fileName)
{
	loadDocument(fileName);
}

void PdfViewer::slotSaveCopy()
{
#ifndef QT_NO_FILEDIALOG
    if (!m_pdfView->document()) {
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Copy"), m_file, tr("PDF Documents (*.pdf)"));
    if (fileName.isEmpty()) {
        return;
    }

    Poppler::PDFConverter *converter = m_pdfView->document()->pdfConverter();
    converter->setOutputFileName(fileName);
//    converter->setPDFOptions(converter->pdfOptions() & ~Poppler::PDFConverter::WithChanges);
    converter->setPDFOptions(converter->pdfOptions() | Poppler::PDFConverter::WithChanges);
    if (!converter->convert()) {
        QMessageBox msgbox(QMessageBox::Critical, tr("Save Error"), tr("Cannot export to:\n%1").arg(fileName),
                           QMessageBox::Ok, this);
    }
	else
		m_fileOpenRecentAction->addFile(fileName);
    delete converter;
#endif // QT_NO_FILEDIALOG
}

/*******************************************************************/

void PdfViewer::slotReloadWhenIdle(const QString &file)
{
	Q_UNUSED(file);
	if (m_reloadTimer)
		m_reloadTimer->stop();
	else
	{
		m_reloadTimer = new QTimer(this);
		m_reloadTimer->setSingleShot(true);
		m_reloadTimer->setInterval(1500);
		connect(m_reloadTimer, SIGNAL(timeout()), this, SLOT(slotReload()));
	}
	m_reloadTimer->start();
}

void PdfViewer::slotReload()
{
	loadDocument(m_file); // using DocumentObserver::KeepPosition as second argument doesn't work, but since closeDocument() saves the current position, this is not a problem
}

/*******************************************************************/

void PdfViewer::slotSearch(const QString &text, const PdfView::FindFlags &flags)
{
	m_findText = text;
	m_findFlags = flags;
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif // QT_NO_CURSOR
	m_pdfView->search(text, flags);
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
}

void PdfViewer::slotFind()
{
	if (!m_findWidget)
	{
		m_findWidget = new FindWidget(this);
		centralWidget()->layout()->addWidget(m_findWidget);
		connect(m_findWidget, SIGNAL(search(QString,PdfView::FindFlags)), this, SLOT(slotSearch(QString,PdfView::FindFlags)));
		connect(m_findWidget, SIGNAL(focusEditor()), m_pdfView, SLOT(setFocus()));
		connect(m_pdfView, SIGNAL(closeFindWidget()), m_findWidget, SLOT(hide()));
	}
	m_findWidget->setVisible(true);
	m_findWidget->setFocus();
}

void PdfViewer::slotFindNext()
{
	if (m_findText.isEmpty())
	{
		slotFind();
		m_findWidget->setForward(true);
	}
	else
		slotSearch(m_findText, m_findFlags & ~PdfView::FindBackward);
}

void PdfViewer::slotFindPrevious()
{
	if (m_findText.isEmpty())
	{
		slotFind();
		m_findWidget->setForward(false);
	}
	else
	slotSearch(m_findText, m_findFlags | PdfView::FindBackward);
}

/*******************************************************************/

void PdfViewer::slotDoAction(Poppler::LinkAction::ActionType actionType)
{
	switch (actionType)
	{
		case Poppler::LinkAction::PageFirst: setPage(0); break;
		case Poppler::LinkAction::PagePrev: slotGoToPreviousPage(); break;
		case Poppler::LinkAction::PageNext: slotGoToNextPage(); break;
		case Poppler::LinkAction::PageLast: setPage(m_pdfView->document()->numPages()-1); break;
		case Poppler::LinkAction::HistoryBack: qCritical("TODO history back"); break;
		case Poppler::LinkAction::HistoryForward: qCritical("TODO history forward"); break;
		case Poppler::LinkAction::Quit: qApp->closeAllWindows(); break;
		case Poppler::LinkAction::Presentation: slotShowPresentation(); break;
		case Poppler::LinkAction::EndPresentation: break; // do nothing in normal mode
		case Poppler::LinkAction::Find: slotFind(); break;
		case Poppler::LinkAction::GoToPage: m_pdfView->slotGoToPage(); break;
		case Poppler::LinkAction::Close: closeDocument(); break;
		case Poppler::LinkAction::Print: qCritical("TODO print"); break;
	}
}

void PdfViewer::slotShowPresentation()
{
//	PresentationWidget *presentationWidget = new PresentationWidget;
//	m_presentationWidget->setPopplerPages(m_view->popplerPages());
//	m_presentationWidget->setPopplerPageLabels(m_view->popplerPageLabels());
	m_presentationWidget->setPopplerPages(m_pdfView->popplerPages());
	m_presentationWidget->setPopplerPageLabels(m_pdfView->popplerPageLabels());
	m_presentationWidget->setPage(m_currentPage);
	m_presentationWidget->show();
//	connect(presentationWidget, SIGNAL(pageChanged(int)), this, SLOT(slotGoToPage(int)));
//	connect(presentationWidget, SIGNAL(doAction(Poppler::LinkAction::ActionType)), this, SLOT(slotDoAction(Poppler::LinkAction::ActionType)));
	// each PresentationWidget has the attribute Qt::WA_DeleteOnClose, so don't bother deleting it here
}

/*******************************************************************/

void PdfViewer::slotGoToPreviousPage()
{
	if (m_currentPage > 0)
		setPage(m_currentPage - 1);
}

void PdfViewer::slotGoToNextPage()
{
	if (m_currentPage < m_pdfView->document()->numPages() - 1)
		setPage(m_currentPage + 1);
}

void PdfViewer::slotGoToPage(int pageNumber)
{
	if (pageNumber >= 0 && pageNumber < m_pdfView->document()->numPages())
		setPage(pageNumber);
}

void PdfViewer::slotGoToPage(double pageNumber)
{
	if (pageNumber >= 0 && pageNumber < m_pdfView->document()->numPages())
		setPage(pageNumber);
}

void PdfViewer::slotViewScrollPositionChanged(qreal fraction, int pageNumber)
{
	Q_UNUSED(fraction);
	Q_UNUSED(pageNumber);
}

/*******************************************************************/

void PdfViewer::slotAbout()
{
	QPointer<AboutDialog> aboutDialog = new AboutDialog(this);
	aboutDialog->exec();
	aboutDialog->deleteLater();
}

void PdfViewer::slotAboutQt()
{
    QMessageBox::aboutQt(this);
}

/*******************************************************************/

void PdfViewer::slotToggleTextAA(bool value)
{
	m_pdfView->setRenderHint(Poppler::Document::TextAntialiasing, value);
}

void PdfViewer::slotToggleGfxAA(bool value)
{
	m_pdfView->setRenderHint(Poppler::Document::Antialiasing, value);
}

void PdfViewer::slotRenderBackend(QAction *act)
{
	m_pdfView->setRenderBackend(Poppler::Document::RenderBackend(act->data().toInt()));
}

/*******************************************************************/

void PdfViewer::showToolBarStylePopupMenu(const QPoint &pos)
{
	QMenu *popupMenu = new QMenu(this);
	popupMenu->setAttribute(Qt::WA_DeleteOnClose);
	popupMenu->move(mapToGlobal(QPoint(0,0)) + pos);

	QWidgetAction *iconOnlyAction = new QWidgetAction(popupMenu);
	QRadioButton *iconOnlyRadio = new QRadioButton(tr("&Icons Only"), popupMenu);
	iconOnlyAction->setDefaultWidget(iconOnlyRadio);

	QWidgetAction *textOnlyAction = new QWidgetAction(popupMenu);
	QRadioButton *textOnlyRadio = new QRadioButton(tr("&Text Only"), popupMenu);
	textOnlyAction->setDefaultWidget(textOnlyRadio);

	QWidgetAction *textBesideIconAction = new QWidgetAction(popupMenu);
	QRadioButton *textBesideIconRadio = new QRadioButton(tr("Text &Alongside Icons"), popupMenu);
	textBesideIconAction->setDefaultWidget(textBesideIconRadio);

	QWidgetAction *textUnderIconAction = new QWidgetAction(popupMenu);
	QRadioButton *textUnderIconRadio = new QRadioButton(tr("Text &Under Icons"), popupMenu);
	textUnderIconAction->setDefaultWidget(textUnderIconRadio);

	QButtonGroup *popupButtonGroup = new QButtonGroup(popupMenu);
	popupButtonGroup->addButton(iconOnlyRadio);
	popupButtonGroup->addButton(textOnlyRadio);
	popupButtonGroup->addButton(textBesideIconRadio);
	popupButtonGroup->addButton(textUnderIconRadio);
	popupButtonGroup->setId(iconOnlyRadio, 0);
	popupButtonGroup->setId(textOnlyRadio, 1);
	popupButtonGroup->setId(textBesideIconRadio, 2);
	popupButtonGroup->setId(textUnderIconRadio, 3);
	connect(popupButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotChangeToolBarStyle(int)));

	popupMenu->addAction(iconOnlyAction);
	popupMenu->addAction(textOnlyAction);
	popupMenu->addAction(textBesideIconAction);
	popupMenu->addAction(textUnderIconAction);
	popupMenu->setContentsMargins(5, 0, 5, 0);

	QSettings settings;
	settings.beginGroup("MainWindow");
	const int toolBarStyleNumber = settings.value("ToolBarStyle", 0).toInt();
	switch (toolBarStyleNumber)
	{
		case 0: iconOnlyRadio->setChecked(true); break;
		case 1: textOnlyRadio->setChecked(true); break;
		case 2: textBesideIconRadio->setChecked(true); break;
		case 3: textUnderIconRadio->setChecked(true); break;
	}
	settings.endGroup();

	popupMenu->show();
	// make sure that the popupMenu stays completely inside the screen (must be done after popupMenu->show() in order to have the correct width)
	const int desktopWidth = QApplication::desktop()->availableGeometry(this).width();
	if (popupMenu->x() + popupMenu->width() > desktopWidth)
		popupMenu->move(desktopWidth - popupMenu->width(), popupMenu->y());
}

void PdfViewer::slotChangeToolBarStyle(int which)
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("ToolBarStyle", which);
	settings.endGroup();

	setToolBarStyle();
}

/*******************************************************************/

void PdfViewer::setToolBarStyle()
{
	QSettings settings;
	settings.beginGroup("MainWindow");

	const int toolBarStyleNumber = settings.value("ToolBarStyle", 0).toInt();
	Qt::ToolButtonStyle toolBarStyle = Qt::ToolButtonIconOnly;
	switch (toolBarStyleNumber)
	{
		case 0: toolBarStyle = Qt::ToolButtonIconOnly; break;
		case 1: toolBarStyle = Qt::ToolButtonTextOnly; break;
		case 2: toolBarStyle = Qt::ToolButtonTextBesideIcon; break;
		case 3: toolBarStyle = Qt::ToolButtonTextUnderIcon; break;
	}
	setToolButtonStyle(toolBarStyle);
	if (!m_showMenuBar)
	{
		m_fileOpenRecentButton->setToolButtonStyle(toolBarStyle);
		m_toolsButton->setToolButtonStyle(toolBarStyle);
		if (toolBarStyle == Qt::ToolButtonTextBesideIcon)
		{
			qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_goToPreviousPageAction))->setToolButtonStyle(Qt::ToolButtonIconOnly);
			qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_goToNextPageAction))->setToolButtonStyle(Qt::ToolButtonIconOnly);
			qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_zoomInAction))->setToolButtonStyle(Qt::ToolButtonIconOnly);
			qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_zoomOutAction))->setToolButtonStyle(Qt::ToolButtonIconOnly);
			qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_mouseBrowseAction))->setToolButtonStyle(Qt::ToolButtonIconOnly);
			qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_mouseMagnifyAction))->setToolButtonStyle(Qt::ToolButtonIconOnly);
			qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_mouseSelectionAction))->setToolButtonStyle(Qt::ToolButtonIconOnly);
			qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_mouseTextSelectionAction))->setToolButtonStyle(Qt::ToolButtonIconOnly);
		}
	}

	settings.endGroup();
}

void PdfViewer::readSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	const QSize size = settings.value("Size", sizeHint()).toSize();
	resize(size);
	restoreState(settings.value("MainWindowState").toByteArray());
	const int mouseToolNumber = settings.value("MouseTool", 1).toInt();
	switch (mouseToolNumber)
	{
		case 0: m_mouseBrowseAction->setChecked(true); break;
		case 1: m_mouseMagnifyAction->setChecked(true); break;
		case 2: m_mouseSelectionAction->setChecked(true); break;
		case 3: m_mouseTextSelectionAction->setChecked(true); break;
	}
	selectMouseTool(mouseToolNumber); // for some strange reason the above actions don't trigger their signal, so we must explicitly select the mouse tool of the view here
	settings.endGroup();

	settings.beginGroup("Document");
	m_pdfView->setZoomFactor(settings.value("ZoomFactor", 1).toDouble());
	settings.endGroup();

	const int maxSize = settings.value("FileInfoCacheMaxSize", QString(PDFVIEWER_FILE_INFO_CACHE_MAX_SIZE).toDouble()).toInt();
	m_pdfView->setMaximumFileSettingsCacheSize(maxSize);

	m_pdfView->setRenderHint(Poppler::Document::TextAntialiasing, m_settingsTextAAAction->isChecked());
	m_pdfView->setRenderHint(Poppler::Document::Antialiasing, m_settingsGfxAAAction->isChecked());
	m_pdfView->setRenderBackend(Poppler::Document::RenderBackend(m_settingsRenderBackendGrp->checkedAction()->data().toInt()));

	setToolBarStyle();

	m_fileOpenRecentAction->loadEntries();

#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->setExclusivityGroup(QStringList() << tr("Presentation"));
	ShortcutHandler::instance()->readSettings();
#endif // QT_NO_SHORTCUT
}

void PdfViewer::writeSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("Size", size());
	settings.setValue("MainWindowState", QMainWindow::saveState());
	settings.endGroup();

	settings.beginGroup("Document");
	settings.setValue("ZoomFactor", m_pdfView->zoomFactor());
	settings.endGroup();

	m_fileOpenRecentAction->saveEntries();
}

void PdfViewer::slotConfigure()
{
	QPointer<ConfigDialog> configDialog = new ConfigDialog(this);
	configDialog->exec();
	delete configDialog;
}

/*******************************************************************/

void PdfViewer::setPage(double page, PdfView::PositionHandling keepPosition)
{
    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->pageChanged(page, keepPosition);
    }
	m_pdfView->setPage(page, keepPosition);

    m_currentPage = page;
}

int PdfViewer::page() const
{
    return m_currentPage;
}

/*******************************************************************/
// Mouse tool

void PdfViewer::selectMouseTool(int which)
{
	switch (which)
	{
		case 0: m_pdfView->setMouseTool(PdfView::Browsing); break;
		case 1: m_pdfView->setMouseTool(PdfView::Magnifying); break;
		case 2: m_pdfView->setMouseTool(PdfView::Selection); break;
		case 3: m_pdfView->setMouseTool(PdfView::TextSelection); break;
	}
}

void PdfViewer::slotSelectMouseTool(PdfView::MouseTool tool)
{
	int which;
	if (tool == PdfView::Browsing)
		which = 0;
	else if (tool == PdfView::Magnifying)
		which = 1;
	else if (tool == PdfView::Selection)
		which = 2;
	else if (tool == PdfView::TextSelection)
		which = 3;
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("MouseTool", which);
	settings.endGroup();
}

/*******************************************************************/
// Synctex

void PdfViewer::slotOpenTexDocument(const QString &fileName, int pageNumber)
{
	QSettings settings;
	QString editorCommand = settings.value("Editor", PDFVIEWER_TEX_EDITOR).toString();
	editorCommand.replace(QLatin1String("%l"), QString::number(pageNumber));
	editorCommand.replace(QLatin1String("%f"), QLatin1Char('\"') + fileName + QLatin1Char('\"'));
	QProcess::startDetached(editorCommand);
}

void PdfViewer::syncFromSource(const QString &sourceFile, int lineNumber)
{
	m_pdfView->syncFromSource(sourceFile, lineNumber);
}
