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

#ifndef SHORTCUTCONFIGDIALOG_H
#define SHORTCUTCONFIGDIALOG_H

#ifndef QT_NO_SHORTCUT

#include <QtGui/QDialog>

class QAction;
class QKeySequence;
class ShortcutConfigWidget;

/**
 * Application programmers are not supposed to access this class directly.
 * \see ShortcutHandler
 */
class ShortcutConfigDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ShortcutConfigDialog(QWidget *parent = 0);
	~ShortcutConfigDialog();

	void setExclusivityGroups(const QList<QStringList> &groups);
	void setActions(const QList<QAction*> &actions, const QStringList &m_parentIds);
	QList<QAction*> actions();
	void setDefaultShortcuts(const QList<QKeySequence> &shortcuts);

protected Q_SLOTS:
	void accept();
	void reject();

private:
	ShortcutConfigWidget *m_shortcutConfigWidget;
};

#endif // QT_NO_SHORTCUT

#endif // SHORTCUTCONFIGDIALOG_H
