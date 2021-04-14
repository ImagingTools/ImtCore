#include <imtlicgui/CFeatureTreeGuiComp.h>


// Qt includes
#include <QtWidgets/QTreeWidgetItem>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>


namespace imtlicgui
{


// public methods

CFeatureTreeGuiComp::CFeatureTreeGuiComp()
	:m_blockItemChangedSignal(false),
	m_selectedItemPtr(nullptr)
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureTreeGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_blockItemChangedSignal = true;

	m_selectedItemPtr = nullptr;

	Features->clear();

	imtlicgui::IItemTree* rootItem = GetObservedObject();

	QList<IItemTree*> childs;
	for (int i = 0; i < rootItem->GetChildsCount(); i++){
		childs.append(dynamic_cast<IItemTree*>(rootItem->GetChild(i)));
	}

	CreteTreeItems(nullptr, childs);

	Features->expandAll();

	if (m_selectedItemPtr!= nullptr) {
		m_selectedItemPtr->setSelected(true);
		Features->setCurrentItem(m_selectedItemPtr);
	}

	m_blockItemChangedSignal = false;
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeatureTreeGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	Features->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	Features->setHeaderLabel(*m_headerLabelAttrPtr);
}


// private methods

void CFeatureTreeGuiComp::CreteTreeItems(QTreeWidgetItem* parentTreeItemPtr, QList<IItemTree*> items)
{
	int itemCountcount = items.count();
	for (int itemIndex = 0; itemIndex < itemCountcount; itemIndex++){
		QTreeWidgetItem* treeItemPtr = new QTreeWidgetItem({items[itemIndex]->GetName()});

		QByteArray itemId = items[itemIndex]->GetId();
		treeItemPtr->setData(0, Qt::UserRole, itemId);
		if (itemId == m_lastSelectedItemId) {
			m_selectedItemPtr = treeItemPtr;
		}

		if (*m_showFeatureStatesAttrPtr && items[itemIndex]->IsActivationEnabled()){
			treeItemPtr->setCheckState(0, items[itemIndex]->IsActivated() ? Qt::Checked : Qt::Unchecked);
		}

		if (!items[itemIndex]->IsEnabled()){
			treeItemPtr->setFlags(treeItemPtr->flags() & (~Qt::ItemIsEnabled));
		}

		if (parentTreeItemPtr != nullptr){
			parentTreeItemPtr->addChild(treeItemPtr);
		}
		else{
			Features->addTopLevelItem(treeItemPtr);
		}

		// Process childs items
		QList<IItemTree*> childs;

		int childCount = items[itemIndex]->GetChildsCount();
		for (int childIndex = 0; childIndex < childCount; childIndex++){
			IItemTree* itemTreePtr = dynamic_cast<IItemTree*>(items[itemIndex]->GetChild(childIndex));
			if (itemTreePtr != nullptr){
				childs.append(itemTreePtr);
			}
		}

		CreteTreeItems(treeItemPtr, childs);
	}
}


IItemTree* CFeatureTreeGuiComp::FindItem(IItemTree* fromItemPtr, const QByteArray& itemId) const
{
	if (fromItemPtr->GetId() == itemId){
		return fromItemPtr;
	}

	IItemTree* retVal = nullptr;

	int count = fromItemPtr->GetChildsCount();
	for (int i = 0; i < count; i++){
		IItemTree* itemPtr = dynamic_cast<IItemTree*>(fromItemPtr->GetChild(i));
		Q_ASSERT(itemPtr != nullptr);

		retVal = FindItem(itemPtr, itemId);
		if (retVal != nullptr){
			return retVal;
		}
	}

	return retVal;
}


// private slots

void CFeatureTreeGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int /*column*/)
{
	if (m_blockItemChangedSignal){
		return;
	}

	if (!IsUpdateBlocked()){
		UpdateBlocker blocker(this);

		QByteArray itemId = item->data(0, Qt::UserRole).toByteArray();
		IItemTree* itemPtr = FindItem(GetObservedObject(), itemId);
		if (itemPtr != nullptr){
			itemPtr->SetActivated(item->checkState(0) == Qt::Checked);
		}
	}
}


void CFeatureTreeGuiComp::on_Features_itemSelectionChanged()
{
	QList<QTreeWidgetItem*> items = Features->selectedItems();
	if (!items.isEmpty()){
		m_lastSelectedItemId = items[0]->data(0, Qt::UserRole).toByteArray();
	}
}


} // namespace imtlicgui


