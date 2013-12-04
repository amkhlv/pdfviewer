/*
 * Copyright (C) 2008, Pino Toscano <pino@kde.org>
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

#ifndef PDFVIEWER_PDFVIEWER_H
#define PDFVIEWER_PDFVIEWER_H

#include "documentobserver.h"
#include <QtGui/QMainWindow>
#include <poppler-qt4.h>
#include <pdfview.h>

class QAbstractButton;
class QAction;
class QActionGroup;
class QFileSystemWatcher;
class QLabel;
class QToolBar;
class QToolButton;
class DocumentObserver;
class FindWidget;
class PageView;
class PresentationWidget;
class RecentFilesAction;
class SelectPageAction;

class PdfViewer : public QMainWindow
{
    Q_OBJECT

    friend class DocumentObserver;

public:
    PdfViewer();
    ~PdfViewer();

    /*virtual*/ QSize sizeHint() const;

    void loadDocument(const QString &file, PdfView::PositionHandling keepPosition = PdfView::KeepPosition);
    void closeDocument();
	Poppler::Document *document();

public Q_SLOTS:
	QString currentDocument() const;
	void syncFromSource(const QString &sourceFile, int lineNumber);

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
	void slotSetToolBarToggleViewActionHidden(bool hidden);
    void slotOpenFile();
	void slotLoadDocument(const QString &fileName);
    void slotSaveCopy();
	void slotReloadWhenIdle(const QString &file);
	void slotReload();
	void slotSearch(const QString &text, const PdfView::FindFlags &flags);
	void slotFind();
	void slotFindNext();
	void slotFindPrevious();
	void slotDoAction(Poppler::LinkAction::ActionType actionType);
	void slotShowPresentation();
	void slotGoToPreviousPage();
	void slotGoToNextPage();
	void slotGoToPage(int pageNumber);
	void slotGoToPage(double pageNumber);
	void slotViewScrollPositionChanged(qreal fraction, int pageNumber);
    void slotAbout();
    void slotAboutQt();
    void slotToggleTextAA(bool value);
    void slotToggleGfxAA(bool value);
    void slotRenderBackend(QAction *act);
	void slotChangeToolBarStyle(int which);
	void slotConfigure();
	void slotSelectMouseTool(PdfView::MouseTool tool);
	void slotOpenTexDocument(const QString &fileName, int pageNumber);

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createToolBarsWhenNoMenuBar();
	void createDocks();
	void showToolBarStylePopupMenu(const QPoint &pos);
	void setToolBarStyle();
	void readSettings();
	void writeSettings();
    void setPage(double page, PdfView::PositionHandling keepPosition = PdfView::KeepPosition);
    int page() const;
	void selectMouseTool(int which);

    int m_currentPage;

	QMenu *m_viewMenu;
	QToolBar *m_toolBar;
    QAction *m_fileOpenAction;
    QAction *m_fileSaveCopyAction;
	QAction *m_printAction;
	QAction *m_quitAction;
	RecentFilesAction *m_fileOpenRecentAction;
	QAction *m_findAction;
	QAction *m_findNextAction;
	QAction *m_findPreviousAction;
	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	QAction *m_zoomAction;
	QAction *m_showPresentationAction;
	QAction *m_goToStartAction;
	QAction *m_goToEndAction;
	QAction *m_goToPreviousPageAction;
	QAction *m_goToNextPageAction;
	QAction *m_goToPageAction;
	QAction *m_mouseBrowseAction;
	QAction *m_mouseMagnifyAction;
	QAction *m_mouseSelectionAction;
	QAction *m_mouseTextSelectionAction;
    QAction *m_settingsTextAAAction;
    QAction *m_settingsGfxAAAction;
	QAction *m_renderBackendAction;
	QAction *m_configureAction;
    QAction *m_amkhlvDnAction;
    QAction *m_amkhlvUpAction;
    QAction *m_amkhlvDnFAction;
    QAction *m_amkhlvUpFAction;
    QAction *m_amkhlvRtAction;
    QAction *m_amkhlvLtAction;
    QAction *m_amkhlvRtFAction;
    QAction *m_amkhlvLtFAction;
    QAction *m_ReturnBackAction;
    QActionGroup *m_settingsRenderBackendGrp;

	QToolButton *m_fileOpenRecentButton;
	QToolButton *m_toolsButton;
	bool m_showMenuBar;

    QList<DocumentObserver *> m_observers;
	QList<QDockWidget*> m_dockWidgets;
	PdfView *m_pdfView;

    Poppler::Document *m_doc;
	QString m_file;
	QFileSystemWatcher *m_watcher;
	QTimer *m_reloadTimer;

	FindWidget *m_findWidget;
	QString m_findText;
	PdfView::FindFlags m_findFlags;

	PresentationWidget *m_presentationWidget;
};

#endif // PDFVIEWER_PDFVIEWER_H
