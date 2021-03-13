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
	:m_featureSelectionObserver(*this),
	m_featureStateObserver(*this)
{
}


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
						featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
						featureItemPtr->setData(0, DR_ITEM_ID, featureInfoPtr->GetFeatureId());

						packageItemPtr->addChild(featureItemPtr);

						if (*m_showFeatureStatesAttrPtr) {
							featureItemPtr->setFlags(featureItemPtr->flags() | Qt::ItemIsAutoTristate);
							featureItemPtr->setCheckState(0, Qt::Unchecked);
						}
					}
				}
			}
		}
	}

	Features->expandAll();
}


void CFeatureTreeGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_featureSelectionCompPtr.IsValid() && m_featureSelectionModelCompPtr.IsValid()){
		m_featureSelectionObserver.RegisterObject(
					m_featureSelectionCompPtr.GetPtr(),
					&CFeatureTreeGuiComp::OnFeatureSelectionChanged);
	}

	if (m_featureStateCompPtr.IsValid() && m_featureStateModelCompPtr.IsValid()){
		m_featureStateObserver.RegisterObject(
			m_featureStateCompPtr.GetPtr(),
			&CFeatureTreeGuiComp::OnFeatureStateChanged);
	}
}


void CFeatureTreeGuiComp::OnGuiModelDetached()
{
	m_featureStateObserver.UnregisterAllObjects();
	m_featureSelectionObserver.UnregisterAllObjects();

	BaseClass::OnGuiModelDetached();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeatureTreeGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	Features->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}


void CFeatureTreeGuiComp::OnGuiDestroyed()
{
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


void CFeatureTreeGuiComp::OnFeatureSelectionChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IMultiSelection* selectionPtr)
{
	QSignalBlocker blocker(Features);

	imtbase::IMultiSelection::Ids selectedIds = selectionPtr->GetSelectedIds();

	if (*m_showFeatureStatesAttrPtr){
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


void CFeatureTreeGuiComp::OnFeatureStateChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IMultiSelection* statePtr)
{
	QSignalBlocker blocker(Features);

	imtbase::IMultiSelection::Ids disabledIds = statePtr->GetSelectedIds();

	if (*m_showFeatureStatesAttrPtr){
		int groupCount = Features->topLevelItemCount();
		for (int groupIndex = 0; groupIndex < groupCount; groupIndex++){
			QTreeWidgetItem* groupItemPtr = Features->topLevelItem(groupIndex);

			int featureCount = groupItemPtr->childCount();
			for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
				QTreeWidgetItem* featureItemPtr = groupItemPtr->child(featureIndex);

				QByteArray featureId = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();
				if (disabledIds.contains(featureId)){
					featureItemPtr->setFlags(featureItemPtr->flags() & (~Qt::ItemIsEnabled));
				}
			}
		}
	}
}


// private slots

void CFeatureTreeGuiComp::OnFeatureItemStateChanged(const QByteArray& itemId, bool isChecked)
{
	if (!IsUpdateBlocked()){
		UpdateBlocker updateBlocker(this);

		if (m_featureItemStateHandlerCompPtr.IsValid()){
			m_featureItemStateHandlerCompPtr->OnItemStateChanged(itemId, isChecked);
		}
	}
}


void CFeatureTreeGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int column)
{
	if (column == 0 && item->data(0, DR_ITEM_TYPE) == IT_FEATURE){
		OnFeatureItemStateChanged(item->data(0, DR_ITEM_ID).toByteArray(), item->checkState(0) == Qt::Checked);
	}
}


} // namespace imtlicgui


