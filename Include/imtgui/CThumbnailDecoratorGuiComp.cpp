#include <imtgui/CThumbnailDecoratorGuiComp.h>


// Qt includes
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QToolbar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QFrame>

// ACF includes
#include <iprm/IOptionsList.h>
#include <iwidgets/iwidgets.h>
#include <iqtgui/IMultiVisualStatusProvider.h>
#include <iqtgui/CCommandTools.h>

//imtgui includes
#include <imtgui/CThumbpageItemGuiDelegate.h>


namespace imtgui
{


static const int s_minSpacing = 10;


// public methods

CThumbnailDecoratorGuiComp::CThumbnailDecoratorGuiComp()
	:m_commands("&View", 100),
	m_mainToolBar(NULL),
	m_commandsObserver(*this),
	m_columnsCount(0),
	m_rowsCount(0),
	m_verticalSpacing(0),
	m_horizontalSpacing(0),
	m_horizontalFrameMargin(6),
	m_verticalFrameMargin(6),
	m_itemDelegate(nullptr)
{
	m_rootCommands.InsertChild(&m_commands);
	m_minItemSize = QSize(100, 50);
	m_maxWidth = 0;
	m_maxHeight = 0;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CThumbnailDecoratorGuiComp::GetCommands() const
{
	return &m_rootCommands;
}


// reimplemented (QOjbect)

bool CThumbnailDecoratorGuiComp::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::Resize){
		UpdateSpacing();
	}
	return BaseClass::eventFilter(watched, event);
}


// protected methods

// reimplemented (iqtgui::TRestorableGuiWrap)

void CThumbnailDecoratorGuiComp::OnRestoreSettings(const QSettings& settings)
{
	bool isFullscreen = settings.value("IsFullScreenEnabled").toBool();

	FullScreenButton->setChecked(isFullscreen);
}


void CThumbnailDecoratorGuiComp::OnSaveSettings(QSettings& settings) const
{
	bool isFullscreen = FullScreenButton->isChecked();

	settings.setValue("IsFullScreenEnabled", isFullscreen);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CThumbnailDecoratorGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	QWidget* widgetPtr = GetWidget();
	Q_ASSERT(widgetPtr != NULL);

	if (widgetPtr->isFullScreen()){
		FullScreenButton->setChecked(true);
	}

	m_itemInfoMap.clear();
	m_menuItemInfoMap.clear();

	LeftFrame->setVisible(false);

	static const istd::IChangeable::ChangeSet commandsChangeSet(ibase::ICommandsProvider::CF_COMMANDS);

	if (m_commandsProviderModelCompPtr.IsValid()){
		m_commandsObserver.RegisterModel(m_commandsProviderModelCompPtr.GetPtr(), 0, commandsChangeSet);
	}

	if (m_pageModelCompPtr.IsValid()){
		CreateItems(m_pageModelCompPtr.GetPtr());
		PageStack->setCurrentIndex(0);

		CurrentPageLabel->setText(tr("Home"));

		if (m_pagesWidgetCompPtr.IsValid()){
			m_pagesWidgetCompPtr->CreateGui(ContentFrame);
		}

		m_pageModelCompPtr->SetSelectedOptionIndex(-1);
	}

	installEventFilter(this);
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
	PageTree->clear();
	CurrentPageLabel->clear();

	int pageCount = PageStack->count();

	QStandardItem* item = m_menuItemModel.itemFromIndex(index);
	bool pageExists = (item == nullptr) ? (false) : (item->data(CThumbpageItemGuiDelegate::DR_PAGE_ID).isValid());
	if (!pageExists){
		on_HomeButton_clicked();

		return;
	}

	QString itemName = item->text();
	CurrentPageLabel->setText(itemName);

	if (m_itemInfoMap.contains(item)){
		ItemInfo& info = m_itemInfoMap[item];
		if (info.selectionPtr != nullptr){
			info.selectionPtr->SetSelectedOptionIndex(info.pageIndex);

			const iprm::ISelectionParam* subSelectionPtr = info.selectionPtr->GetSubselection(info.pageIndex);
			if (subSelectionPtr != NULL){
				CreateMenu(subSelectionPtr, NULL);
			}
		}

		if (info.pageIndex < 0){
			PageStack->setCurrentIndex(0);
		}
		else{
			PageStack->setCurrentIndex(1);
		}
	}
}


