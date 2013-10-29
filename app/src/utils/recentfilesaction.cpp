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

#include "recentfilesaction.h"

#include <QtCore/QSettings>
#include <QtGui/QIcon>
#include <QtGui/QMenu>

RecentFilesAction::RecentFilesAction(QObject *parent)
	: QAction(parent)
	, m_settings(0)
{
	init();
}

RecentFilesAction::RecentFilesAction(const QString &text, QObject *parent)
	: QAction(text, parent)
	, m_settings(0)
{
	init();
}

RecentFilesAction::RecentFilesAction(const QIcon &icon, const QString &text, QObject *parent)
	: QAction(icon, text, parent)
	, m_settings(0)
{
	init();
}

RecentFilesAction::~RecentFilesAction()
{
	delete m_recentMenu;
}

void RecentFilesAction::setSettingsObject(QSettings *settings)
{
	m_settings = settings;
}

void RecentFilesAction::init()
{
	m_numOfRecentFiles = 10; // is set correctly in loadEntries() which must be executed before anything else happens with the menu

	setObjectName("file_open_recent");
	setText(tr("Open &Recent"));

	m_recentMenu = new QMenu();
	setMenu(m_recentMenu);
}

void RecentFilesAction::openRecentFile()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
		emit fileSelected(action->data().toString());
}

void RecentFilesAction::createActions()
{
	m_recentFileActions.clear();
	QAction *action;
	for (int i = 0; i < m_numOfRecentFiles; ++i)
	{
		action = new QAction(this);
		action->setVisible(false);
		connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
		m_recentFileActions.append(action);
	}

	// when the user has decreased the maximum number of recent files, then we must remove the superfluous entries
	while (m_recentFilesList.size() > m_numOfRecentFiles)
		m_recentFilesList.removeLast();

	updateMenu();

	m_recentMenu->clear(); // this also deletes all the QActions in the menu
	m_recentMenu->addActions(m_recentFileActions);
}

void RecentFilesAction::loadEntries()
{
	Q_ASSERT_X(m_settings, "RecentFilesAction", "a settings object should be set using setSettingsObject() prior to using readSettings()");
	m_settings->beginGroup("RecentFiles");
	m_numOfRecentFiles = m_settings->value("Number", 10).toInt();
	m_recentFilesList = m_settings->value("Files").toStringList();
	m_settings->endGroup();
	createActions();
	setEnabled(true);
}

void RecentFilesAction::saveEntries()
{
	Q_ASSERT_X(m_settings, "RecentFilesAction", "a settings object should be set using setSettingsObject() prior to using readSettings()");
	m_settings->beginGroup("RecentFiles");
	if (m_recentFilesList.size() > 0)
		m_settings->setValue("Files", m_recentFilesList);
	m_settings->endGroup();
}

void RecentFilesAction::updateMenu()
{
	m_recentMenu->setEnabled(m_recentFilesList.count() > 0);

	for (int i = 0; i < m_recentFilesList.count(); ++i)
	{
		m_recentFileActions[i]->setText(m_recentFilesList.at(i));
		m_recentFileActions[i]->setData(m_recentFilesList.at(i));
		m_recentFileActions[i]->setVisible(true);
	}
	for (int i = m_recentFilesList.count(); i < m_numOfRecentFiles; ++i)
		m_recentFileActions[i]->setVisible(false);
}

void RecentFilesAction::addFile(const QString &fileName)
{
	if (m_recentFilesList.contains(fileName))
		m_recentFilesList.move(m_recentFilesList.indexOf(fileName), 0);
	else
	{
		if (m_recentFilesList.count() >= m_numOfRecentFiles)
			m_recentFilesList.removeLast();
		m_recentFilesList.prepend(fileName);
	}
	updateMenu();
}

void RecentFilesAction::removeFile(const QString &fileName)
{
	m_recentFilesList.removeAll(fileName);
	updateMenu();
}
