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

#ifndef PDFVIEWER_PRESENTATIONWIDGET_H
#define PDFVIEWER_PRESENTATIONWIDGET_H

#include <QtGui/QWidget>
#include <poppler-qt4.h>

class Label;
class QAction;
class GoToPageAction;
class QTimer;
class QToolBar;

class PresentationWidget : public QWidget
{
	Q_OBJECT

public:
	PresentationWidget(QWidget *parent = 0);
	~PresentationWidget();

	void setPopplerPages(const QList<Poppler::Page*> &popplerPages);
	void setPopplerPageLabels(const QStringList &popplerPageLabels);
	void setPage(int pageNumber);

Q_SIGNALS:
	void loadPage(Poppler::Page *popplerPage, qreal zoomFactor, int pageNumber);
	void pageChanged(int pageNumber);
	void doAction(Poppler::LinkAction::ActionType);

protected:
	/*virtual*/ void keyPressEvent(QKeyEvent *event);
	/*virtual*/ void mouseMoveEvent(QMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
	/*virtual*/ void wheelEvent(QWheelEvent *event);
#endif // QT_NO_WHEELEVENT

private Q_SLOTS:
	void slotDelayedPageChange();
	void slotDelayedPageLoad();
	void slotGoToPreviousPage();
	void slotGoToNextPage();
	void slotGoToFirstPage();
	void slotGoToLastPage();
	void slotScrollToNextOrPreviousPage(int delta);
	void slotDoAction(Poppler::LinkAction::ActionType actionType);
	void slotMagnify(const QPoint &pos);
	void slotMagnifyEnded();
	void slotSetPage(int pageNumber);
	void slotSetPage(double pageNumber);
	void slotOpenPageSelector();

private:
	void initTopBar();
	void showTopBar(bool visible);
	void delayedPageChange();
	void delayedPageLoad();
	void showPage(Poppler::Page *popplerPage, const QImage &image);
	void loadPage(int pageNumber);

	QList<Poppler::Page*> m_popplerPages;
	QStringList m_popplerPageLabels;
	Label *m_imageLabel;
	int m_pageNumber;

	int m_dpiX;
	int m_dpiY;

	Label *m_magnifiedImageLabel;

	QToolBar *m_topBar;
	QAction *m_goToPreviousPageAction;
	GoToPageAction *m_goToPageAction;
	QAction *m_goToNextPageAction;

	QTimer *m_pageChangeTimer;
	QTimer *m_pageLoadTimer;
	QList<QImage> m_cachedImages;
	int m_preloadingPageNumber;
	int m_cachedCount;
};

#endif // PDFVIEWER_PRESENTATIONWIDGET_H