void CThumbnailDecoratorGuiComp::on_FullScreenButton_toggled(bool isToggled)
{
	if (!isToggled){
		GetWidget()->showNormal();
	}
	else{
		GetWidget()->showFullScreen();
	}
}


void CThumbnailDecoratorGuiComp::on_PageTree_itemSelectionChanged()
{
	CurrentPageLabel->clear();

	QList<QTreeWidgetItem*> selectedItems = PageTree->selectedItems();
	if (!selectedItems.isEmpty()){
		QTreeWidgetItem* selectedItemPtr = selectedItems[0];
		QTreeWidgetItem* parentItemPtr = selectedItemPtr->parent();

		while (parentItemPtr != nullptr){
			if (m_menuItemInfoMap.contains(parentItemPtr)){
				ItemInfo& info = m_menuItemInfoMap[parentItemPtr];

				if (info.selectionPtr != nullptr){
					info.selectionPtr->SetSelectedOptionIndex(info.pageIndex);
				}
			}

			parentItemPtr = parentItemPtr->parent();
		}

		if (m_menuItemInfoMap.contains(selectedItemPtr)){
			ItemInfo& info = m_menuItemInfoMap[selectedItemPtr];

			if (info.selectionPtr != nullptr){
				info.selectionPtr->SetSelectedOptionIndex(info.pageIndex);
			}
		}

		CurrentPageLabel->setText(selectedItemPtr->text(0));
	}
}


void CThumbnailDecoratorGuiComp::on_HomeButton_clicked()
{
	m_pageModelCompPtr->SetSelectedOptionIndex(-1);

	PageStack->setCurrentIndex(0);
	PageList->clearSelection();
	PageTree->clear();
	LeftFrame->setVisible(false);

	CurrentPageLabel->setText(tr("Home"));
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

	SetLayoutProperties(menuItemsCount);

	m_menuItemModel.setRowCount(m_rowsCount);
	m_menuItemModel.setColumnCount(m_columnsCount);
	m_menuItemModel.setParent(this);

	for (int itemIndex = 0; itemIndex < menuItemsCount; ++itemIndex){
		QString itemName = itemsListPtr->GetOptionName(itemIndex);
		if (itemName.isEmpty()){
			itemName = tr("<unnamed>");
		}
		QByteArray menuItemId = itemsListPtr->GetOptionId(itemIndex);

		QStandardItem* menuItem = new QStandardItem(itemName);
		menuItem->setData(menuItemId, CThumbpageItemGuiDelegate::DR_PAGE_ID);

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

		int itemRow = itemIndex / m_columnsCount;
		int itemCol = itemIndex % m_columnsCount;
		m_menuItemModel.setItem(itemRow, itemCol, menuItem);
	}

	if (m_menuItemModel.columnCount() > 0){
		PageList->setModel(&m_menuItemModel);
		m_itemDelegate = new imtgui::CThumbpageItemGuiDelegate(m_menuItemModel, m_horizontalSpacing, m_verticalSpacing, this);

		PageList->setItemDelegate(m_itemDelegate);
	}

	PageList->resizeColumnsToContents();
	PageList->resizeRowsToContents();

	UpdateSpacing();
	//UpdateMinSize();
}


