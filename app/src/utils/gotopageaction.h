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

#ifndef GOTOPAGEACTION_H
#define GOTOPAGEACTION_H

#include <QtGui/QWidgetAction>

class QLabel;
class QSpinBox;

class GoToPageAction : public QWidgetAction
{
	Q_OBJECT

public:
	explicit GoToPageAction(QObject *parent, const QString &name = 0);
	~GoToPageAction();

	void setValue(int value);
	void setMinimum(int minimum);
	void setMaximum(int maximum);
	void setFocus();

Q_SIGNALS:
	void valueChanged(int value);

private Q_SLOTS:
	void changeValue(int value);

private:
	QWidget *m_mainWidget;
	QSpinBox *m_pageSpinBox;
	QLabel *m_numPagesLabel;
};

#endif // GOTOPAGEACTION_H
