/***************************************************************************
 *   Copyright (C) 2009, 2012 by Glad Deschrijver                          *
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

#ifndef RECENTFILESACTION_H
#define RECENTFILESACTION_H

#include <QtGui/QAction>

class QIcon;
class QMenu;
class QSettings;

class RecentFilesAction : public QAction
{
	Q_OBJECT

public:
	RecentFilesAction(QObject *parent);
	RecentFilesAction(const QString &text, QObject *parent);
	RecentFilesAction(const QIcon &icon, const QString &text, QObject *parent);
	~RecentFilesAction();

	void setSettingsObject(QSettings *settings);
	void loadEntries();
	void saveEntries();
	void addFile(const QString &fileName);
	void removeFile(const QString &fileName);

Q_SIGNALS:
	void fileSelected(const QString &fileName);

private Q_SLOTS:
	void openRecentFile();

private:
	void init();
	void createActions();
	void updateMenu();

	QMenu *m_recentMenu;
	QList<QAction*> m_recentFileActions;
	QStringList m_recentFilesList;
	int m_numOfRecentFiles;
	QSettings *m_settings;
};

#endif // RECENTFILESACTION_H