void CThumbnailDecoratorGuiComp::CreateMenu(const iprm::ISelectionParam* selectionPtr, QTreeWidgetItem* parentItemPtr)
{
	PageTree->clear();

	if (selectionPtr == nullptr){
		return;
	}

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(selectionPtr);

	const iprm::IOptionsList* pageListPtr = selectionPtr->GetSelectionConstraints();
	int pagesCount = pageListPtr->GetOptionsCount();

	int selectedIndex = selectionPtr->GetSelectedOptionIndex();

	for (int pageIndex = 0; pageIndex < pagesCount; ++pageIndex){
		QString pageName = pageListPtr->GetOptionName(pageIndex);
		if (pageName.isEmpty()){
			continue;
		}

		QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

		QTreeWidgetItem* pageItem = new QTreeWidgetItem;
		pageItem->setText(0, pageName);
		pageItem->setData(0, CThumbpageItemGuiDelegate::DR_PAGE_ID, pageId);

		if (visualStatusProviderPtr != nullptr){
			const iqtgui::IVisualStatus* statusPtr = visualStatusProviderPtr->GetVisualStatus(pageIndex);
			if (statusPtr != nullptr){
				pageItem->setIcon(0, statusPtr->GetStatusIcon());
			}
		}

		ItemInfo itemInfo;
		itemInfo.pageIndex = pageIndex;
		itemInfo.selectionPtr = const_cast<iprm::ISelectionParam*>(selectionPtr);
		m_menuItemInfoMap[pageItem] = itemInfo;

		if (parentItemPtr == nullptr){
			PageTree->addTopLevelItem(pageItem);
		}
		else {
			parentItemPtr->addChild(pageItem);
		}

		if ((parentItemPtr == nullptr) && (pageIndex == selectedIndex)){
			pageItem->setSelected(true);
		}

		iprm::ISelectionParam* subSelectionPtr = selectionPtr->GetSubselection(pageIndex);
		if (subSelectionPtr != nullptr){
			CreateMenu(subSelectionPtr, pageItem);
		}
	}

	LeftFrame->setVisible(PageTree->topLevelItemCount() > 0);
}



void CThumbnailDecoratorGuiComp::GetMenuLayout(const int count)
{
	if (m_columnsCount <= 0 || m_rowsCount <= 0){
		m_columnsCount = m_horizontalItemsViewAttrPtr.IsValid() ? qMax(0, *m_horizontalItemsViewAttrPtr) : 0;
		m_rowsCount = m_verticalItemsViewAttrPtr.IsValid() ? qMax(0, *m_verticalItemsViewAttrPtr) : 0;
	}

	bool ifOneSet = (m_columnsCount > 0) || (m_rowsCount > 0);
	bool ifFits = ifOneSet && (m_columnsCount * m_rowsCount == count);

	if (m_columnsCount <= 0 && m_rowsCount <= 0 || (!ifOneSet && !ifFits)){
		int columns = (int)sqrt((double)count);
		m_columnsCount = qMax(1, columns);
		m_rowsCount = count / m_columnsCount;

		if ((count % m_columnsCount) > 0){
			m_rowsCount++;
		}
	}
	else if (m_rowsCount <= 0){
		m_rowsCount = count / m_columnsCount;
		if ((count % m_columnsCount) > 0){
			m_rowsCount++;
		}
	}
	else if (m_columnsCount <= 0){
		m_columnsCount = count / m_rowsCount;
		if ((count % m_rowsCount) > 0){
			m_columnsCount++;
		}
	}
}


