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

#include "optcontentdock.h"

#include <poppler-qt4.h>

#include <QtGui/QTreeView>

OptContentDock::OptContentDock(QWidget *parent)
    : AbstractInfoDock(parent)
{
	m_view = 0;
	setWindowTitle(tr("&Optional Content"));
	setObjectName("OptionalContentDock");
}

OptContentDock::~OptContentDock()
{
}

void OptContentDock::loadWidget()
{
	m_view = new QTreeView(this);
	m_view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	setWidget(m_view);
}

void OptContentDock::documentLoaded()
{
    AbstractInfoDock::documentLoaded();
    if ( document()->pageMode() == Poppler::Document::UseOC ) {
        show();
    }
}

void OptContentDock::fillInfo()
{
	if (!m_view) // use delayed initialization for faster startup if not all docks are visible
		loadWidget();

    if (!document()->hasOptionalContent()) {
        return;
    }

    m_view->setModel(document()->optionalContentModel());
    connect(m_view->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(reloadImage()));
    m_view->expandToDepth(1);
}

void OptContentDock::documentClosed()
{
	if (m_view)
		m_view->setModel(0);
	AbstractInfoDock::documentClosed();
}

void OptContentDock::reloadImage()
{
    reloadPage();
}
