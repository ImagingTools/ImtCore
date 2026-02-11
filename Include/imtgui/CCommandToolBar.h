// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QToolBar>

// ACF includes
#include <ibase/ICommandsProvider.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>


namespace imtgui
{


class CCommandToolBar: public QObject
{
public:
	typedef QObject BaseClass;

	CCommandToolBar();

	void RegisterCommands(QWidget* parentWidgetPtr, const ibase::ICommandsProvider* commandsPtr, int commandGroupId = -1);
	void UnregisterCommands();

	QToolBar* GetToolBar() const;

protected:
	virtual void OnCommandsChanged(const istd::IChangeable::ChangeSet& changeSet, const ibase::ICommandsProvider* commandsPtr);

private:
	void UpdateCommands(const ibase::ICommandsProvider* commandsPtr);
	const ibase::IHierarchicalCommand* FindCommand(const ibase::IHierarchicalCommand* commandPtr, int groupId);

private:
	imtbase::TModelUpdateBinder<ibase::ICommandsProvider, CCommandToolBar> m_pageCommandsObserver;

	QToolBar* m_commandsToolBar = nullptr;
	QWidget* m_parentWidgetPtr = nullptr;

	int m_groupId = -1;
};


} // namespace imtgui


