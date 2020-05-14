#pragma once


// Qt includes
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QSplitter>
#include <QtCore/QTimer>

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
#include <imtgui/ILayout.h>
#include <imtgui/CHierarchicalLayoutWidget.h>
#include <GeneratedFiles/imtgui/ui_CLayoutManagerGuiComp.h>


namespace imtgui
{


/**
	Layout editor.
*/
class CLayoutManagerGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<Ui::CLayoutManagerGuiComp, imtgui::ILayout>,
			public virtual ibase::ICommandsProvider
{
	Q_OBJECT

public:
 	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CLayoutManagerGuiComp, imtgui::ILayout> BaseClass;
	typedef QList<int> SizeList;

	I_BEGIN_COMPONENT(CLayoutManagerGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_SUBELEMENT(GuiViewOptionsList);
		I_REGISTER_SUBELEMENT_INTERFACE(GuiViewOptionsList, iprm::IOptionsList, ExtractGuiViewOptionList);
		I_ASSIGN(m_commandsProviderCompPtr, "UndoManagerCommands", "Undo manager commands providing commands functionality", false, "UndoManagerCommandsProvider");	
		I_ASSIGN(m_isShowBoxAttrPtr, "DefaultShowBox", "Show box", true, true);
		I_ASSIGN(m_isFixedLayoutAttrPtr, "DefaultFixedLayout", "Fixed layout", true, true);
		I_ASSIGN(m_colorBorderAttrPtr, "DefaultBorderColor", "Border color", true, "#808080");
		I_ASSIGN_MULTI_0(m_guiViewIdMultiAttrPtr, "ViewIds", "View ids to be used in layout creation", true);
		I_ASSIGN_MULTI_0(m_guiViewMultiFactCompPtr, "ViewFactories", "View factories to create gui in layout", true);
		I_ASSIGN_MULTI_0(m_guiViewNameMultiAttrPtr, "ViewNames", "View names will be shown to user in layout widget", true);
	I_END_COMPONENT

	CLayoutManagerGuiComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const;

protected:
	QLayout* CreateCustomLayoutWidget(ILayout* layout);

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet);

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiRetranslate(); 

protected Q_SLOTS:
	void OnSplitterMoved(int pos, int index);
	void OnSplitterMoveFinished();
	void OnStartEndEditCommand();
	void OnClearAll();
	void OnLoad();
	void OnSave();
	void OnChangeTitle(const QByteArray& id, const QString& title);
	void OnChangeAlignTitle(const QByteArray& id, const ILayout::AlignType& align);
	void OnChangeIcon(const QByteArray& id);
	void OnRemoveIcon(const QByteArray& id);
	void OnSplitLayout(const QByteArray& id, ILayout::LayoutType type, int width, int height);
	void OnDeleteWidget(const QByteArray& id);
	void OnAddWidget(const QByteArray& id, int index);
	void OnAddWidgetByViewId(const QByteArray& id, const QByteArray& viewId);
	void OnChangeSizes(const QByteArray& id, const SizeList& sizeList);
	void OnChangeProperties(const QByteArray& id, const ILayout::LayoutProperties& properties);

private:
	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractGuiViewOptionList(CLayoutManagerGuiComp& component)
	{
		return &component.m_guiViewOptionsManager;
	}

private:
	I_REF(ibase::ICommandsProvider, m_commandsProviderCompPtr);
	I_ATTR(bool, m_isFixedLayoutAttrPtr);
	I_ATTR(bool, m_isShowBoxAttrPtr);
	I_ATTR(QByteArray, m_colorBorderAttrPtr);
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

	iqtgui::CHierarchicalCommand* m_undoCommands;

	QTimer m_splitterTimer;
	QByteArray m_activeId;
	QMap<QSplitter*, QByteArray> SplittersMap;

	iprm::COptionsManager m_guiViewOptionsManager;
};


} // namespace iloggui