void CThumbnailDecoratorGuiComp::SetLayoutProperties(const int count)
{
	QSize size = StartPage->size();
	UpdateSettings(count);

	static const int spacing = 10;
	const QSize gridSize = QSize(size.width() / m_columnsCount, size.height() / m_rowsCount);
	PageList->setIconSize(gridSize);
	PageList->setShowGrid(false);
	PageList->horizontalHeader()->hide();
	PageList->verticalHeader()->hide();
	PageList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	PageList->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void CThumbnailDecoratorGuiComp::UpdateSettings(const int count)
{
	GetMenuLayout(count);
	UpdateSpacing();
	UpdateMargins();
	UpdateMaxSize();
	//UpdateMinSize();
}


void CThumbnailDecoratorGuiComp::UpdateSpacing()
{
	m_horizontalSpacing = m_horizontalSpacingAttrPtr.IsValid() ? qMax(0, *m_horizontalSpacingAttrPtr) : s_minSpacing;
	m_verticalSpacing = m_verticalSpacingAttrPtr.IsValid() ? qMax(0, *m_verticalSpacingAttrPtr) : s_minSpacing;

	//check constraints
	QSize currentSize = PageList->size();

	if (m_itemDelegate != nullptr){
		QStyleOptionViewItem option;
		option.font = PageList->font();
		m_minItemSize = m_itemDelegate->sizeHint(option, QModelIndex());
	}

	int emptySpaceH = currentSize.width() - (m_columnsCount * m_minItemSize.width());
	int emptySpaceV = currentSize.height() - (m_rowsCount * m_minItemSize.height());

	int maxPossibleSpacingH = (m_columnsCount > 1) ? (float(emptySpaceH) / (float(m_columnsCount - 1))) : -1;
	int maxPossibleSpacingV = (m_rowsCount > 1) ? (float(emptySpaceV) / (float(m_rowsCount - 1))) : -1;

	m_horizontalSpacing = (m_horizontalSpacing > maxPossibleSpacingH && maxPossibleSpacingH > 0) ?
		maxPossibleSpacingH : m_horizontalSpacing;
	m_verticalSpacing = (m_verticalSpacing > maxPossibleSpacingV && maxPossibleSpacingV > 0) ?
		maxPossibleSpacingV : m_verticalSpacing;

	if (m_itemDelegate != nullptr){
		m_itemDelegate->SetMargins(m_horizontalSpacing/2, m_verticalSpacing/2);
	}
}


void CThumbnailDecoratorGuiComp::UpdateMargins()
{
	if (m_horizontalFrameMargin <= 0){
		m_horizontalFrameMargin = m_horizontalFrameMarginAttrPtr.IsValid() ? qMax(0, *m_horizontalFrameMarginAttrPtr) : 15;
		m_verticalFrameMargin = m_verticalFrameMarginAttrPtr.IsValid() ? qMax(0, *m_verticalFrameMarginAttrPtr) : 15;
	}
	//TODO check constraints
	hSpacerLeft->changeSize(m_horizontalFrameMargin, 20);
	hSpacerRight->changeSize(m_horizontalFrameMargin, 20);
	vSpacerTop->changeSize(20, m_verticalFrameMargin);
	vSpacerBottom->changeSize(20, m_verticalFrameMargin);
}


void CThumbnailDecoratorGuiComp::UpdateMaxSize()
{
	if (m_maxWidth <= 0 || m_maxHeight <= 0){
		m_maxWidth = m_maximumFrameWidthAttrPtr.IsValid() ? qMax(0, *m_maximumFrameWidthAttrPtr) : 800;
		m_maxHeight = m_maximumFrameHeightAttrPtr.IsValid() ? qMax(0, *m_maximumFrameHeightAttrPtr) : 600;
	}

	PageList->setMaximumSize(QSize(m_maxWidth, m_maxHeight));
}


/**
	does not perform plausibility check for variables. Should be called after all variables are set!
*/
void CThumbnailDecoratorGuiComp::UpdateMinSize()
{
	if (m_itemDelegate != nullptr){
		QStyleOptionViewItem option;
		option.font = PageList->font();
		m_minItemSize = m_itemDelegate->sizeHint(option, QModelIndex());
	}

	int minWidth = m_columnsCount * m_minItemSize.width() + (m_columnsCount - 1) * s_minSpacing;
	int minHeight = m_rowsCount * m_minItemSize.height() + (m_rowsCount - 1) * s_minSpacing;

	PageList->setMinimumSize(minWidth, minHeight);
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
				m_mainToolBar = new QToolBar(CurrentPageToolBarFrame);
				CurrentPageToolBarFrame->layout()->addWidget(m_mainToolBar);

				m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			}

			iqtgui::CCommandTools::SetupToolbar(*commandsPtr, *m_mainToolBar);
		}
	}
}


// public methods of embedded class CommandsObserver

CThumbnailDecoratorGuiComp::CommandsObserver::CommandsObserver(CThumbnailDecoratorGuiComp& parent)
	:m_parent(parent)
{
}


// protected methods of embedded class CommandsObserver

// reimplemented (imod::CMultiModelDispatcherBase)

void CThumbnailDecoratorGuiComp::CommandsObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdateCommands();
}


} // namespace imtgui


