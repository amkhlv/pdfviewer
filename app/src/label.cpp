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
#include "label.h"

#include <poppler-qt4.h>

#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtGui/QPainter>
#ifndef QT_NO_TOOLTIP
#include <QtGui/QToolTip>
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHEELEVENT
#include <QtGui/QWheelEvent>
#else
#include <QtGui/QMouseEvent>
#endif // QT_NO_WHEELEVENT

Label::Label(QWidget *parent)
	: QLabel(parent)
	, m_hoveredLink(-1)
	, m_isMagnifying(false)
	, m_highlightTop(-1)
	, m_highlightLeft(-1)
	, m_highlightRight(-1)
	, m_highlightBottom(-1)
{
//	setStyleSheet(QLatin1String("Label { border: 1px solid black; background-color: white; }"));
	setMouseTracking(true);
}

/*******************************************************************/

/*
static QList<Link> getAnnotations(Poppler::Page *popplerPage)
{
	QList<Annotation> annotations;

	QList <Poppler::Annotation*> popplerAnnotations = popplerPage->annotations();
	annotations.reserve(popplerAnnotations.size());

	while (!popplerAnnotations.isEmpty())
	{
		Poppler::Annotation* popplerAnnotation = popplerAnnotations.takeFirst();
		Annotation annotation;
		switch (popplerAnnotation->subType())
		{
//			case Poppler::Annotation::AFileAttachment:
//			{
//				PopplerFileAttachmentAnnotation *attachAnnotation = static_cast<Poppler::FileAttachmentAnnotation*>(popplerAnnotation);
//				annotation.fileIconName = attachAnnotation->fileIconName();
//				annotation.embeddedFile = attachAnnotation->embeddedFile();
//			}
		}
		delete popplerAnnotation;
	}

	return annotations;
}

void Label::generateAnnotations(Poppler::Page *popplerPage)
{
	m_annotations.clear();
	m_annotations = getAnnotations(popplerPage);
}
*/

/*******************************************************************/

static QList<Link> getLinks(Poppler::Page *popplerPage, const QStringList &popplerPageLabels)
{
	QList<Link> links;

	QList <Poppler::Link*> popplerLinks = popplerPage->links();
	links.reserve(popplerLinks.size());
	while (!popplerLinks.isEmpty())
	{
		Poppler::Link *popplerLink = popplerLinks.takeFirst();
		Link link;
//		link.linkArea = popplerLink->linkArea();
		const QRectF linkArea = popplerLink->linkArea();
		link.linkArea = QRectF(linkArea.left(), qMin(linkArea.top(), linkArea.bottom()), qAbs(linkArea.right() - linkArea.left()), qAbs(linkArea.bottom() - linkArea.top())); // poppler switches top and bottom of this box :(
		switch (popplerLink->linkType())
		{
			case Poppler::Link::Goto:
			{
				const Poppler::LinkGoto *popplerLinkGoto = static_cast<const Poppler::LinkGoto*>(popplerLink);
				const Poppler::LinkDestination popplerDest = popplerLinkGoto->destination();
				link.pageNumber = popplerDest.pageNumber() - 1 + popplerDest.top();
				link.pageLabel = popplerPageLabels.at(int(link.pageNumber));
			}
			break;
			case Poppler::Link::Browse:
			{
				const Poppler::LinkBrowse *popplerLinkBrowse = static_cast<const Poppler::LinkBrowse*>(popplerLink);
				link.url = popplerLinkBrowse->url();
			}
			break;
			case Poppler::Link::Action:
			{
				const Poppler::LinkAction *popplerLinkAction = static_cast<const Poppler::LinkAction*>(popplerLink);
				link.pageNumber = -1; // since Poppler::LinkAction::ActionType doesn't specify a "None" value, we use pageNumber to distinguish this type of action, we do not check whether popplerLinkAction->actionType() is > 0 because it is not documented that all valid action types have a value > 0
				link.actionType = popplerLinkAction->actionType();
			}
			break;
			case Poppler::Link::Execute: // TODO
			case Poppler::Link::Sound: // TODO
			case Poppler::Link::Movie: // not implemented in poppler 0.16
			case Poppler::Link::JavaScript: // TODO
			default: // do nothing
			break;
		}
		links << link;
		delete popplerLink;
	}

	return links;
}

void Label::generateLinks(Poppler::Page *popplerPage, const QStringList &popplerPageLabels)
{
	m_links.clear();
	m_links = getLinks(popplerPage, popplerPageLabels);
}

void Label::openLink(const QString &url)
{
	QDesktopServices::openUrl(QUrl(url));
}

/*******************************************************************/

void Label::setHighlight(int left, int top, int right, int bottom)
{
	m_highlightTop = top;
	m_highlightLeft = left;
	m_highlightRight = right;
	m_highlightBottom = bottom;
}

