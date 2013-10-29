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

#include "shortcuthandler.h"

#ifndef QT_NO_SHORTCUT

#include <QtGui/QAction>
#include <QtCore/QSettings>
#include "shortcutconfigdialog.h"
#include "shortcutconfigwidget.h"

ShortcutHandler *ShortcutHandler::self = 0;

ShortcutHandler::ShortcutHandler(QWidget *parent)
	: QObject(parent)
	, m_settings(0)
	, m_shortcutConfigAction(0)
	, m_shortcutConfigDialog(0)
	, m_shortcutConfigWidget(0)
{
	Q_ASSERT_X(!self, "ShortcutHandler", "there should be only one shortcut handler object");
	ShortcutHandler::self = this;
	m_exclusivityGroups.append(QStringList() << tr("Main Window"));
}

ShortcutHandler::~ShortcutHandler()
{
	if (m_shortcutConfigAction)
		delete m_shortcutConfigAction;
	if (m_shortcutConfigDialog)
		delete m_shortcutConfigDialog;
	if (m_shortcutConfigWidget)
		delete m_shortcutConfigWidget;
}

void ShortcutHandler::setSettingsObject(QSettings *settings)
{
	m_settings = settings;
}

void ShortcutHandler::setExclusivityGroup(const QStringList &groups)
{
	if (groups.contains(tr("Main Window")))
	{
		m_exclusivityGroups.takeFirst();
		m_exclusivityGroups.prepend(groups);
	}
	else
		m_exclusivityGroups.append(groups);
}

QSettings *ShortcutHandler::settingsObject()
{
	return m_settings;
}

QAction *ShortcutHandler::action(const QString &actionName)
{
	for (int i = 0; i < m_actions.size(); ++i)
		if (m_actions.at(i)->objectName() == actionName)
			return m_actions.at(i);
	return 0;
}

void ShortcutHandler::addAction(QAction *action, const QString &parentId)
{
	Q_ASSERT_X(!action->objectName().isEmpty(), "ShortcutHandler", qPrintable("actions added to the shortcut handler should have an object name: " + action->text()));
	if (!m_actions.contains(action))
	{
		m_actions.append(action);
		m_defaultShortcuts.append(action->shortcut());
		m_parentIds.append(parentId);
	}
}

/***************************************************************************/

QAction *ShortcutHandler::shortcutConfigAction()
{
	Q_ASSERT_X(!m_shortcutConfigWidget, "ShortcutHandler", "a shortcut configuration dialog and a shortcut configuration widget cannot exist at the same time in one application");
	if (!m_shortcutConfigAction)
	{
		m_shortcutConfigAction = new QAction(tr("Configure S&hortcuts..."), qobject_cast<QWidget*>(parent()));
		QObject::connect(m_shortcutConfigAction, SIGNAL(triggered()), this, SLOT(openShortcutConfigDialog()));
	}
	return m_shortcutConfigAction;
}

void ShortcutHandler::openShortcutConfigDialog()
{
	if (!m_shortcutConfigDialog)
	{
		m_shortcutConfigDialog = new ShortcutConfigDialog(qobject_cast<QWidget*>(parent()));
		m_shortcutConfigDialog->setDefaultShortcuts(m_defaultShortcuts);
		m_shortcutConfigDialog->setActions(m_actions, m_parentIds);
		m_shortcutConfigDialog->setExclusivityGroups(m_exclusivityGroups);
	}
	m_shortcutConfigDialog->exec();
}

/***************************************************************************/

ShortcutConfigWidget *ShortcutHandler::configWidget()
{
	Q_ASSERT_X(!m_shortcutConfigAction, "ShortcutHandler", "a shortcut configuration dialog and a shortcut configuration widget cannot exist at the same time in one application");
	if (!m_shortcutConfigWidget)
	{
		m_shortcutConfigWidget = new ShortcutConfigWidget(qobject_cast<QWidget*>(parent()));
		m_shortcutConfigWidget->setDefaultShortcuts(m_defaultShortcuts);
		m_shortcutConfigWidget->setActions(m_actions, m_parentIds);
		m_shortcutConfigWidget->setExclusivityGroups(m_exclusivityGroups);
	}
	return m_shortcutConfigWidget;
}

void ShortcutHandler::accept()
{
	m_shortcutConfigWidget->accept();
}

void ShortcutHandler::reject()
{
	m_shortcutConfigWidget->reject();
}

/***************************************************************************/

void ShortcutHandler::readSettings()
{
	Q_ASSERT_X(m_settings, "ShortcutHandler", "a settings object should be set using setSettingsObject() prior to using readSettings()");
	m_settings->beginGroup("ShortcutHandler");
	const int size = m_settings->beginReadArray("Shortcuts");
	for (int i = 0; i < size; ++i)
	{
		m_settings->setArrayIndex(i);
		const QString actionName = m_settings->value("Action").toString();
//		QAction *action = qobject_cast<QWidget*>(parent())->findChild<QAction*>(actionName);
		// the following is not really optimal, maybe do something like the above (but make a list in addAction() of all parents of the actions)
		for (int j = 0; j < m_actions.size(); ++j)
			if (m_actions.at(j)->objectName() == actionName)
				m_actions.at(j)->setShortcut(QKeySequence(m_settings->value("Shortcut").toString()));
	}
	m_settings->endArray();
	m_settings->endGroup();
}

#endif // QT_NO_SHORTCUT
