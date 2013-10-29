/***************************************************************************
 *   Copyright (C) 2012 by Glad Deschrijver                                *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include <QDebug>
#include "presentationwidget.h"

#include "utils/icon.h"
#include "label.h"
#include "utils/gotopageaction.h"
#include "shortcuthandler/shortcuthandler.h"

#include <QtCore/QTimer>
#include <QtCore/qmath.h>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QToolBar>
#ifndef QT_NO_WHEELEVENT
#include <QtGui/QWheelEvent>
#else
#include <QtGui/QMouseEvent>
#endif // QT_NO_WHEELEVENT

#include <poppler-qt4.h>

PresentationWidget::PresentationWidget(QWidget *parent)
	: QWidget(0, Qt::FramelessWindowHint) // parent must be 0, to have an independent widget
	, m_pageNumber(-1)
	, m_dpiX(QApplication::desktop()->physicalDpiX())
	, m_dpiY(QApplication::desktop()->physicalDpiY())
	, m_magnifiedImageLabel(0)
	, m_pageChangeTimer(0)
	, m_pageLoadTimer(0)
	, m_preloadingPageNumber(-1)
	, m_cachedCount(0)
{
	Q_UNUSED(parent);
//	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName(QLatin1String("PresentationWidget"));
	setWindowState(windowState() | Qt::WindowFullScreen); // set full screen

	QPalette backgroundPalette = palette();
	backgroundPalette.setColor(QPalette::Window, QColor(0,0,0));
	setPalette(backgroundPalette);

	m_imageLabel = new Label(this);
	connect(m_imageLabel, SIGNAL(scroll(int)), this, SLOT(slotScrollToNextOrPreviousPage(int)));
	connect(m_imageLabel, SIGNAL(magnify(QPoint)), this, SLOT(slotMagnify(QPoint)));
	connect(m_imageLabel, SIGNAL(endMagnify()), this, SLOT(slotMagnifyEnded()));
	connect(m_imageLabel, SIGNAL(goToPage(double)), this, SLOT(slotSetPage(double)));
	connect(m_imageLabel, SIGNAL(doAction(Poppler::LinkAction::ActionType)), this, SLOT(slotDoAction(Poppler::LinkAction::ActionType)));

//	show();
	m_imageLabel->show();

	initTopBar(); // must be initialized after m_imageLabel in order to be on top
	setMouseTracking(true);
}

PresentationWidget::~PresentationWidget()
{
	m_cachedImages.clear();
}

void PresentationWidget::setPopplerPages(const QList<Poppler::Page*> &popplerPages)
{
	m_popplerPages = popplerPages;
	const int pageCount = popplerPages.size();
	m_goToPageAction->setMinimum(1);
	m_goToPageAction->setMaximum(popplerPages.size());
	// do not load any page here, because this crashes the app sometimes because at this point m_popplerPageLabels is not yet set (I wonder why not always); the correct page is loaded later anyway in viewer.cpp
	m_cachedCount = 0;
	m_cachedImages.clear();
	m_cachedImages.reserve(pageCount);
	for (int i = 0; i < pageCount; ++i)
		m_cachedImages.append(QImage());
}

void PresentationWidget::setPopplerPageLabels(const QStringList &popplerPageLabels)
{
	m_popplerPageLabels = popplerPageLabels;
}

/*******************************************************************/

