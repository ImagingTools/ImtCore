#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtWidgets/QListView>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <ibase/ICommandsProvider.h>
#include <iprm/ISelectionParam.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtgui/CThumbpageItemGuiDelegate.h>
#include <GeneratedFiles/imtgui/ui_CThumbnailDecoratorGuiComp.h>


class QToolBar;


namespace imtgui
{


class CThumbnailDecoratorGuiComp:
			public iqtgui::TDesignerGuiCompBase<Ui::CThumbnailDecoratorGuiComp>,
			public virtual ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiCompBase<Ui::CThumbnailDecoratorGuiComp> BaseClass;

	I_BEGIN_COMPONENT(CThumbnailDecoratorGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_pagesWidgetCompPtr, "PageUiContainer", "UI component containing all application pages", true, "PageUiContainer");
		I_ASSIGN(m_pageModelCompPtr, "PageModel", "Data model describing the used pages", true, "PageModel");
		I_ASSIGN(m_commandsProviderCompPtr, "Commands", "Provider of the commands showed in the main tool bar", false, "Commands");
		I_ASSIGN_TO(m_commandsProviderModelCompPtr, m_commandsProviderCompPtr, false);
		I_ASSIGN(m_horizontalItemsViewAttrPtr, "HorizontalItemsView", "Count of visual items in horizontal row", false, 1);
		I_ASSIGN(m_verticalItemsViewAttrPtr, "VerticalItemsView", "Count of visual items in vertical column", false, 1);
		I_ASSIGN(m_horizontalSpacingAttrPtr, "HorizontalSpacing", "Horizontal spacing of items on view", false, 6);
		I_ASSIGN(m_verticalSpacingAttrPtr, "VerticalSpacing", "Vertical spacing of items on view", false, 6);
		I_ASSIGN(m_horizontalFrameMarginAttrPtr, "HorizontalFrameMargin", "Horizontal (left&right) side margin of thumbnail frame", false, 6);
		I_ASSIGN(m_verticalFrameMarginAttrPtr, "VerticalFrameMargin", "Vertical (top&bottom) side margin of thumbnail frame", false, 6);
		I_ASSIGN(m_maximumFrameWidthAttrPtr, "MaximumFrameWidth", "Maximum width of thumbnail frame", false, 320);
		I_ASSIGN(m_maximumFrameHeightAttrPtr, "MaximumFrameHeight", "Maximum height of thumbnail frame", false, 240);
	I_END_COMPONENT;

	CThumbnailDecoratorGuiComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const;

	//reimplemented (QObject)
	virtual bool eventFilter(QObject *watched, QEvent *event);

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();
	virtual void OnGuiRetranslate();

private Q_SLOTS:
	void on_PageList_clicked(const QModelIndex& index);
	void on_FullScreenButton_toggled(bool isToggled);
	void on_PageTree_itemSelectionChanged();
	void on_HomeButton_clicked();

private:
	void CreateItems(const iprm::ISelectionParam* selectionPtr);
	void CreateMenu(const iprm::ISelectionParam* selectionPtr, QTreeWidgetItem* parentItemPtr);

	/**
		calculates layout of page thumbnails based on m_horizontalItemsViewAttrPtr and m_verticalItemsViewAttrPtr
		if the attributes are not set, or set incorrectly, the dimentions close to sqrt(totalcounr) are taken with
		the preferred vertical
	*/
	void GetMenuLayout(const int count);
	void SetLayoutProperties(const int count);
	void UpdateSettings(const int count);
	void UpdateSpacing();
	void UpdateMargins();
	void UpdateMaxSize();
	void UpdateMinSize();
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

	typedef QMap<QTreeWidgetItem*, ItemInfo> MenuItemInfoMap;
	MenuItemInfoMap m_menuItemInfoMap;

	class CommandsObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		typedef imod::CMultiModelDispatcherBase BaseClass;

		explicit CommandsObserver(CThumbnailDecoratorGuiComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet);

	private:
		CThumbnailDecoratorGuiComp& m_parent;
	};

	CommandsObserver m_commandsObserver;

	QStandardItemModel m_menuItemModel;

	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_commands;
	int m_columnsCount, m_rowsCount;
	int m_horizontalSpacing;
	int m_verticalSpacing;
	int m_horizontalFrameMargin;
	int m_verticalFrameMargin;
	int m_maxWidth;
	int m_maxHeight;
	QSize m_minItemSize;
	imtgui::CThumbpageItemGuiDelegate* m_itemDelegate;

	I_REF(iqtgui::IGuiObject, m_pagesWidgetCompPtr);
	I_REF(iprm::ISelectionParam, m_pageModelCompPtr);
	I_REF(ibase::ICommandsProvider, m_commandsProviderCompPtr);
	I_REF(imod::IModel, m_commandsProviderModelCompPtr);
	I_ATTR(int, m_horizontalItemsViewAttrPtr);
	I_ATTR(int, m_verticalItemsViewAttrPtr);
	I_ATTR(int, m_horizontalSpacingAttrPtr);
	I_ATTR(int, m_verticalSpacingAttrPtr);
	I_ATTR(int, m_horizontalFrameMarginAttrPtr);
	I_ATTR(int, m_verticalFrameMarginAttrPtr);
	I_ATTR(int, m_maximumFrameWidthAttrPtr);
	I_ATTR(int, m_maximumFrameHeightAttrPtr);

	QToolBar* m_mainToolBar;
};


} // namespace imtgui



