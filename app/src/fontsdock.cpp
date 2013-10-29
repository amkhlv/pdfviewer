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

#include "fontsdock.h"

#include <poppler-qt4.h>

#include <QtCore/QCoreApplication>
#include <QtGui/QTableWidget>

static QString yesNoStatement(bool value)
{
    return value ? QCoreApplication::translate("FontsDock", "yes") : QCoreApplication::translate("FontsDock", "no");
}

FontsDock::FontsDock(QWidget *parent)
    : AbstractInfoDock(parent)
{
	m_table = 0;
	setWindowTitle(tr("&Fonts"));
	setObjectName("FontsDock");
}

FontsDock::~FontsDock()
{
}

void FontsDock::loadWidget()
{
	m_table = new QTableWidget(this);
	m_table->setColumnCount(5);
	m_table->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Type") << tr("Embedded") << tr("Subset") << tr("File"));
	m_table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	setWidget(m_table);
}

void FontsDock::fillInfo()
{
	if (!m_table) // use delayed initialization for faster startup if not all docks are visible
		loadWidget();

    const QList<Poppler::FontInfo> fonts = document()->fonts();
    m_table->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Type") << tr("Embedded") << tr("Subset") << tr("File"));
    m_table->setRowCount(fonts.count());
    int i = 0;
    Q_FOREACH(const Poppler::FontInfo &font, fonts) {
        if (font.name().isNull()) {
            m_table->setItem(i, 0, new QTableWidgetItem(QString::fromLatin1("[none]")));
        } else {
            m_table->setItem(i, 0, new QTableWidgetItem(font.name()));
        }
        m_table->setItem(i, 1, new QTableWidgetItem(font.typeName()));
        m_table->setItem(i, 2, new QTableWidgetItem(yesNoStatement(font.isEmbedded())));
        m_table->setItem(i, 3, new QTableWidgetItem(yesNoStatement(font.isSubset())));
        m_table->setItem(i, 4, new QTableWidgetItem(font.file()));
        ++i;
    }
	m_table->resizeColumnsToContents();
}

void FontsDock::documentClosed()
{
	if (m_table)
	{
		m_table->clear();
		m_table->setRowCount(0);
	}
	AbstractInfoDock::documentClosed();
}