void PresentationWidget::initTopBar()
{
	m_topBar = new QToolBar(this);
	m_topBar->setObjectName(QLatin1String("PresentationToolBar"));
	m_topBar->setIconSize(QSize(32, 32));
	m_topBar->setMovable(false);
	m_topBar->layout()->setMargin(0);

	// add actions
	m_goToPreviousPageAction = new QAction(Icon("go-previous"), tr("&Previous Page", "Action"), this);
	m_goToPreviousPageAction->setIconText(tr("Previous", "Action icon text: go to previous page"));
	m_goToPreviousPageAction->setObjectName("presentation_go_previous_page");
	connect(m_goToPreviousPageAction, SIGNAL(triggered()), this, SLOT(slotGoToPreviousPage()));
	m_topBar->addAction(m_goToPreviousPageAction);
#ifndef QT_NO_SHORTCUT
//	ShortcutHandler::instance()->addAction(m_goToPreviousPageAction, tr("Presentation"));
#endif // QT_NO_SHORTCUT

	m_goToPageAction = new GoToPageAction(this);
	connect(m_goToPageAction, SIGNAL(valueChanged(int)), this, SLOT(slotSetPage(int)));
	m_topBar->addAction(m_goToPageAction);

	QAction *openPageSelectorAction = new QAction(Icon("go-jump"), tr("&Go to Page"), this);
#ifndef QT_NO_SHORTCUT
	openPageSelectorAction->setShortcut(tr("Ctrl+G"));
#endif // QT_NO_SHORTCUT
	openPageSelectorAction->setObjectName("presentation_go_to_page");
	connect(openPageSelectorAction, SIGNAL(triggered()), this, SLOT(slotOpenPageSelector()));
	addAction(openPageSelectorAction);
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(openPageSelectorAction, tr("Presentation"));
#endif // QT_NO_SHORTCUT

	m_goToNextPageAction = new QAction(Icon("go-next"), tr("&Next Page", "Action"), this);
	m_goToNextPageAction->setIconText(tr("Next", "Action icon text: go to next page"));
	m_goToNextPageAction->setObjectName("presentation_go_next_page");
	connect(m_goToNextPageAction, SIGNAL(triggered()), this, SLOT(slotGoToNextPage()));
	m_topBar->addAction(m_goToNextPageAction);
#ifndef QT_NO_SHORTCUT
//	ShortcutHandler::instance()->addAction(m_goToNextPageAction, tr("Presentation"));
#endif // QT_NO_SHORTCUT

	QWidget *spacer = new QWidget(m_topBar);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
	m_topBar->addWidget(spacer);

	QAction *closeAction = new QAction(Icon("application-exit"), tr("Exit Presentation Mode", "Action"), this);
	closeAction->setIconText(tr("Exit", "Action: exit presentation mode"));
	connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
	m_topBar->addAction(closeAction);

	QAction *quitAction = new QAction(Icon("application-exit"), tr("&Quit"), this);
#ifndef QT_NO_SHORTCUT
	quitAction->setShortcut(tr("Ctrl+Q"));
#endif // QT_NO_SHORTCUT
	quitAction->setObjectName("presentation_application_exit");
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
	addAction(quitAction);
#ifndef QT_NO_SHORTCUT
	ShortcutHandler::instance()->addAction(quitAction, tr("Presentation"));
#endif // QT_NO_SHORTCUT

	m_topBar->setAutoFillBackground(true);
	m_topBar->setGeometry(0, 0, QApplication::desktop()->screenGeometry().width(), 32 + 10); // icon size + 10
	showTopBar(false);

	// change topBar background color
	QPalette topPalette = m_topBar->palette();
	topPalette.setColor(QPalette::Active, QPalette::Button, Qt::gray);
	topPalette.setColor(QPalette::Active, QPalette::Background, Qt::darkGray);
	m_topBar->setPalette(topPalette);
}

void PresentationWidget::showTopBar(bool visible)
{
	if (visible)
		m_topBar->show();
	else
	{
		m_topBar->hide();
		m_imageLabel->setFocus(); // remove focus from m_goToPageAction if the focus is there
	}
}

/*******************************************************************/

void PresentationWidget::delayedPageChange()
{
	// Changing a page in the main window is slow, so we use a timer to
	// ensure that this only occurs when the user stops browsing the pages.
	// Indeed, each start cancels the previous one, this means that timeout()
	// is only fired when the user has not changed the page during 100msecs.
	// This ensures that the page in the main window is not changed on every
	// page change in presentation mode.
	if (m_pageChangeTimer)
		m_pageChangeTimer->stop();
	else
	{
		m_pageChangeTimer = new QTimer(this);
		m_pageChangeTimer->setSingleShot(true);
		m_pageChangeTimer->setInterval(100);
		connect(m_pageChangeTimer, SIGNAL(timeout()), this, SLOT(slotDelayedPageChange()));
	}
	m_pageChangeTimer->start();
}

