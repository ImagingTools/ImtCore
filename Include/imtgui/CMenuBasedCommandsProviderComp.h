#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/IVisualStatusProvider.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <iqtgui/CGuiComponentBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtgui/IWidgetProvider.h>


namespace imtgui
{


class CMenuBasedCommandsProviderComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CMenuBasedCommandsProviderComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_menuGuiCompPtr, "MenuGuiObject", "The gui object, provides menu", true, "MenuGuiObject");
		I_ASSIGN(m_menuAnchorProviderCompPtr, "MenuAnchorProvider", "The widget provider for positioning the GUI component", true, "MenuAnchorProvider");
		I_ASSIGN_TO(m_menuCommandVisualStatusProviderCompPtr, m_menuGuiCompPtr, true);
	I_END_COMPONENT;

	CMenuBasedCommandsProviderComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimpemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemStarted() override;
	virtual void OnSystemShutdown() override;

	void UpdateMenuCommandVisualStatus();

private Q_SLOTS:
	void OnCommandActivated(bool checked = false);


private:
	class MenuCommandVisualStatusObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		explicit MenuCommandVisualStatusObserver(CMenuBasedCommandsProviderComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CMenuBasedCommandsProviderComp& m_parent;
	};

private:
	I_REF(iqtgui::IGuiObject, m_menuGuiCompPtr);
	I_REF(imtgui::IWidgetProvider, m_menuAnchorProviderCompPtr);

	I_REF(iqtgui::IVisualStatusProvider, m_menuCommandVisualStatusProviderCompPtr);

	iqtgui::CHierarchicalCommand m_rootMenuCommand;
	iqtgui::CHierarchicalCommand m_mainMenuCommand;
	iqtgui::CHierarchicalCommand* m_menuCommand;
	QWidget* m_anchorButtonPtr;
	QWidget* m_topFramePtr;
	QWidget* m_mainWidgetPtr;
	MenuCommandVisualStatusObserver m_menuCommandVisualStatusObserver;
};


} // namespace imtgui


