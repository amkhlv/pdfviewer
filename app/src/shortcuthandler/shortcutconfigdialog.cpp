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

#include "shortcutconfigdialog.h"

#ifndef QT_NO_SHORTCUT

#include <QtGui/QDialogButtonBox>
#include "shortcutconfigwidget.h"

ShortcutConfigDialog::ShortcutConfigDialog(QWidget *parent)
	: QDialog(parent)
{
	setModal(true);
	setWindowTitle(tr("Configure Shortcuts") + " - " + QApplication::applicationName());

	m_shortcutConfigWidget = new ShortcutConfigWidget(this);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
	buttonBox->addButton(QDialogButtonBox::Ok);
	buttonBox->addButton(QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QWidget *buttonWidget = new QWidget(this);
	QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
	buttonLayout->addWidget(m_shortcutConfigWidget->clearButton());
	buttonLayout->addWidget(m_shortcutConfigWidget->useDefaultButton());
	buttonLayout->addStretch();
	buttonLayout->addWidget(buttonBox);
	buttonLayout->setContentsMargins(0, 0, 0, 0);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_shortcutConfigWidget);
	mainLayout->addWidget(buttonWidget);
	setLayout(mainLayout);
}

ShortcutConfigDialog::~ShortcutConfigDialog()
{
}

/***************************************************************************/

void ShortcutConfigDialog::setExclusivityGroups(const QList<QStringList> &groups)
{
	m_shortcutConfigWidget->setExclusivityGroups(groups);
}

void ShortcutConfigDialog::setActions(const QList<QAction*> &actions, const QStringList &parentIds)
{
	m_shortcutConfigWidget->setActions(actions, parentIds);
}

QList<QAction*> ShortcutConfigDialog::actions()
{
	return m_shortcutConfigWidget->actions();
}

void ShortcutConfigDialog::setDefaultShortcuts(const QList<QKeySequence> &shortcuts)
{
	m_shortcutConfigWidget->setDefaultShortcuts(shortcuts);
}

/***************************************************************************/

void ShortcutConfigDialog::accept()
{
	m_shortcutConfigWidget->accept();
	QDialog::accept();
}

void ShortcutConfigDialog::reject()
{
	m_shortcutConfigWidget->reject();
	QDialog::reject();
}

#endif // QT_NO_SHORTCUT
