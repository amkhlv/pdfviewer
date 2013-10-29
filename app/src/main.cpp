/*
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include "viewer.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#ifdef USE_QDBUS
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusReply>
#include "pdfviewer_adaptor.h"
#include "pdfviewer_interface.h"
#ifdef Q_OS_WIN32
#include <process.h> // for getting process id under windows
#else // Q_OS_WIN32
#include <unistd.h> // for getting process id under unix systems
#endif // Q_OS_WIN32
#endif // USE_QDBUS

// add copyright notice to the *.ts files; this string is not used anywhere else
static struct { const char *source; const char *comment; } copyrightString = QT_TRANSLATE_NOOP3("__Copyright__",
	"The original English text is copyrighted by the authors of the source "
	"files where the strings come from. This file is distributed under the "
	"same license as the PdfViewer package. The translations in this file are "
	"copyrighted as follows.",
	"Translators: do not translate this, but put in the \"translation\" "
	"a copyright notice of the form \"This file was translated by <NAME>. "
	"Copyright (C) <YEAR> <NAME>.\" in which you fill in the year(s) of "
	"translation and your name.");

bool findTranslator(QTranslator *translator, const QString &transName, const QString &transDir)
{
	const QString qmFile = transName + ".qm";
	const QFileInfo fi(QDir(transDir), qmFile);
	if (fi.exists())
		return translator->load(qmFile, transDir);
	return false;
}

QTranslator *createTranslator(const QString &transName, const QString &transDir)
{
	const QString locale = QString(QLocale::system().name());
	const QString localeShort = locale.left(2).toLower();

	QTranslator *translator = new QTranslator(0);

	const QStringList transDirs = QStringList() << transDir
#ifdef PDFVIEWER_TRANSLATIONS_INSTALL_DIR
		<< QDir(PDFVIEWER_TRANSLATIONS_INSTALL_DIR).absolutePath() // set during compilation
#endif // PDFVIEWER_TRANSLATIONS_INSTALL_DIR
#ifdef PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR
		<< QDir(PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR).absolutePath() // set during compilation
#endif // PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR
		<< ""; // working dir

	for (int i = 0; i < transDirs.size(); ++i)
	{
		if (findTranslator(translator, transName + '_' + locale, transDirs.at(i)))
			return translator;
		if (findTranslator(translator, transName + '_' + localeShort, transDirs.at(i)))
			return translator;
	}

	return translator;
}

void addToSessionBus(PdfViewer *viewer)
{
#ifdef USE_QDBUS
	QDBusConnection connection = QDBusConnection::sessionBus();
	if (connection.isConnected())
	{
		new PdfViewerAdaptor(viewer);
		connection.registerService("glad.PdfViewer" + QString::number(getpid()));
		connection.registerObject("/MainWindow", viewer);
	}
#endif // USE_QDBUS
}

bool syncExistingInSessionBus(const QString &fileName, const QString &sourceFile, int lineNumber)
{
#ifdef USE_QDBUS
	// sync with source if the PDF file is already open in some PdfViewer window
	QDBusConnection connection = QDBusConnection::sessionBus();
	if (connection.isConnected())
	{
		QDBusReply<QStringList> reply = connection.interface()->registeredServiceNames();
		if (!reply.isValid())
			fprintf(stderr, "Error: %s\n", qPrintable(reply.error().message()));
		else
		{
			Q_FOREACH(const QString &name, reply.value())
			{
				if (!name.startsWith(QLatin1String("glad.PdfViewer")))
					continue;
				PdfViewerInterface interface(name, "/MainWindow", connection, qApp);
				if (interface.currentDocument().value() == fileName)
				{
					interface.syncFromSource(sourceFile, lineNumber);
					QTimer::singleShot(200, qApp, SLOT(quit()));
					return true;
				}
			}
		}
	}
	else
	{
		fprintf(stderr, "Cannot connect to the D-BUS session bus.\n"
		    "To start it, run:\n"
		    "\tdbus-launch --auto-syntax\n\n"
		    "The program will continue, but forward search will open a new window each time it is used.\n");
	}
#endif // USE_QDBUS
	return false;
}

bool syncFromSource(PdfViewer *viewer, const QString &arg)
{
	// arg is of the form "file:fileName.pdf#src:12 fileName.tex" where 12 can be any line number and fileName can be any file name
	// we assume that the file name doesn't contain the substring "#src:"

	// get PDF file name, source file name and line number in source file
	const int endOfFileName = arg.indexOf(QLatin1String("#src:"));
	const int endOfPageNumber = arg.indexOf(QLatin1Char(' '), endOfFileName);
	const QString fileName = QFileInfo(arg.mid(5, endOfFileName-5)).absoluteFilePath();
	const QString sourceFile = arg.mid(endOfPageNumber+1);
	const int lineNumber = arg.mid(endOfFileName+5, endOfPageNumber-endOfFileName-5).toInt();

	if (syncExistingInSessionBus(fileName, sourceFile, lineNumber))
		return true;

	// load PDF file and sync with source
	viewer->loadDocument(fileName);
	viewer->syncFromSource(sourceFile, lineNumber);
	return false;
}

int main(int argc, char *argv[])
{
//QTime t = QTime::currentTime();
	Q_UNUSED(copyrightString);

    QApplication app(argc, argv);
	QCoreApplication::setOrganizationName(ORGNAME);
	QCoreApplication::setApplicationName(APPNAME);
	QCoreApplication::setApplicationVersion(APPVERSION);

	// load translations
	const QString translationsDirPath = qgetenv("PDFVIEWER_TRANSLATIONS_DIR");
	QTranslator *qtTranslator = createTranslator("qt", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	QTranslator *pdfviewerTranslator = createTranslator("pdfviewer", translationsDirPath);
	QTranslator *pdfviewlibTranslator = createTranslator("pdfviewlib", translationsDirPath);
	app.installTranslator(qtTranslator);
	app.installTranslator(pdfviewerTranslator);
	app.installTranslator(pdfviewlibTranslator);

    PdfViewer viewer;

	// load file if passed as the first argument
    const QStringList args = QCoreApplication::arguments();
    if (args.count() > 1) {
		app.processEvents(); // make sure the window appears before loading the document, so the user sees that the app is being loaded, even if the file takes very long to load (e.g. the PGF manual)
		// synctex
		if (args.count() > 1)
		{
			if (args.at(1).startsWith(QLatin1String("file:")))
			{
				if (syncFromSource(&viewer, args.at(1)))
					return 0;
			}
			else
			{
			    viewer.show();
				viewer.loadDocument(args.at(1));
			}
		}
    }

    viewer.show();
	addToSessionBus(&viewer);
//qCritical() << t.msecsTo(QTime::currentTime());

    int success = app.exec();

	delete qtTranslator;
	delete pdfviewerTranslator;
	return success;
}
