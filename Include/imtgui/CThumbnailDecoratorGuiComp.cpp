#include <imtgui/CThumbnailDecoratorGuiComp.h>


// Qt includes
#include <QtWidgets/QToolbar>
#include <QtWidgets/QTableView>
#include <QtGui/QStandardItemModel>
#include <QFrame>

// ACF includes
#include <iprm/IOptionsList.h>
#include <iwidgets/iwidgets.h>
#include <iqtgui/IMultiVisualStatusProvider.h>
#include <iqtgui/CCommandTools.h>

//imtgui includes
#include <imtgui/CThumbpageItemGuiDelegate.h>


namespace imtgui
{


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
	m_verticalFrameMargin(6)
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

	static const istd::IChangeable::ChangeSet commandsChangeSet(ibase::ICommandsProvider::CF_COMMANDS);

	if (m_commandsProviderModelCompPtr.IsValid()) {
		m_commandsObserver.RegisterModel(m_commandsProviderModelCompPtr.GetPtr(), 0, commandsChangeSet);
	}

	if (m_pageModelCompPtr.IsValid()){
		CreateItems(m_pageModelCompPtr.GetPtr());
		pagesStack->setCurrentIndex(0);

		CurrentPageLabel->setText(QString::number(pagesStack->count()));

		if (m_pagesWidgetCompPtr.IsValid()){
			m_pagesWidgetCompPtr->CreateGui(ContentFrame);
		}

		m_pageModelCompPtr->SetSelectedOptionIndex(-1);
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
	bool pageExists = (item == nullptr) ? (false) : (item->data(CThumbpageItemGuiDelegate::DR_PAGE_ID).isValid());
	if (!pageExists){
		on_HomeButton_clicked();

		return;
	}

	const QString itemName = item->text();
	CurrentPageLabel->setText(itemName);

	if (m_itemInfoMap.contains(item)){
		ItemInfo& info = m_itemInfoMap[item];
		if (info.selectionPtr != nullptr){
			info.selectionPtr->SetSelectedOptionIndex(info.pageIndex);

			QLayout* subPageLayoutPtr = SubPageToolBarFrame->layout();
			Q_ASSERT(subPageLayoutPtr != NULL);

			iwidgets::ClearLayout(subPageLayoutPtr);

			const iprm::ISelectionParam* subSelectionPtr = info.selectionPtr->GetSubselection(info.pageIndex);
			if (subSelectionPtr != NULL){
				const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(subSelectionPtr);
				const iprm::IOptionsList* subPagesListPtr = subSelectionPtr->GetSelectionConstraints();
				if (subPagesListPtr != NULL){
					int subPagesCount = subPagesListPtr->GetOptionsCount();
					for (int subPageIndex = 0; subPageIndex < subPagesCount; ++subPageIndex){
						QString subPageName = subPagesListPtr->GetOptionName(subPageIndex);

						QToolButton* subPageButton = new QToolButton(SubPageToolBarFrame);
						subPageButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
						subPageButton->setText(subPageName);

						QIcon icon;
						const iqtgui::IVisualStatus* subPageStatusPtr = visualStatusProviderPtr->GetVisualStatus(subPageIndex);
						if (subPageStatusPtr != NULL){
							icon = subPageStatusPtr->GetStatusIcon();
						}

						subPageButton->setIcon(icon);

						subPageLayoutPtr->addWidget(subPageButton);
					}
				}
			}
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
	CurrentPageLabel->setText(tr("Home"));

	m_pageModelCompPtr->SetSelectedOptionIndex(-1);

	pagesStack->setCurrentIndex(0);
	PageList->clearSelection();
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

	//Q_ASSERT((rowsCount * colsCount) == menuItemsCount);
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
		PageList->setItemDelegate(new imtgui::CThumbpageItemGuiDelegate(m_menuItemModel, m_horizontalSpacing, m_verticalSpacing, this));
	}


	PageList->resizeColumnsToContents();
	PageList->resizeRowsToContents();

}


void CThumbnailDecoratorGuiComp::GetMenuLayout(const int count)
{
	m_columnsCount = m_horizontalItemsViewAttrPtr.IsValid() ? qMax(0, *m_horizontalItemsViewAttrPtr) : 0;
	m_rowsCount = m_verticalItemsViewAttrPtr.IsValid() ? qMax(0, *m_verticalItemsViewAttrPtr) : 0;

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
	else if (m_columnsCount <= 0) {
		m_columnsCount = count / m_rowsCount;
		if ((count % m_rowsCount) > 0) {
			m_columnsCount++;
		}
	}
}


void CThumbnailDecoratorGuiComp::SetLayoutProperties(const int count)
{
	QSize size = StartPage->size();
	if (m_columnsCount <= 0 || m_rowsCount <= 0){
		GetMenuLayout(count);
	}
	if (m_maxWidth < 0 || m_maxHeight <= 0){
		UpdateMaxSize();
	}
	if (m_verticalSpacing <= 0 || m_horizontalSpacing <= 0){
		UpdateSpacing();
	}
	UpdateMargins();

	static const int spacing = 10;
	const QSize gridSize = QSize(size.width() / m_columnsCount, size.height() / m_rowsCount);
	PageList->setIconSize(gridSize);
	PageList->setShowGrid(false);
	PageList->horizontalHeader()->hide();
	PageList->verticalHeader()->hide();
	PageList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	PageList->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void CThumbnailDecoratorGuiComp::UpdateSpacing()
{
	m_horizontalSpacing = m_horizontalSpacingAttrPtr.IsValid() ? qMax(0, *m_horizontalSpacingAttrPtr) : 15;
	m_verticalSpacing = m_verticalSpacingAttrPtr.IsValid() ? qMax(0, *m_verticalSpacingAttrPtr) : 15;
	//TODO check constraints
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
	m_maxWidth = m_maximumFrameWidthAttrPtr.IsValid() ? qMax(0, *m_maximumFrameWidthAttrPtr) : 800;
	m_maxHeight = m_maximumFrameHeightAttrPtr.IsValid() ? qMax(0, *m_maximumFrameHeightAttrPtr) : 600;

	PageList->setMaximumSize(QSize(m_maxWidth, m_maxHeight));
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


