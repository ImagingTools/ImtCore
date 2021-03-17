#include <imtlicgui/CFeatureTreeGuiComp.h>


// Qt includes
#include <QtWidgets/QTreeWidgetItem>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>


namespace imtlicgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureTreeGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	QSignalBlocker blocker(Features);

	Features->clear();

	imtlicgui::IItemTree* rootItem = GetObservedObject();

	QList<IItemTree*> childs;
	for (int i = 0; i < rootItem->GetChildsCount(); i++){
		childs.append(dynamic_cast<IItemTree*>(rootItem->GetChild(i)));
	}

	CreteTreeItems(nullptr, childs);

	Features->expandAll();
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
		treeItemPtr->setData(0, Qt::UserRole, items[itemIndex]->GetId());
		if (items[itemIndex]->GetChildsCount() > 0){
			treeItemPtr->setFlags(treeItemPtr->flags() | Qt::ItemIsAutoTristate);
		}

		if (!items[itemIndex]->IsEnabled()){
			treeItemPtr->setFlags(treeItemPtr->flags() & (~Qt::ItemIsEnabled));
		}

		if (*m_showFeatureStatesAttrPtr){
			treeItemPtr->setCheckState(0, items[itemIndex]->IsActivated() ? Qt::Checked : Qt::Unchecked);
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


// private slots

void CFeatureTreeGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int column)
{
	//Q_EMIT EmitItemChanged(item->data(0, DR_ITEM_ID).toByteArray(), item->checkState(0) == Qt::Checked);
}


} // namespace imtlicgui


