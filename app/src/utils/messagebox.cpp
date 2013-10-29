/***************************************************************************
 *   Copyright (C) 2011, 2012 by Glad Deschrijver                          *
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

#include "messagebox.h"

#include <QtCore/QPointer>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>

int MessageBox::questionYesNo(QWidget *parent, const QString &text, const QString &caption, const QString &yesButtonText, const QString &noButtonText)
{
	QMessageBox::StandardButton result;
	if (!yesButtonText.isEmpty())
	{
		QPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Question, caption, text, QMessageBox::NoButton, parent);
		QPushButton *yesButton = msgBox->addButton(yesButtonText, QMessageBox::YesRole);
		if (!noButtonText.isEmpty())
			msgBox->addButton(noButtonText, QMessageBox::NoRole);
		else
			msgBox->addButton(QMessageBox::No);
		msgBox->setDefaultButton(yesButton);

		msgBox->exec();
		const int answer = (msgBox->clickedButton() == yesButton) ? Yes : No;
		delete msgBox;
		return answer;
	}
	else
		result = QMessageBox::question(parent, caption, text, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	return (result == QMessageBox::Yes) ? Yes : No;
}

void MessageBox::sorry(QWidget *parent, const QString &text, const QString &caption)
{
	QMessageBox::warning(parent, caption, text);
}

void MessageBox::error(QWidget *parent, const QString &text, const QString &caption)
{
	QMessageBox::critical(parent, caption, text);
}