void PresentationWidget::slotDelayedPageChange()
{
	emit pageChanged(m_pageNumber);
}

void PresentationWidget::delayedPageLoad()
{
	// Loading a page full screen in presentation mode is slow, so we use
	// a timer to ensure that this only occurs when the user stops browsing
	// the pages. See comment in delayedPageChange() for why this works.
	if (m_pageLoadTimer)
		m_pageLoadTimer->stop();
	else
	{
		m_pageLoadTimer = new QTimer(this);
		m_pageLoadTimer->setSingleShot(true);
		m_pageLoadTimer->setInterval(50);
		connect(m_pageLoadTimer, SIGNAL(timeout()), this, SLOT(slotDelayedPageLoad()));
	}
	m_pageLoadTimer->start();
}

void PresentationWidget::slotDelayedPageLoad()
{
	if (!isVisible()) // stop loading all pages in the background if the presentation widget is hidden again
		return;

	// render page to image
	const int screenWidth = QApplication::desktop()->screenGeometry().width();
	const int screenHeight = QApplication::desktop()->screenGeometry().height();
	Poppler::Page *popplerPage = m_popplerPages.at(m_preloadingPageNumber);
	const qreal zoomFactor = qMin(screenWidth / popplerPage->pageSizeF().width(), screenHeight / popplerPage->pageSizeF().height()) * 72.0;
	const QImage image = popplerPage->renderToImage(zoomFactor, zoomFactor);

	// add image to cache
	if (m_cachedImages.at(m_preloadingPageNumber).isNull()) // make sure cachedCount is updated only when there is no cached image
	{
		m_cachedImages[m_preloadingPageNumber] = image;
		++m_cachedCount;
	}

	// actually display the page
	if (m_preloadingPageNumber == m_pageNumber)
		showPage(popplerPage, image);

	// Fill cache while user is not browsing pages.
	// Because this function is only called when m_pageLoadTimer times out,
	// and each start of the timer cancels out the previous one, we are
	// ensured that the code below only runs when the user is not browsing
	// the pages.
	if (m_cachedCount < m_popplerPages.size())
	{
		while (m_preloadingPageNumber < m_popplerPages.size() - 1 && !m_cachedImages.at(++m_preloadingPageNumber).isNull());
		if (m_preloadingPageNumber == m_popplerPages.size() - 1 && !m_cachedImages.at(m_preloadingPageNumber).isNull())
			m_preloadingPageNumber = 0;
		delayedPageLoad();
	}
}

void PresentationWidget::showPage(Poppler::Page *popplerPage, const QImage &image)
{
	const int screenWidth = QApplication::desktop()->screenGeometry().width();
	const int screenHeight = QApplication::desktop()->screenGeometry().height();

	if (!image.isNull())
	{
		m_imageLabel->resize(image.size());
		m_imageLabel->setPixmap(QPixmap::fromImage(image));
	}
	m_imageLabel->generateLinks(popplerPage, m_popplerPageLabels);

	// center m_imageLabel on screen
	if (m_imageLabel->width() < screenWidth)
		m_imageLabel->move((screenWidth - m_imageLabel->width()) / 2, 0);
	else
		m_imageLabel->move(0, (screenHeight - m_imageLabel->height()) / 2);

	// enable/disable actions
	m_goToPreviousPageAction->setEnabled(m_pageNumber > 0);
	m_goToNextPageAction->setEnabled(m_pageNumber < m_popplerPages.size() - 1);
	disconnect(m_goToPageAction, SIGNAL(valueChanged(int)), this, SLOT(slotSetPage(int)));
	m_goToPageAction->setValue(m_pageNumber);
	connect(m_goToPageAction, SIGNAL(valueChanged(int)), this, SLOT(slotSetPage(int)));

	// update page number in the main window
	delayedPageChange();
}

