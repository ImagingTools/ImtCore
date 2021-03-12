#include <imtlicgui/CFeatureTreeGuiComp.h>


// Qt includes
#include <QtWidgets/QTreeWidgetItem>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>


namespace imtlicgui
{


// public methods


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureTreeGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Features->clear();

	imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	imtbase::ICollectionInfo::Ids packageCollectionIds = collectionPtr->GetElementIds();

	for (const QByteArray& packageCollectionId : packageCollectionIds){
		imtbase::IObjectCollection::DataPtr packageDataPtr;
		if (collectionPtr->GetObjectData(packageCollectionId, packageDataPtr)){
			imtlic::IFeaturePackage* packagePtr = dynamic_cast<imtlic::IFeaturePackage*>(packageDataPtr.GetPtr());
			if (packagePtr != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();

				if (!featureCollectionIds.isEmpty()){
					QString packageName = collectionPtr->GetElementInfo(packageCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString();
					// TODO: QString packageId = packagePtr->GetPackageId();

					QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({packageName});
					Features->addTopLevelItem(packageItemPtr);
					packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
					packageItemPtr->setData(0, DR_ITEM_ID, packageCollectionId);
					if (*m_showFeatureStatesAttrPtr){
						packageItemPtr->setFlags(packageItemPtr->flags() & (~Qt::ItemIsAutoTristate));
					}

					Features->addTopLevelItem(packageItemPtr);

					for (const QByteArray& featureCollectionId : featureCollectionIds){
						const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
						if (featureInfoPtr != nullptr){
							QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({featureInfoPtr->GetFeatureName()});
							packageItemPtr->addChild(featureItemPtr);
							featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
							featureItemPtr->setData(0, DR_ITEM_ID, featureInfoPtr->GetFeatureId());

							packageItemPtr->addChild(packageItemPtr);
						}
					}
				}
			}
		}
	}

	if (*m_showFeatureStatesAttrPtr){
		imtbase::IMultiSelection::Ids selectedIds;
			
		imtbase::IMultiSelection* selectionPtr = dynamic_cast<imtbase::IMultiSelection*>(GetObservedObject());
		if (selectionPtr != nullptr){
			selectedIds = selectionPtr->GetSelectedIds();
		}

		int groupCount = Features->topLevelItemCount();
		for (int groupIndex = 0; groupIndex < groupCount; groupIndex++){
			QTreeWidgetItem* groupItemPtr = Features->topLevelItem(groupIndex);

			int featureCount = groupItemPtr->childCount();
			for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
				QTreeWidgetItem* featureItemPtr = groupItemPtr->child(featureIndex);

				QByteArray featureId = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();
				bool isFeatureSelected = selectedIds.contains(featureId);
				featureItemPtr->setCheckState(0, isFeatureSelected ? Qt::Checked : Qt::Unchecked);
			}
		}
	}
}


void CFeatureTreeGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CFeatureTreeGuiComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeatureTreeGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	Features->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

	connect(this, &CFeatureTreeGuiComp::EmitFeatureTreeItemChanged, this, &CFeatureTreeGuiComp::OnFeatureTreeItemChanged, Qt::QueuedConnection);
}


void CFeatureTreeGuiComp::OnGuiDestroyed()
{
	disconnect(this, &CFeatureTreeGuiComp::EmitFeatureTreeItemChanged, this, &CFeatureTreeGuiComp::OnFeatureTreeItemChanged);

	BaseClass::OnGuiDestroyed();
}


// private methods

QTreeWidgetItem* CFeatureTreeGuiComp::FindItem(const QByteArray& itemId)
{
	int groupCount = Features->topLevelItemCount();
	for (int groupIndex = 0; groupIndex < groupCount; groupIndex++){
		QTreeWidgetItem* groupItemPtr = Features->topLevelItem(groupIndex);
		if (groupItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
			return groupItemPtr;
		}

		int featureCount = groupItemPtr->childCount();
		for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
			QTreeWidgetItem* featurteItemPtr = groupItemPtr->child(featureIndex);
			if (featurteItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
				return featurteItemPtr;
			}
		}
	}

	return nullptr;
}


// private slots

void CFeatureTreeGuiComp::OnFeatureTreeItemChanged(const QByteArray& itemId, bool isChecked)
{
	if (m_itemSelectionChangeDelegateCompPtr.IsValid()){
		m_itemSelectionChangeDelegateCompPtr->ItemSelectionChanged(itemId, isChecked);
	}
}


void CFeatureTreeGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int column)
{
	if (column == 0 && item->data(0, DR_ITEM_TYPE) == IT_FEATURE){
		Q_EMIT EmitFeatureTreeItemChanged(item->data(0, DR_ITEM_ID).toByteArray(), item->checkState(0) == Qt::Checked);
	}
}


} // namespace imtlicgui


