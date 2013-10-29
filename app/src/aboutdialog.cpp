/***************************************************************************
 *   Copyright (C) 2007, 2012 by Glad Deschrijver                          *
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

#include "aboutdialog.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("About %1").arg(QCoreApplication::applicationName()));

	// Title widget
	QLabel *pixmapLabel = new QLabel;
	pixmapLabel->setPixmap(QPixmap(":/icons/pdfviewer-64.png"));
	QLabel *label = new QLabel(QString("<p><font size=\"5\">%1</font><br /><b>%2</b><br />%3</p>")
	    .arg(QCoreApplication::applicationName())
	    .arg(tr("Version %1").arg(QCoreApplication::applicationVersion()))
	    .arg(tr("Using Qt Toolkit version %1").arg(qVersion())));
	label->setWordWrap(true);

	QWidget *titleWidget = new QWidget;
	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->addWidget(pixmapLabel);
	titleLayout->addWidget(label);
	titleLayout->setStretch(1, 1);
	titleWidget->setLayout(titleLayout);

	// Main widget
	QTabWidget *tabWidget = new QTabWidget;

	QLabel *aboutLabel = new QLabel(tr("<p>A PDF Viewer</p>"
	    "<p>This is a program for viewing PDF files.</p><p>%1</p>")
	    .arg(QString::fromUtf8("Copyright (C) 2008, 2009, Pino Toscano<br />"
	    "Copyright (C) 2008, Albert Astals Cid<br />"
	    "Copyright (C) 2009, Shawn Rutledge<br />"
	    "Copyright (C) 2007-2011, Jonathan Kew, Stefan L\303\266ffler<br />"
	    "Copyright (C) 2008-2011, Jerome Laurens (SyncTeX)<br />"
	    "Copyright (C) 2012, Glad Deschrijver")));
	aboutLabel->setContentsMargins(6, 6, 6, 6);
	tabWidget->addTab(aboutLabel, tr("&About"));

	QLabel *authorsLabel = new QLabel(tr("<p>Maintainer:<br />%1</p>"
	    "<p>Former Authors (authors of the Poppler Qt4 Demo PDF Viewer<br />on which this program is based):<br />%2</p>"
	    "<p>Other contributions:<br />%3</p>")
	    .arg("Glad Deschrijver &lt;<a href=\"mailto:glad.deschrijver@gmail.com\">glad.deschrijver@gmail.com</a>&gt;")
	    .arg("Copyright (C) 2008, 2009, Pino Toscano &lt;<a href=\"mailto:pino@kde.org\">pino@kde.org</a>&gt;<br />"
	    "Copyright (C) 2008, Albert Astals Cid &lt;<a href=\"mailto:aacid@kde.org\">aacid@kde.org</a>&gt;<br />"
	    "Copyright (C) 2009, Shawn Rutledge &lt;<a href=\"mailto:shawn.t.rudledge@gmail.com\">shawn.t.rutledge@gmail.com</a>&gt;")
	    .arg(QString::fromUtf8("Copyright (C) 2007-2011 Jonathan Kew, Stefan L\303\266ffler &lt;<a href=\"http://www.tug.org/texworks/\">http://www.tug.org/texworks/</a>&gt;<br />"
	    "Copyright (C) 2008-2011 Jerome Laurens (SyncTeX) &lt;<a href=\"mailto:jerome.laurens@u-bourgogne.fr\">jerome.laurens@u-bourgogne.fr</a>&gt;")));
	authorsLabel->setContentsMargins(6, 6, 6, 6);
	authorsLabel->setOpenExternalLinks(true);
	tabWidget->addTab(authorsLabel, tr("A&uthors"));

	QTextBrowser *licenseEdit = new QTextBrowser;
	licenseEdit->setHtml(tr("<p>This program is free "
	    "software; you can redistribute it and/or modify it under the "
	    "terms of the GNU General Public License as published by the "
	    "Free Software Foundation; either version 2 of the License, "
	    "or (at your option) any later version.</p>"
	    "<p>This program is distributed in the hope that it will "
	    "be useful, but WITHOUT ANY WARRANTY; without even the implied "
	    "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  "
	    "See the <a href=\"http://www.gnu.org/licenses/\">GNU General Public License</a> for more details.</p>"));
	licenseEdit->setReadOnly(true);
	licenseEdit->setOpenExternalLinks(true);
	tabWidget->addTab(licenseEdit, tr("&License"));

	// Accept button
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(titleWidget);
	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(buttonBox);
	mainLayout->setSpacing(10);
	buttonBox->setFocus();
}
