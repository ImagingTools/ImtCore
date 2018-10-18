#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtWidgets/QListView>
#include <QStandardItemModel>
#include <QStandardItem>


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <ibase/ICommandsProvider.h>
#include <iprm/ISelectionParam.h>
#include <iqtgui/CHierarchicalCommand.h>

#include <GeneratedFiles/imt3d/ui_CMatrixMenuGuiComp.h>


class QToolBar;


namespace imt3d
{


class CMatrixMenuGuiComp:
			public iqtgui::TDesignerGuiCompBase<Ui::CMatrixMenuGuiComp>,
			public virtual ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiCompBase<Ui::CMatrixMenuGuiComp> BaseClass;

	I_BEGIN_COMPONENT(CMatrixMenuGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		//I_ASSIGN(m_pagesWidgetCompPtr, "PageUiContainer", "UI component containing all application pages", true, "PageUiContainer");
		I_ASSIGN(m_pageModelCompPtr, "PageModel", "Data model describing the used pages", true, "PageModel");
		I_ASSIGN(m_commandsProviderCompPtr, "Commands", "Provider of the commands showed in the main tool bar", false, "Commands");
		I_ASSIGN_TO(m_commandsProviderModelCompPtr, m_commandsProviderCompPtr, false);
	I_END_COMPONENT;

	enum DataRole
	{
		DR_PAGE_ID = Qt::UserRole + 1
	};

	CMatrixMenuGuiComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const;

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();
	virtual void OnGuiRetranslate();

private Q_SLOTS:
	void on_listView_clicked(const QModelIndex& index);
	void on_HomeButton_clicked();

private:
	void CreateItems(const iprm::ISelectionParam* selectionPtr);
	QWidget* createOneMenuItem(const iprm::IOptionsList* optionsPtr, const int index);
	void GetMenuLayout(int& rows, int& columns, const int count);
	
	void UpdateCommands();

private:
	struct ItemInfo
	{
		ItemInfo()
			:selectionPtr(nullptr),
			pageIndex(-1)
		{
		}

		iprm::ISelectionParam* selectionPtr;
		int pageIndex;
	};

	typedef QMap<QStandardItem*, ItemInfo> ItemInfoMap;
	ItemInfoMap m_itemInfoMap;

	class CommandsObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		typedef imod::CMultiModelDispatcherBase BaseClass;

		explicit CommandsObserver(CMatrixMenuGuiComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet);

	private:
		CMatrixMenuGuiComp& m_parent;
	};

	CommandsObserver m_commandsObserver;

	QListView* m_menuPage;
	QStandardItemModel m_menuItemModel;

	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_commands;

	I_REF(iprm::ISelectionParam, m_pageModelCompPtr);
	I_REF(ibase::ICommandsProvider, m_commandsProviderCompPtr);
	I_REF(imod::IModel, m_commandsProviderModelCompPtr);

	QToolBar* m_mainToolBar;
};


} // namespace imt3d



