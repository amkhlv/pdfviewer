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
#include "printhandler.h"

#include <poppler-qt4.h>
#include <poppler-form.h>

#include <QtCore/QPointer>
#include <QtCore/QProcess>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QMessageBox>
#include <QtGui/QRadioButton>

PrintHandler::PrintHandler(QWidget *parent)
	: QObject(parent)
{
}

PrintHandler::~PrintHandler()
{
}

/*******************************************************************/
// Cups options page

QWidget *PrintHandler::cupsOptionsPagesWidget()
{
	QWidget *widget = new QWidget;
	widget->setWindowTitle(tr("Pages"));
	QVBoxLayout *layout = new QVBoxLayout(widget);

	QGroupBox *pagesGroupBox = new QGroupBox(widget);
	pagesGroupBox->setTitle(tr("Pages Per Sheet"));
	QGridLayout *pagesLayout = new QGridLayout(pagesGroupBox);
	m_onePageRadio = new QRadioButton(tr("1", "# pages per sheet"), widget);
	m_onePageRadio->setChecked(true);
	m_twoPagesRadio = new QRadioButton(tr("2", "# pages per sheet"), widget);
	m_fourPagesRadio = new QRadioButton(tr("4", "# pages per sheet"), widget);
	m_sixPagesRadio = new QRadioButton(tr("6", "# pages per sheet"), widget);
	m_ninePagesRadio = new QRadioButton(tr("9", "# pages per sheet"), widget);
	m_sixteenPagesRadio = new QRadioButton(tr("16", "# pages per sheet"), widget);
	m_pagesFlowComboBox = new QComboBox(widget);
	m_pagesFlowComboBox->addItems(QStringList()
	    << tr("Left to Right, Top to Bottom")
	    << tr("Left to Right, Bottom to Top")
	    << tr("Right to Left, Top to Bottom")
	    << tr("Right to Left, Bottom to Top")
	    << tr("Bottom to Top, Left to Right")
	    << tr("Bottom to Top, Right to Left")
	    << tr("Top to Bottom, Left to Right")
	    << tr("Top to Bottom, Right to Left")
	);
	pagesLayout->addWidget(m_onePageRadio, 0, 0);
	pagesLayout->addWidget(m_twoPagesRadio, 1, 0);
	pagesLayout->addWidget(m_fourPagesRadio, 2, 0);
	pagesLayout->addWidget(m_sixPagesRadio, 0, 1);
	pagesLayout->addWidget(m_ninePagesRadio, 1, 1);
	pagesLayout->addWidget(m_sixteenPagesRadio, 2, 1);
	pagesLayout->addWidget(m_pagesFlowComboBox, 3, 0, 1, 2);

	layout->addWidget(pagesGroupBox);

	return widget;
}

QStringList PrintHandler::cupsOptions() const
{
	QStringList options;
	if (m_twoPagesRadio->isChecked())
		options << QLatin1String("-o number-up=2");
	if (m_fourPagesRadio->isChecked())
		options << QLatin1String("-o number-up=4");
	if (m_sixPagesRadio->isChecked())
		options << QLatin1String("-o number-up=6");
	if (m_ninePagesRadio->isChecked())
		options << QLatin1String("-o number-up=9");
	if (m_sixteenPagesRadio->isChecked())
		options << QLatin1String("-o number-up=16");
	switch (m_pagesFlowComboBox->currentIndex())
	{
		case 1: options << QLatin1String("-o number-up-layout=lrbt"); break;
		case 2: options << QLatin1String("-o number-up-layout=rltb"); break;
		case 3: options << QLatin1String("-o number-up-layout=rlbt"); break;
		case 4: options << QLatin1String("-o number-up-layout=btlr"); break;
		case 5: options << QLatin1String("-o number-up-layout=btrl"); break;
		case 6: options << QLatin1String("-o number-up-layout=tblr"); break;
		case 7: options << QLatin1String("-o number-up-layout=tbrl"); break;
		case 0: default: break;
	}

	return options;
}

/*******************************************************************/
// Print

