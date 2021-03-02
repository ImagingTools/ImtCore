#include <imtlicgui/CLicenseInfoEditorGuiComp.h>


// Acf includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgui
{


// public methods

CLicenseInfoEditorGuiComp::CLicenseInfoEditorGuiComp()
	:m_collectionObserver(*this),
	m_itemChangedSignalBlockCounter(0),
	m_isGuiModelInitialized(false),
	m_isCollectionRepresentationInitialized(false)
{
}


// protected methods

void CLicenseInfoEditorGuiComp::OnFeaturePackageCollectionUpdate()
{
	imtbase::IObjectCollection* collectionPtr = m_collectionObserver.GetObservedObject();
	imtbase::ICollectionInfo::Ids packageIds = collectionPtr->GetElementIds();

	Features->clear();
	m_collectionRepresentation.clear();

	ItemChangedSignalBlocker blocker(&m_itemChangedSignalBlockCounter);

	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr packageDataPtr;
		if (collectionPtr->GetObjectData(packageId, packageDataPtr)){
			imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<imtlic::IFeatureInfoProvider*>(packageDataPtr.GetPtr());
			if (packagePtr != nullptr){
				QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({collectionPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString()});
				Features->addTopLevelItem(packageItemPtr);
				packageItemPtr->setData(0, DR_ITEM_ID, packageId);
				packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);

				imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureId : featureIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureId);
					if (featureInfoPtr != nullptr){
						QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({featureInfoPtr->GetFeatureName()});
						packageItemPtr->addChild(featureItemPtr);
						featureItemPtr->setData(0, DR_ITEM_ID, featureInfoPtr->GetFeatureId());
						featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);

						m_collectionRepresentation[packageId].append(featureInfoPtr->GetFeatureId());
					}
				}
			}
		}
	}

	m_isCollectionRepresentationInitialized = true;

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		RemoveMissingFeatures();
		UpdateTreeItemCheckStates();
		DoUpdateModel();
	}
}


void CLicenseInfoEditorGuiComp::UpdateTreeItemCheckStates()
{
	ItemChangedSignalBlocker blocker(&m_itemChangedSignalBlockCounter);

	QByteArrayList packageIds = m_collectionRepresentation.keys();
	for (const QByteArray& packageId : packageIds){
		QByteArrayList featureIds = m_collectionRepresentation[packageId];
		for (const QByteArray& featureId : featureIds){
			QTreeWidgetItem* featureItemPtr = GetItem(featureId);
			bool isFeaturePresent = m_featureIds.contains(featureId);
			featureItemPtr->setCheckState(0, isFeaturePresent ? Qt::Checked : Qt::Unchecked);
		}
	}

	for (const QByteArray& packageId : packageIds){
		bool isAllFeaturesChecked = true;
		bool isAllFeaturesUnchecked = true;

		QByteArrayList featureIds = m_collectionRepresentation[packageId];
		for (const QByteArray& featureId : featureIds){
			QTreeWidgetItem* featureItemPtr = GetItem(featureId);
			if (featureItemPtr->checkState(0) == Qt::Checked){
				isAllFeaturesUnchecked = false;
			}
			else if (featureItemPtr->checkState(0) == Qt::Unchecked){
				isAllFeaturesChecked = false;
			}

			QTreeWidgetItem* packageItemPtr = GetItem(packageId);
			if (isAllFeaturesChecked){
				packageItemPtr->setCheckState(0, Qt::Checked);
			}
			else if (isAllFeaturesUnchecked){
				packageItemPtr->setCheckState(0, Qt::Unchecked);
			}
			else{
				packageItemPtr->setCheckState(0, Qt::PartiallyChecked);
			}
		}
	}
}


void CLicenseInfoEditorGuiComp::RemoveMissingFeatures()
{
	QByteArrayList allFeatureIds;

	QByteArrayList packageIds = m_collectionRepresentation.keys();
	for (const QByteArray& packageId : packageIds){
		allFeatureIds.append(m_collectionRepresentation[packageId]);
	}

	QByteArrayList presentIds;

	for (const QByteArray& featureId : m_featureIds){
		if (allFeatureIds.contains(featureId)){
			presentIds.append(featureId);
		}
	}

	m_featureIds = presentIds;
}


