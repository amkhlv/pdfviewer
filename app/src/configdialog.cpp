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

#include "configdialog.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
	const QString editorWhatsThis = tr("<p>Enter the editor command you "
	    "want to launch when %1 wants to open a LaTeX source file.</p>"
	    "<p>The string %f will be replaced by the file name and %l by "
	    "the line number.</p>",
	    "%1 = application name").arg(QCoreApplication::applicationName());
	ui.editorLabel->setWhatsThis(editorWhatsThis);
	ui.editorLineEdit->setWhatsThis(editorWhatsThis);
	const QString cacheSizeWhatsThis = tr("<p>Specify the maximum size "
	    "of the cache containing the file specific information which is "
	    "remembered throughout sessions, such as the last opened location, "
	    "zoom factor and bookmarks.</p>");
	ui.cacheSizeLabel->setWhatsThis(cacheSizeWhatsThis);
	ui.cacheSizeSpinBox->setWhatsThis(cacheSizeWhatsThis);
	setWindowTitle(tr("Configure %1", "Configuration dialog window title").arg(QCoreApplication::applicationName()));
	readSettings();
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::readSettings()
{
	QSettings settings;
	const int cacheSize = settings.value("FileInfoCacheMaxSize", QString(PDFVIEWER_FILE_INFO_CACHE_MAX_SIZE).toDouble()).toInt() / 1000000;
	const QString editor = settings.value("Editor", PDFVIEWER_TEX_EDITOR).toString();
	settings.beginGroup("MainWindow");
	const bool showMenuBar = settings.value("ShowMenuBar", false).toBool();
	settings.endGroup();
	ui.cacheSizeSpinBox->setValue(cacheSize);
	ui.editorLineEdit->setText(editor);
	ui.showMenuBarCheckBox->setChecked(showMenuBar);
}

void ConfigDialog::writeSettings()
{
	QSettings settings;
	settings.setValue("FileInfoCacheMaxSize", ui.cacheSizeSpinBox->value() * 1000000);
	settings.setValue("Editor", ui.editorLineEdit->text());
	settings.beginGroup("MainWindow");
	settings.setValue("ShowMenuBar", ui.showMenuBarCheckBox->isChecked());
	settings.endGroup();
}

void ConfigDialog::accept()
{
	writeSettings();
	QDialog::accept();
}
