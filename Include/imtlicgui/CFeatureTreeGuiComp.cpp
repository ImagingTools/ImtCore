#include <imtlicgui/CFeatureTreeGuiComp.h>


// Qt includes
#include <QtWidgets/QTreeWidgetItem>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>


namespace imtlicgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureTreeGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Features->clear();

	imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	imtbase::ICollectionInfo::Ids packageCollectionIds = collectionPtr->GetElementIds();

	for (const QByteArray& packageCollectionId : packageCollectionIds){
		const istd::IChangeable* constObjectPtr = collectionPtr->GetObjectPtr(packageCollectionId);
		istd::IChangeable* objectPtr = const_cast<istd::IChangeable*>(constObjectPtr);
		imtlic::IFeaturePackage* packagePtr = dynamic_cast<imtlic::IFeaturePackage*>(objectPtr);

		if (packagePtr != nullptr){
			imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();

			if (!featureCollectionIds.isEmpty()){
				QString packageName = collectionPtr->GetElementInfo(packageCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString();
				// TODO: QByteArray packageId = packagePtr->GetPackageId();

				QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({packageName});
				Features->addTopLevelItem(packageItemPtr);
				packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
				packageItemPtr->setData(0, DR_ITEM_ID, packageCollectionId);
				if (*m_showFeatureStatesAttrPtr){
					packageItemPtr->setFlags(packageItemPtr->flags() | Qt::ItemIsAutoTristate);
					packageItemPtr->setCheckState(0, Qt::Unchecked);
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

	Features->expandAll();
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

	connect(this, &CFeatureTreeGuiComp::EmitFeatureItemStateChanged, this, &CFeatureTreeGuiComp::OnFeatureItemStateChanged, Qt::QueuedConnection);
}


void CFeatureTreeGuiComp::OnGuiDestroyed()
{
	disconnect(this, &CFeatureTreeGuiComp::EmitFeatureItemStateChanged, this, &CFeatureTreeGuiComp::OnFeatureItemStateChanged);

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

void CFeatureTreeGuiComp::OnFeatureItemStateChanged(const QByteArray& itemId, bool isChecked)
{
	if (m_featureItemStateHandlerCompPtr.IsValid()){
		m_featureItemStateHandlerCompPtr->OnItemStateChanged(itemId, isChecked);
	}
}


void CFeatureTreeGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int column)
{
	if (column == 0 && item->data(0, DR_ITEM_TYPE) == IT_FEATURE){
		Q_EMIT EmitFeatureItemStateChanged(item->data(0, DR_ITEM_ID).toByteArray(), item->checkState(0) == Qt::Checked);
	}
}


} // namespace imtlicgui