QTreeWidgetItem* CLicenseInfoEditorGuiComp::GetItem(const QByteArray& itemId)
{
	int packageCount = Features->topLevelItemCount();
	for (int packageIndex = 0; packageIndex < packageCount; packageIndex++){
		QTreeWidgetItem* packageItemPtr = Features->topLevelItem(packageIndex);
		if (packageItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
			return packageItemPtr;
		}

		int featureCount = packageItemPtr->childCount();
		for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
			QTreeWidgetItem* featurteItemPtr = packageItemPtr->child(featureIndex);
			if (featurteItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
				return featurteItemPtr;
			}
		}
	}

	return nullptr;
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CLicenseInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	NameEdit->setText(licenseInfoPtr->GetLicenseName());
	IdEdit->setText(licenseInfoPtr->GetLicenseId());
	m_featureIds = licenseInfoPtr->GetFeatures();

	m_isGuiModelInitialized = true;

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		RemoveMissingFeatures();
		UpdateTreeItemCheckStates();
		DoUpdateModel();
	}
}


void CLicenseInfoEditorGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	m_featureIds = licenseInfoPtr->GetFeatures();

	imod::IModel* modelPtr = const_cast<imod::IModel*>(
				dynamic_cast<const imod::IModel*>(
							dynamic_cast<const imtbase::IObjectCollection*>(licenseInfoPtr->GetFeaturePackages())));
	if (modelPtr != nullptr){
		modelPtr->AttachObserver(&m_collectionObserver);
	}
}


void CLicenseInfoEditorGuiComp::OnGuiModelDetached()
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	imod::IModel* modelPtr = const_cast<imod::IModel*>(
				dynamic_cast<const imod::IModel*>(
					dynamic_cast<const imtbase::IObjectCollection*>(licenseInfoPtr->GetFeaturePackages())));
	if (modelPtr != nullptr){
		if (modelPtr->IsAttached(&m_collectionObserver)){
			modelPtr->DetachObserver(&m_collectionObserver);
		}
	}

	m_isGuiModelInitialized = false;
	m_isCollectionRepresentationInitialized = false;

	BaseClass::OnGuiModelDetached();
}


void CLicenseInfoEditorGuiComp::UpdateModel() const
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	istd::CChangeGroup changeGroup(licenseInfoPtr);

	licenseInfoPtr->SetLicenseName(NameEdit->text());
	licenseInfoPtr->SetLicenseId(IdEdit->text().toUtf8());
	licenseInfoPtr->SetFeatures(m_featureIds);
}


// private slots

void CLicenseInfoEditorGuiComp::on_NameEdit_editingFinished()
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_IdEdit_editingFinished()
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int column)
{
	Q_ASSERT(m_itemChangedSignalBlockCounter >= 0);

	if (m_itemChangedSignalBlockCounter > 0){
		return;
	}

	if (item->data(0, DR_ITEM_TYPE) == IT_PACKAGE){
		Qt::CheckState state = item->checkState(0);

		for (int i = 0; i < item->childCount(); i++){
			QTreeWidgetItem* featureItemPtr = item->child(i);
			QByteArray id = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();

			if (state == Qt::Checked){

				if (!m_featureIds.contains(id)){
					m_featureIds.append(id);
				}
			}
			else{
				if (m_featureIds.contains(id)){
					m_featureIds.removeOne(id);
				}
			}
		}
	}
	else{
		QByteArray id = item->data(0, DR_ITEM_ID).toByteArray();

		if (item->checkState(0) == Qt::Checked){
			if (!m_featureIds.contains(id)){
				m_featureIds.append(id);
			}
		}
		else{
			if (m_featureIds.contains(id)){
				m_featureIds.removeOne(id);
			}
		}
	}

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		RemoveMissingFeatures();
		UpdateTreeItemCheckStates();
		DoUpdateModel();
	}
}


// public methods of the embedded class FeaturePackageCollectionObserver

CLicenseInfoEditorGuiComp::FeaturePackageCollectionObserver::FeaturePackageCollectionObserver(CLicenseInfoEditorGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CLicenseInfoEditorGuiComp::FeaturePackageCollectionObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_parent.OnFeaturePackageCollectionUpdate();
}


} // namespace imtlicgui


