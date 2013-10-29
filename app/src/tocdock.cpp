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

#include "tocdock.h"

#include <poppler-qt4.h>

#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidget>

static void fillToc(Poppler::Document *doc, const QDomNode &parent, QTreeWidget *tree, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *newitem = 0;
    for (QDomNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling()) {
        QDomElement e = node.toElement();

		// for some unknown reason e.attribute("Destination") does not exist while Okular successfully uses it; so we cannot use Poppler::LinkDestination(e.attribute(QString::fromLatin1("Destination"))).pageNumber() and must use the following
//		const int pageNumber = doc->linkDestination(e.attribute(QString::fromLatin1("DestinationName")))->pageNumber();
		Poppler::LinkDestination *dest = doc->linkDestination(e.attribute(QString::fromLatin1("DestinationName")));
		const double pageNumber = dest->pageNumber() + dest->top();
		delete dest;

        if (!parentItem) {
            newitem = new QTreeWidgetItem(tree, newitem);
        } else {
            newitem = new QTreeWidgetItem(parentItem, newitem);
        }
        newitem->setText(0, e.tagName());
		newitem->setData(0, Qt::UserRole, pageNumber);

        bool isOpen = false;
        if (e.hasAttribute(QString::fromLatin1("Open"))) {
            isOpen = QVariant(e.attribute(QString::fromLatin1("Open"))).toBool();
        }
        if (isOpen) {
            tree->expandItem(newitem);
        }

        if (e.hasChildNodes()) {
            fillToc(doc, node, tree, newitem);
        }
    }
}


TocDock::TocDock(QWidget *parent)
    : AbstractInfoDock(parent)
{
	m_tree = 0;
	setWindowTitle(tr("Table of &Contents"));
	setObjectName("TOCDock");
}

TocDock::~TocDock()
{
}

void TocDock::loadWidget()
{
	m_tree = new QTreeWidget(this);
	m_tree->setAlternatingRowColors(true);
	m_tree->header()->hide();
	m_tree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	connect(m_tree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(goToPage(QTreeWidgetItem*,int)));
	setWidget(m_tree);
}

void TocDock::fillInfo()
{
	if (!m_tree) // use delayed initialization for faster startup if not all docks are visible
		loadWidget();

    const QDomDocument *toc = document()->toc();
    if (toc) {
        fillToc(document(), *toc, m_tree, 0);
    } else {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, tr("No table of contents found."));
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        m_tree->addTopLevelItem(item);
    }
	delete toc;
}

void TocDock::documentClosed()
{
	if (m_tree)
		m_tree->clear();
	AbstractInfoDock::documentClosed();
}

void TocDock::goToPage(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(column);
	const double pageNumber = item->data(0, Qt::UserRole).toDouble() - 1;
	setPage(pageNumber);
}
