#pragma once


// Qt includes
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>

// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <ibase/ICommandsProvider.h>
#include <iprm/COptionsManager.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <imod/IModel.h>
#include <idoc/IUndoManager.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtgui/ILayout.h>
#include <imtgui/CHierarchicalLayoutWidget.h>
#include <GeneratedFiles/imtgui/ui_CLayoutManagerComp.h>


namespace imtgui
{


/**
	Layout editor.
*/
class CLayoutManagerComp:
	public iqtgui::TDesignerGuiCompBase<Ui::CLayoutManagerComp>,
	public virtual iser::ISerializable,
//	protected imod::CSingleModelObserverBase,
 //   public iqtgui::TDesignerGuiObserverCompBase<
	//Ui::CLayoutManagerComp, imtgui::ILayout>,
	public virtual ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiCompBase<Ui::CLayoutManagerComp> BaseClass;


 	//typedef iqtgui::TDesignerGuiObserverCompBase<
		//Ui::CLayoutManagerComp, imtgui::ILayout> BaseClass;

	I_BEGIN_COMPONENT(CLayoutManagerComp);
	//I_REGISTER_INTERFACE(imod::IObserver);
	I_REGISTER_INTERFACE(ibase::ICommandsProvider);
	I_REGISTER_INTERFACE(iser::ISerializable);
	I_REGISTER_SUBELEMENT(GuiViewOptionsList);
	I_REGISTER_SUBELEMENT_INTERFACE(GuiViewOptionsList, iprm::IOptionsList, ExtractGuiViewOptionList);
	//I_ASSIGN(m_undoManagerCompFact, "UndoManager", "Undo manager providing undo functionality", false, "UndoManager");
	I_ASSIGN(m_commandsProviderCompPtr, "UndoManagerCommands", "Undo manager commands providing commands functionality", false, "UndoManagerCommandsProvider");	
	I_ASSIGN_MULTI_0(m_guiViewIdMultiAttrPtr, "ViewIds", "View ids to be used in layout creation", true);
	I_ASSIGN_MULTI_0(m_guiViewMultiFactCompPtr, "ViewFactories", "View factories to create gui in layout", true);
	// \todo think about icons for layout options widget
	I_ASSIGN_MULTI_0(m_guiViewNameMultiAttrPtr, "ViewNames", "View names will be shown to user in layout widget", true);

	I_END_COMPONENT

	CLayoutManagerComp(QWidget* parentPtr = Q_NULLPTR);

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

protected:

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet);

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiRetranslate(); 

protected Q_SLOTS:
	void OnDropWidget(QByteArray id, QDropEvent* eventPtr);
	void OnOpenMenu(QByteArray id, QMouseEvent* eventPtr);
	void OnClearWidget(QByteArray id);
	void OnStartEndEditCommand();
	void OnClearAll();
	void OnLoad();
	void OnSave();
	void OnChangeName();
	void OnSplitVertical();
	void OnSplitVertical(const QByteArray& id);
	void OnSplitHorizontal();
	void OnSplitHorizontal(const QByteArray& id);
	void OnDelete();
	void OnDeleteWidget(const QByteArray& id);
	void OnClear();
	void OnAddWidget();
	void OnAddWidget(const QByteArray& id, int index);
	void OnAddWidgetByViewId(const QByteArray& id, const QByteArray& viewId);

private:
	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractGuiViewOptionList(CLayoutManagerComp& component)
	{
		return &component.m_guiViewOptionsManager;
	}

private:
//	I_FACT(idoc::IUndoManager, m_undoManagerCompFact);
	I_REF(ibase::ICommandsProvider, m_commandsProviderCompPtr);
	I_MULTIATTR(QByteArray, m_guiViewIdMultiAttrPtr);
	I_MULTIFACT(iqtgui::IGuiObject, m_guiViewMultiFactCompPtr);
	I_MULTIATTR(QString, m_guiViewNameMultiAttrPtr);

	CHierarchicalLayoutWidget* m_layoutWidgetPtr;

	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_commands;
	iqtgui::CHierarchicalCommand m_startEndEditModeCommand;
	iqtgui::CHierarchicalCommand m_clearCommand;
	iqtgui::CHierarchicalCommand m_loadCommand;
	iqtgui::CHierarchicalCommand m_saveCommand;

	typedef istd::TSmartPtr<iqtgui::IGuiObject> GuiObjectDelPtr;
	typedef QMap<QByteArray, GuiObjectDelPtr> GuiObjectMap;
	GuiObjectMap m_createdViewMap;
	QByteArray m_activeId;

	iprm::COptionsManager m_guiViewOptionsManager;
};


} // namespace iloggui