void PrintHandler::print(Poppler::Document *popplerDocument, QList<Poppler::Page*> popplerPages, const QString &fileName, int pageNumber)
{
	if (!popplerDocument)
		return;

//	QPrinter printer(QPrinter::HighResolution);
	QPrinter printer(QPrinter::PrinterResolution);

	// choose printer
	QWidget *parentWidget = qobject_cast<QWidget*>(parent());
	QPointer<QPrintDialog> printDialog = new QPrintDialog(&printer, parentWidget);
	printDialog->setWindowTitle(tr("Print document"));
	printDialog->setOptions(printDialog->options() | QAbstractPrintDialog::PrintPageRange | QAbstractPrintDialog::PrintCurrentPage);
#ifndef Q_OS_WIN
	printDialog->setOptionTabs(QList<QWidget*>() << cupsOptionsPagesWidget());
#endif // Q_OS_WIN
	printDialog->setMinMax(0, popplerDocument->numPages());
	printDialog->setFromTo(0, popplerDocument->numPages());
	if (printDialog->exec() != QDialog::Accepted)
	{
		delete printDialog;
		return;
	}

	// get page range
	int startPage, endPage;
	if (printer.printRange() == QPrinter::PageRange)
	{
		startPage = printer.fromPage();
		endPage = printer.toPage();
	}
	else if (printer.printRange() == QPrinter::CurrentPage)
	{
		startPage = pageNumber;
		endPage = pageNumber;
	}
	else
	{
		startPage = 1;
		endPage = popplerDocument->numPages();
	}

	// print
	// TODO put this in separate thread
#ifdef Q_OS_WIN
	QPainter painter;
	painter.begin(&printer);
	for (int i = startPage; i <= endPage; ++i)
	{
		if (i != startPage)
			printer.newPage();

		Poppler::Page *popplerPage = popplerPages.at(i);
		if (popplerPage)
		{
			QImage image = popplerPage->renderToImage(printer.physicalDpiX(), printer.physicalDpiY()); // slow
			painter.drawImage(painter.window(), image, QRectF(0, 0, image.width(), image.height()));
		}
	}
	painter.end();
#else // Q_OS_WIN
	Q_UNUSED(popplerPages);
	if (!printer.printerName().isEmpty())
	{
		QStringList args;
		if (!printer.printerName().contains(QLatin1Char(' '))) {
			args << QString(QLatin1String("-d"));
			args << printer.printerName();
		}
		args << QString(QLatin1String("-n"));
		args << QString(QLatin1String("%1")).arg(printer.copyCount());
		args << QString(QLatin1String("-P"));
		args << QString(QLatin1String("%1-%2")).arg(startPage).arg(endPage);
		switch(printer.duplex())
		{
			case QPrinter::DuplexNone:
				args << QLatin1String("-o");
				args << QLatin1String("sides=one-sided");
				break;
			case QPrinter::DuplexShortSide:
				args << QLatin1String("-o");
				args << QLatin1String("sides=two-sided-short-edge");
				break;
			case QPrinter::DuplexLongSide:
				args << QLatin1String("-o");
				args << QLatin1String("sides=two-sided-long-edge");
				break;
			default:
				break;
		}
		args << cupsOptions();
		args << QLatin1String("--");
		args << QString(QLatin1String("%1")).arg(fileName);

//		QProcess::execute(PDFVIEWLIB_PRINT_PROGRAM, args);
		QProcess::startDetached(QString::fromLocal8Bit(PDFVIEWLIB_PRINT_PROGRAM), args);
	}
	else if (!printer.outputFileName().isEmpty())
	{
		QList<int> pageList;
		for (int i = startPage; i <= endPage; ++i)
			pageList << i;
		if (printer.outputFormat() == QPrinter::PdfFormat)
		{
			Poppler::PDFConverter *converter = popplerDocument->pdfConverter();
			converter->setOutputFileName(printer.outputFileName());
			if (!converter->convert())
				QMessageBox msgBox(QMessageBox::Critical, tr("Print Error"),
					tr("Cannot print to file:\n%1").arg(printer.outputFileName()),
					QMessageBox::Ok, parentWidget);
		}
		else if (printer.outputFormat() == QPrinter::PostScriptFormat)
		{
			// get the real page size to pass to the ps generator
			QPrinter dummy(QPrinter::PrinterResolution);
			dummy.setFullPage(true);
			dummy.setOrientation(printer.orientation());
			dummy.setPageSize(printer.pageSize());
			dummy.setPaperSize(printer.paperSize(QPrinter::Millimeter), QPrinter::Millimeter);

			Poppler::PSConverter *converter = popplerDocument->psConverter();
			converter->setOutputFileName(printer.outputFileName());
			converter->setPageList(pageList);
			converter->setPaperWidth(dummy.width());
			converter->setPaperHeight(dummy.height());
			if (!converter->convert())
				QMessageBox msgBox(QMessageBox::Critical, tr("Print Error"),
					tr("Cannot print to file:\n%1").arg(printer.outputFileName()),
					QMessageBox::Ok, parentWidget);
		}
	}
#endif // Q_OS_WIN
	delete printDialog; // must be done after cupsOptions()
}
