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

#include "embeddedfilesdock.h"

#include <poppler-qt4.h>

#include <QtGui/QTableWidget>

EmbeddedFilesDock::EmbeddedFilesDock(QWidget *parent)
    : AbstractInfoDock(parent)
{
	m_table = 0;
	setWindowTitle(tr("&Embedded Files"));
	setObjectName("EmbeddedFilesDock");
}

EmbeddedFilesDock::~EmbeddedFilesDock()
{
}

void EmbeddedFilesDock::loadWidget()
{
	m_table = new QTableWidget(this);
	m_table->setColumnCount(6);
	m_table->setHorizontalHeaderLabels(
	    QStringList() << tr("Name") << tr("Description") << tr("Size") << tr("Creation date")
	                  << tr("Modification date") << tr("Checksum"));
	m_table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	setWidget(m_table);
}

void EmbeddedFilesDock::fillInfo()
{
	if (!m_table) // use delayed initialization for faster startup if not all docks are visible
		loadWidget();

    m_table->setHorizontalHeaderLabels(
        QStringList() << tr("Name") << tr("Description") << tr("Size") << tr("Creation date")
                      << tr("Modification date") << tr("Checksum"));
    if (!document()->hasEmbeddedFiles()) {
        m_table->setItem(0, 0, new QTableWidgetItem(tr("No files")));
        return;
    }

    const QList<Poppler::EmbeddedFile*> files = document()->embeddedFiles();
    m_table->setRowCount(files.count());
    int i = 0;
    Q_FOREACH(Poppler::EmbeddedFile *file, files) {
        m_table->setItem(i, 0, new QTableWidgetItem(file->name()));
        m_table->setItem(i, 1, new QTableWidgetItem(file->description()));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(file->size())));
        m_table->setItem(i, 3, new QTableWidgetItem(file->createDate().toString(Qt::SystemLocaleDate)));
        m_table->setItem(i, 4, new QTableWidgetItem(file->modDate().toString(Qt::SystemLocaleDate)));
        const QByteArray checksum = file->checksum();
        const QString checksumString = !checksum.isEmpty() ? QString::fromAscii(checksum.toHex()) : QString::fromLatin1("n/a");
        m_table->setItem(i, 5, new QTableWidgetItem(checksumString));
        ++i;
    }
}

void EmbeddedFilesDock::documentLoaded()
{
    if ( document()->pageMode() == Poppler::Document::UseAttach ) {
        show();
    }
}

void EmbeddedFilesDock::documentClosed()
{
	if (m_table)
	{
		m_table->clear();
		m_table->setRowCount(0);
	}
	AbstractInfoDock::documentClosed();
}