void PresentationWidget::loadPage(int pageNumber)
{
	// This is where loading the page starts. First we try to load the page
	// from the cache if it is already there. If not then a smaller version
	// is loaded (this is faster) and scaled to fill the screen (the scaled
	// version is however blurred). Finally we load the real-sized version
	// of the page. Since this is slow, we only do this when the user stops
	// browsing the pages (we therefore use a timer in delayedPageLoad()).
	// The real-sized version of the page is then added to the cache. In
	// order to avoid that the user always sees a blurred version of the
	// page before the real-sized version is loaded, we fill the cache with
	// the other pages when the user is not browsing the pages.
	// We do not load the real-sized version of the pages in a separate
	// thread because poppler 0.16 crashes when two pages are rendered
	// simultaneously using renderToImage(), so we cannot render the blurred
	// version while in the background the real-sized version is loading.
	m_pageNumber = pageNumber;
	m_preloadingPageNumber = m_pageNumber;

	// first try to load the page from cache if it is already there
	if (!m_cachedImages.at(m_pageNumber).isNull())
	{
		showPage(m_popplerPages.at(m_pageNumber), m_cachedImages.at(m_pageNumber));
		return;
	}

	const int screenWidth = QApplication::desktop()->screenGeometry().width();
	const int screenHeight = QApplication::desktop()->screenGeometry().height();
	Poppler::Page *popplerPage = m_popplerPages.at(m_pageNumber);
	const qreal zoomFactor = qMin(screenWidth / popplerPage->pageSizeF().width(), screenHeight / popplerPage->pageSizeF().height()) * 72.0;

	// first show a rescaled version of a smaller rendering of the page (this is faster than the real thing)
	QImage image = popplerPage->renderToImage(zoomFactor / 2, zoomFactor / 2);
	image = image.scaled(screenWidth, screenHeight, Qt::KeepAspectRatio);

	if (!image.isNull())
	{
		m_imageLabel->resize(image.size());
		m_imageLabel->setPixmap(QPixmap::fromImage(image));
	}
	m_imageLabel->generateLinks(popplerPage, m_popplerPageLabels);

	// center m_imageLabel on screen
	if (m_imageLabel->width() < screenWidth)
		m_imageLabel->move((screenWidth - m_imageLabel->width()) / 2, 0);
	else
		m_imageLabel->move(0, (screenHeight - m_imageLabel->height()) / 2);

	// load the real-sized version of the page
	delayedPageLoad();
}

void PresentationWidget::setPage(int pageNumber)
{
	loadPage(pageNumber);
}

void PresentationWidget::slotGoToPreviousPage()
{
	if (m_pageNumber > 0)
		loadPage(m_pageNumber - 1);
}

void PresentationWidget::slotGoToNextPage()
{
	if (m_pageNumber < m_popplerPages.size() - 1)
		loadPage(m_pageNumber + 1);
}

void PresentationWidget::slotGoToFirstPage()
{
	loadPage(0);
}

void PresentationWidget::slotGoToLastPage()
{
	loadPage(m_popplerPages.size() - 1);
}

void PresentationWidget::slotScrollToNextOrPreviousPage(int delta)
{
	if (delta > 0)
		slotGoToPreviousPage();
	else
		slotGoToNextPage();
}

void PresentationWidget::slotSetPage(int pageNumber)
{
	loadPage(pageNumber);
}

void PresentationWidget::slotSetPage(double pageNumber)
{
	loadPage(pageNumber);
}

void PresentationWidget::slotOpenPageSelector()
{
	showTopBar(true);
	m_goToPageAction->setFocus();
}

/*******************************************************************/

