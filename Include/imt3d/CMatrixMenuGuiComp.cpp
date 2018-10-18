#include <imt3d/CMatrixMenuGuiComp.h>


// Qt includes
#include <QDebug>
#include <QtWidgets/QToolbar>
#include <QtGui/QStandardItemModel>

// ACF includes
#include <iprm/IOptionsList.h>
#include <iqtgui/IMultiVisualStatusProvider.h>
#include <iqtgui/CCommandTools.h>


namespace imt3d
{


// public methods

CMatrixMenuGuiComp::CMatrixMenuGuiComp()
	:m_commands("&View", 100),
	m_mainToolBar(NULL),
	m_commandsObserver(*this)
{
	m_rootCommands.InsertChild(&m_commands);
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CMatrixMenuGuiComp::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CMatrixMenuGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_itemInfoMap.clear();

	if (m_pageModelCompPtr.IsValid()){
		CreateItems(m_pageModelCompPtr.GetPtr());

		if (pagesStack->count() > 0){
			pagesStack->setCurrentIndex(0);
			CurrentPageLabel->setText(QString::number(pagesStack->count()));
		}
	}

	static const istd::IChangeable::ChangeSet commandsChangeSet(ibase::ICommandsProvider::CF_COMMANDS);

	if (m_commandsProviderModelCompPtr.IsValid()){
		m_commandsObserver.RegisterModel(m_commandsProviderModelCompPtr.GetPtr(), 0, commandsChangeSet);
	}
}


void CMatrixMenuGuiComp::OnGuiDestroyed()
{


	BaseClass::OnGuiDestroyed();
}


void CMatrixMenuGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


// private slots


void CMatrixMenuGuiComp::on_listView_clicked(const QModelIndex& index)
{
	CurrentPageLabel->clear();
	const int pageCount = pagesStack->count();

	QStandardItem* item = m_menuItemModel.itemFromIndex(index);
	if (item == nullptr){
		qDebug() << "item doesnt exist " << index;

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
		if (info.pageIndex < pageCount){
			pagesStack->setCurrentIndex(info.pageIndex);
		}
	}
	else{
		qDebug() << "item is not in the list. could this be?";
	}
}


void CMatrixMenuGuiComp::on_HomeButton_clicked()
{
	if (pagesStack->count() > 0){
		pagesStack->setCurrentIndex(0);
	}
}

// private methods

void CMatrixMenuGuiComp::CreateItems(const iprm::ISelectionParam* selectionPtr)
{
	if (selectionPtr == nullptr){
		return;
	}

	m_menuPage = listView;
	
	int selectedIndex = selectionPtr->GetSelectedOptionIndex();

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(selectionPtr);

	const iprm::IOptionsList* itemsListPtr = selectionPtr->GetSelectionConstraints();
	int menuItemsCount = itemsListPtr->GetOptionsCount();


	int rowsCount = 0;
	int colsCount = 0;
	GetMenuLayout(rowsCount, colsCount, menuItemsCount);

	for (int itemIndex = 0; itemIndex < menuItemsCount; ++itemIndex){
		QString itemName = itemsListPtr->GetOptionName(itemIndex);
		if (itemName.isEmpty()){
			itemName = "<unnamed>";
		}
		QByteArray menuItemId = itemsListPtr->GetOptionId(itemIndex);

		QStandardItem* menuItem = new QStandardItem(itemName);
		menuItem->setData(menuItemId, DR_PAGE_ID);

		if (visualStatusProviderPtr != nullptr) {
			const iqtgui::IVisualStatus* statusPtr = visualStatusProviderPtr->GetVisualStatus(itemIndex);
			if (statusPtr != nullptr) {
				menuItem->setIcon(statusPtr->GetStatusIcon());
			}
		}

		ItemInfo itemInfo;
		itemInfo.pageIndex = itemIndex;
		itemInfo.selectionPtr = const_cast<iprm::ISelectionParam*>(selectionPtr);
		m_itemInfoMap[menuItem] = itemInfo;

		int itemRow = itemIndex / colsCount;
		int itemCol = itemIndex % colsCount;
		m_menuItemModel.setItem(itemRow, itemCol, menuItem);

		/* add corresponding page to stackedWidget*/
		createOneMenuItem(itemsListPtr, itemIndex);

	}

	if (m_menuItemModel.columnCount() > 0){
		m_menuPage->setModel(&m_menuItemModel);
	}
}


QWidget* CMatrixMenuGuiComp::createOneMenuItem(const iprm::IOptionsList* optionsPtr, const int index)
{
	if (optionsPtr == nullptr) {
		return nullptr;
	}

	if (index < 0 || index > optionsPtr->GetOptionsCount()-1){
		return nullptr;
	}

	QWidget* menuItemPage = new QWidget(pagesStack);
	pagesStack->addWidget(menuItemPage);
	//set text(menuName)
	//set data (menuId)
	
}


void CMatrixMenuGuiComp::GetMenuLayout(int& rows, int& columns, const int count)
{
	// todo
	rows = 2;
	columns = 2;
	
	m_menuPage->setResizeMode(QListView::ResizeMode::Adjust);
	QSize size = m_menuPage->size();
	static const int spacing = 10;
	const QSize gridSize = QSize(size.width() / columns, size.height() / rows);
	const QSize iconSize = QSize(gridSize.width() - spacing * 2, gridSize.height() - spacing * 2);

	m_menuPage->setViewMode(QListView::ViewMode::IconMode);
	m_menuPage->setIconSize(iconSize);
	m_menuPage->setGridSize(gridSize);
}


void CMatrixMenuGuiComp::UpdateCommands()
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

CMatrixMenuGuiComp::CommandsObserver::CommandsObserver(CMatrixMenuGuiComp& parent)
:	m_parent(parent)
{
}


// protected methods of embedded class CommandsObserver

// reimplemented (imod::CMultiModelDispatcherBase)

void CMatrixMenuGuiComp::CommandsObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdateCommands();
}


} // namespace imt3d


