#include <imtgui/CThumbnailDecoratorGuiComp.h>


// Qt includes
#include <QDebug>
#include <QtWidgets/QToolbar>
#include <QtGui/QStandardItemModel>

// ACF includes
#include <iprm/IOptionsList.h>
#include <iqtgui/IMultiVisualStatusProvider.h>
#include <iqtgui/CCommandTools.h>


namespace imtgui
{


// public methods

CThumbnailDecoratorGuiComp::CThumbnailDecoratorGuiComp()
	:m_commands("&View", 100),
	m_mainToolBar(NULL),
	m_commandsObserver(*this)
{
	m_rootCommands.InsertChild(&m_commands);
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CThumbnailDecoratorGuiComp::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CThumbnailDecoratorGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_itemInfoMap.clear();

	if (m_pageModelCompPtr.IsValid()){
		CreateItems(m_pageModelCompPtr.GetPtr());

		pagesStack->setCurrentIndex(0);

		CurrentPageLabel->setText(QString::number(pagesStack->count()));

		if (m_pagesWidgetCompPtr.IsValid()){
			m_pagesWidgetCompPtr->CreateGui(ContentFrame);
		}
	}

	static const istd::IChangeable::ChangeSet commandsChangeSet(ibase::ICommandsProvider::CF_COMMANDS);

	if (m_commandsProviderModelCompPtr.IsValid()){
		m_commandsObserver.RegisterModel(m_commandsProviderModelCompPtr.GetPtr(), 0, commandsChangeSet);
	}
}


void CThumbnailDecoratorGuiComp::OnGuiDestroyed()
{
	if (m_pagesWidgetCompPtr.IsValid() && m_pagesWidgetCompPtr->IsGuiCreated()){
		m_pagesWidgetCompPtr->DestroyGui();
	}

	BaseClass::OnGuiDestroyed();
}


void CThumbnailDecoratorGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


// private slots


void CThumbnailDecoratorGuiComp::on_PageList_clicked(const QModelIndex& index)
{
	CurrentPageLabel->clear();
	const int pageCount = pagesStack->count();

	QStandardItem* item = m_menuItemModel.itemFromIndex(index);
	if (item == nullptr){
		on_HomeButton_clicked();

		return;
	}

	const QString itemName = item->text();
	CurrentPageLabel->setText(itemName);

	if (m_itemInfoMap.contains(item)){
		ItemInfo& info = m_itemInfoMap[item];
		if (info.selectionPtr != nullptr){
			info.selectionPtr->SetSelectedOptionIndex(info.pageIndex);
		}

		if (info.pageIndex < 0){
			pagesStack->setCurrentIndex(0);
		}
		else{
			pagesStack->setCurrentIndex(1);
		}
	}
}


void CThumbnailDecoratorGuiComp::on_HomeButton_clicked()
{
	pagesStack->setCurrentIndex(0);
}


// private methods

void CThumbnailDecoratorGuiComp::CreateItems(const iprm::ISelectionParam* selectionPtr)
{
	if (selectionPtr == nullptr){
		return;
	}

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(selectionPtr);

	const iprm::IOptionsList* itemsListPtr = selectionPtr->GetSelectionConstraints();
	int menuItemsCount = itemsListPtr->GetOptionsCount();


	int rowsCount = 0;
	int colsCount = 0;
	GetMenuLayout(rowsCount, colsCount, menuItemsCount);

	//Q_ASSERT((rowsCount * colsCount) == menuItemsCount);
	m_menuItemModel.setRowCount(rowsCount);
	m_menuItemModel.setColumnCount(colsCount);

	for (int itemIndex = 0; itemIndex < menuItemsCount; ++itemIndex){
		QString itemName = itemsListPtr->GetOptionName(itemIndex);
		if (itemName.isEmpty()){
			itemName = "<unnamed>";
		}
		QByteArray menuItemId = itemsListPtr->GetOptionId(itemIndex);

		QStandardItem* menuItem = new QStandardItem(itemName);
		menuItem->setData(menuItemId, DR_PAGE_ID);

		if (visualStatusProviderPtr != nullptr){
			const iqtgui::IVisualStatus* statusPtr = visualStatusProviderPtr->GetVisualStatus(itemIndex);
			if (statusPtr != nullptr){
				menuItem->setIcon(statusPtr->GetStatusIcon());
			}
		}

		ItemInfo itemInfo;
		itemInfo.pageIndex = itemIndex;
		itemInfo.selectionPtr = const_cast<iprm::ISelectionParam*>(selectionPtr);
		m_itemInfoMap[menuItem] = itemInfo;

		int itemRow = itemIndex / colsCount;
		int itemCol = itemIndex % colsCount;
		m_menuItemModel.setItem(itemIndex, menuItem);
	}

	if (m_menuItemModel.columnCount() > 0){
		PageList->setModel(&m_menuItemModel);
	}
}


void CThumbnailDecoratorGuiComp::GetMenuLayout(int& rows, int& columns, const int count)
{
	// fuck!
	rows = 1;
	columns = count;
	
	PageList->setResizeMode(QListView::ResizeMode::Adjust);
	QSize size = PageList->size();
	static const int spacing = 10;
	const QSize gridSize = QSize(size.width() / columns, size.height() / rows);
	const QSize iconSize = QSize(gridSize.width() - spacing * 2, gridSize.height() - spacing * 2);

	PageList->setViewMode(QListView::ViewMode::IconMode);
	PageList->setFlow(QListView::Flow::LeftToRight);
	PageList->setIconSize(iconSize);
	PageList->setGridSize(gridSize);
	PageList->setWrapping(true);
	PageList->setWordWrap(true);
}


void CThumbnailDecoratorGuiComp::UpdateCommands()
{
	const iqtgui::CHierarchicalCommand* commandsPtr = nullptr;
	if (m_commandsProviderCompPtr.IsValid()){
		if (m_mainToolBar != nullptr){
			m_mainToolBar->clear();
		}
	
		commandsPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(m_commandsProviderCompPtr->GetCommands());
		if (commandsPtr != nullptr){
			if (m_mainToolBar == nullptr){
				m_mainToolBar = new QToolBar(ToolbarFrame);
				ToolbarFrame->layout()->addWidget(m_mainToolBar);

				m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			}

			iqtgui::CCommandTools::SetupToolbar(*commandsPtr, *m_mainToolBar);
		}
	}
}


// public methods of embedded class CommandsObserver

CThumbnailDecoratorGuiComp::CommandsObserver::CommandsObserver(CThumbnailDecoratorGuiComp& parent)
:	m_parent(parent)
{
}


// protected methods of embedded class CommandsObserver

// reimplemented (imod::CMultiModelDispatcherBase)

void CThumbnailDecoratorGuiComp::CommandsObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdateCommands();
}


} // namespace imtgui