void Label::paintEvent(QPaintEvent *event)
{
	QLabel::paintEvent(event);

	// highlight search word (if found)
	QPainter painter(this);
	const QColor color(100, 160, 255, 100);
	const int offset = 1;
	painter.fillRect(m_highlightLeft - offset, m_highlightTop - offset, m_highlightRight - m_highlightLeft + 2 * offset, m_highlightBottom - m_highlightTop + 2 * offset, color);
	painter.end();
}

/*******************************************************************/

void Label::mousePressEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton || m_hoveredLink >= 0)
	{
		QLabel::mousePressEvent(event);
		return;
	}
	// start magnifying
	m_isMagnifying = true;
	emit magnify(event->pos());
}

void Label::mouseMoveEvent(QMouseEvent *event)
{
	// magnify
	if (m_isMagnifying)
		emit magnify(event->pos());
	// hover on link
	else
	{
		m_hoveredLink = -1;
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
		for (int i = 0; i < m_links.size(); ++i)
		{
			const QPointF mousePos(qreal(event->pos().x()) / width(), qreal(event->pos().y()) / height());
			if (m_links.at(i).linkArea.contains(mousePos))
			{
				m_hoveredLink = i;
				break;
			}
		}
		if (m_hoveredLink >= 0)
		{
#ifndef QT_NO_CURSOR
			QApplication::setOverrideCursor(Qt::PointingHandCursor);
#endif // QT_NO_CURSOR
			Link link = m_links.at(m_hoveredLink);
#ifndef QT_NO_TOOLTIP
			if (!link.url.isEmpty())
				QToolTip::showText(this->mapToGlobal(QPoint(0,0)) + event->pos(), tr("Go to %1").arg(link.url), this);
			else if (link.pageNumber >= 0)
				QToolTip::showText(this->mapToGlobal(QPoint(0,0)) + event->pos(), tr("Go to page %1").arg(link.pageLabel), this);
			else
			{
				QString actionText;
				switch (link.actionType)
				{
					case Poppler::LinkAction::PageFirst: actionText = tr("Go to first page", "Link action text"); break;
					case Poppler::LinkAction::PagePrev: actionText = tr("Go to previous page", "Link action text"); break;
					case Poppler::LinkAction::PageNext: actionText = tr("Go to next page", "Link action text"); break;
					case Poppler::LinkAction::PageLast: actionText = tr("Go to last page", "Link action text"); break;
					case Poppler::LinkAction::HistoryBack: actionText = tr("Backward", "Link action text"); break;
					case Poppler::LinkAction::HistoryForward: actionText = tr("Forward", "Link action text"); break;
					case Poppler::LinkAction::Quit: actionText = tr("Quit application", "Link action text"); break;
					case Poppler::LinkAction::Presentation: actionText = tr("Enter presentation mode", "Link action text"); break;
					case Poppler::LinkAction::EndPresentation: actionText = tr("Exit presentation mode", "Link action text"); break;
					case Poppler::LinkAction::Find: actionText = tr("Find...", "Link action text"); break;
					case Poppler::LinkAction::GoToPage: actionText = tr("Go to page...", "Link action text"); break;
					case Poppler::LinkAction::Close: actionText = tr("Close document", "Link action text"); break;
					case Poppler::LinkAction::Print: actionText = tr("Print...", "Link action text"); break;
				}
				QToolTip::showText(this->mapToGlobal(QPoint(0,0)) + event->pos(), actionText, this);
			}
#endif // QT_NO_TOOLTIP
		}
		else
		{
#ifndef QT_NO_TOOLTIP
			QToolTip::hideText();
#endif // QT_NO_TOOLTIP
			event->ignore(); // make sure that the mouse move event propagates to the presentation widget if in presentation mode
		}
	}
}

void Label::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
	{
		QLabel::mouseReleaseEvent(event);
		return;
	}
	// click on link
	else if (m_hoveredLink >= 0)
	{
		Link link = m_links.at(m_hoveredLink);
		if (!link.url.isEmpty()) // we have a Browse link
			openLink(link.url);
		else if (link.pageNumber >= 0) // we have a Goto link
			emit goToPage(link.pageNumber);
		else // we have an Action link
			emit doAction(link.actionType);
		return;
	}

	// stop magnifying
	m_isMagnifying = false;
	emit endMagnify();
}

#ifndef QT_NO_WHEELEVENT
void Label::wheelEvent(QWheelEvent *event)
{
	// zoom in/out
	if (event->modifiers() == Qt::ControlModifier)
	{
		if (event->delta() > 0)
			emit requestZoomIn();
		else
			emit requestZoomOut();
	}
	// scroll page
	else
		emit scroll(event->delta());
}
#endif // QT_NO_WHEELEVENT
