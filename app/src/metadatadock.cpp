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

#include "metadatadock.h"

#include <poppler-qt4.h>

#include <QtGui/QTextEdit>

MetadataDock::MetadataDock(QWidget *parent)
    : AbstractInfoDock(parent)
{
	m_edit = 0;
	setWindowTitle(tr("&Metadata"));
	setObjectName("MetadataDock");
}

MetadataDock::~MetadataDock()
{
}

void MetadataDock::loadWidget()
{
	m_edit = new QTextEdit(this);
	m_edit->setAcceptRichText(false);
	m_edit->setReadOnly(true);
	setWidget(m_edit);
}


void MetadataDock::fillInfo()
{
	if (!m_edit) // use delayed initialization for faster startup if not all docks are visible
		loadWidget();

    m_edit->setPlainText(document()->metadata());
}

void MetadataDock::documentClosed()
{
	if (m_edit)
		m_edit->clear();
	AbstractInfoDock::documentClosed();
}
