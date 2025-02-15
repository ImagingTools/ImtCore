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

	void RegisterCommands(QWidget* parentWidgetPtr, const ibase::ICommandsProvider* commandsPtr);
	void UnregisterCommands();

protected:
	virtual void OnCommandsChanged(const istd::IChangeable::ChangeSet& changeSet, const ibase::ICommandsProvider* commandsPtr);

private:
	void UpdateCommands(const ibase::ICommandsProvider* commandsPtr);

private:
	imtbase::TModelUpdateBinder<ibase::ICommandsProvider, CCommandToolBar> m_pageCommandsObserver;

	QToolBar* m_commandsToolBar = nullptr;
	QWidget* m_parentWidgetPtr = nullptr;
};


} // namespace imtgui