void PresentationWidget::slotDoAction(Poppler::LinkAction::ActionType actionType)
{
	switch (actionType)
	{
		case Poppler::LinkAction::PageFirst: slotGoToFirstPage(); break;
		case Poppler::LinkAction::PagePrev: slotGoToPreviousPage(); break;
		case Poppler::LinkAction::PageNext: slotGoToNextPage(); break;
		case Poppler::LinkAction::PageLast: slotGoToLastPage(); break;
		case Poppler::LinkAction::HistoryBack: qCritical("TODO history back"); break;
		case Poppler::LinkAction::HistoryForward: qCritical("TODO history forward"); break;
		case Poppler::LinkAction::Presentation: break; // do nothing as we are already in presentation mode
		case Poppler::LinkAction::EndPresentation: close(); break;
		case Poppler::LinkAction::Find: qCritical("TODO find in presentation mode"); break;
		case Poppler::LinkAction::GoToPage: slotOpenPageSelector(); break;
		default: emit doAction(actionType); break; // handle in viewer.cpp
	}
}

/*******************************************************************/

void PresentationWidget::slotMagnify(const QPoint &pos)
{
	// create magnification image
	const qreal magnifyZoom = 2;
	const int magnifyWidth = 400;
	const int magnifyHeight = 300;
	const int screenWidth = QApplication::desktop()->screenGeometry().width();
	const int screenHeight = QApplication::desktop()->screenGeometry().height();
	Poppler::Page *popplerPage = m_popplerPages.at(m_pageNumber);
	const qreal zoomFactor = qMin(screenWidth / popplerPage->pageSizeF().width(), screenHeight / popplerPage->pageSizeF().height()) * 72.0;
	const QImage image = popplerPage->renderToImage(zoomFactor*magnifyZoom, zoomFactor*magnifyZoom, magnifyZoom*(pos.x()-magnifyWidth/2), magnifyZoom*(pos.y()-magnifyHeight/2), magnifyZoom*magnifyWidth, magnifyZoom*magnifyHeight);

	// create magnification window
	if (!m_magnifiedImageLabel)
	{
		m_magnifiedImageLabel = new Label(this);
		if (!image.isNull())
			m_magnifiedImageLabel->resize(image.size());
	}
	if (!image.isNull())
		m_magnifiedImageLabel->setPixmap(QPixmap::fromImage(image));

	// move magnification window to the correct place
	const int xRef = (screenWidth - m_imageLabel->width()) / 2;
	const int yRef = (screenHeight - m_imageLabel->height()) / 2;
	m_magnifiedImageLabel->move(xRef + pos.x() - magnifyZoom * magnifyWidth/2, yRef + pos.y() - magnifyZoom * magnifyHeight/2);

	m_magnifiedImageLabel->show();
}

void PresentationWidget::slotMagnifyEnded()
{
	delete m_magnifiedImageLabel;
	m_magnifiedImageLabel = 0;
}

/*******************************************************************/

void PresentationWidget::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
		case Qt::Key_Left:
		case Qt::Key_Backspace:
		case Qt::Key_PageUp:
		case Qt::Key_Up:
			slotGoToPreviousPage();
			break;
		case Qt::Key_Right:
		case Qt::Key_Space:
		case Qt::Key_PageDown:
		case Qt::Key_Down:
			slotGoToNextPage();
			break;
		case Qt::Key_Home:
			slotGoToFirstPage();
			break;
		case Qt::Key_End:
			slotGoToLastPage();
			break;
		case Qt::Key_Escape:
			if (!m_topBar->isHidden())
				showTopBar(false);
			else
				close();
			break;
	}
}

void PresentationWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!m_topBar->isHidden())
	{
		if (event->y() > m_topBar->height() + 1)
		{
			showTopBar(false);
			setFocus(Qt::OtherFocusReason);
		}
	}
	else
	{
		if (event->y() <= 1)
			showTopBar(true);
	}
}

#ifndef QT_NO_WHEELEVENT
void PresentationWidget::wheelEvent(QWheelEvent *event)
{
	slotScrollToNextOrPreviousPage(event->delta());
}
#endif // QT_NO_WHEELEVENT
