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

#ifndef PDFVIEWER_LABEL_H
#define PDFVIEWER_LABEL_H

#include <QtGui/QLabel>
#include <poppler-qt4.h>

struct Link
{
	QRectF linkArea;
	double pageNumber;
	QString pageLabel;
	QString url;
	Poppler::LinkAction::ActionType actionType;
};
//Q_DECLARE_TYPEINFO(Link, Q_PRIMITIVE_TYPE);

class Label : public QLabel
{
	Q_OBJECT

public:
	Label(QWidget *parent = 0);

	void generateLinks(Poppler::Page *popplerPage, const QStringList &popplerPageLabels);
	void setHighlight(int left, int top, int right, int bottom);

Q_SIGNALS:
	void magnify(const QPoint &pos);
	void endMagnify();
	void scroll(int delta);
	void goToPage(double pageNumber);
	void doAction(Poppler::LinkAction::ActionType actionType);
	void requestZoomIn();
	void requestZoomOut();

protected:
	/*virtual*/ void paintEvent(QPaintEvent *event);
	/*virtual*/ void mousePressEvent(QMouseEvent *event);
	/*virtual*/ void mouseMoveEvent(QMouseEvent *event);
	/*virtual*/ void mouseReleaseEvent(QMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
	/*virtual*/ void wheelEvent(QWheelEvent *event);
#endif // QT_NO_WHEELEVENT

private:
	void openLink(const QString &url);

	QList<Link> m_links;
	int m_hoveredLink;
	bool m_isMagnifying;

	int m_highlightTop;
	int m_highlightLeft;
	int m_highlightRight;
	int m_highlightBottom;
};

#endif // PDFVIEWER_LABEL_H
