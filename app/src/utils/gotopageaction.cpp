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

#include "gotopageaction.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>

GoToPageAction::GoToPageAction(QObject *parent, const QString &name)
	: QWidgetAction(parent)
{
	if (!name.isEmpty())
		setObjectName(name);

	m_pageSpinBox = new QSpinBox;
	QLabel *separatorLabel = new QLabel(" / ");
	m_numPagesLabel = new QLabel;

	m_mainWidget = new QWidget;
	QHBoxLayout *mainLayout = new QHBoxLayout(m_mainWidget);
	mainLayout->addWidget(m_pageSpinBox);
	mainLayout->addWidget(separatorLabel);
	mainLayout->addWidget(m_numPagesLabel);

	setDefaultWidget(m_mainWidget);
	connect(m_pageSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeValue(int)));
}

GoToPageAction::~GoToPageAction()
{
	m_mainWidget->deleteLater();
}

void GoToPageAction::setValue(int value)
{
	m_pageSpinBox->setValue(value + m_pageSpinBox->minimum()); // users of GoToPageAction count from 0 to m_pageSpinBox->maximum() - m_pageSpinBox->minimum()
}

void GoToPageAction::setMinimum(int minimum)
{
	disconnect(m_pageSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeValue(int)));
	m_pageSpinBox->setMinimum(minimum);
	connect(m_pageSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeValue(int)));
}

void GoToPageAction::setMaximum(int maximum)
{
	m_pageSpinBox->setMaximum(maximum);
	m_numPagesLabel->setText(QString::number(maximum));
}

void GoToPageAction::changeValue(int value)
{
	emit valueChanged(value - m_pageSpinBox->minimum()); // users of GoToPageAction count from 0 to m_pageSpinBox->maximum() - m_pageSpinBox->minimum()
}

void GoToPageAction::setFocus()
{
	m_pageSpinBox->setFocus();
	m_pageSpinBox->selectAll();
}
