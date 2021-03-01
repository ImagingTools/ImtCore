#include <imtlicgui/CLicenseInfoEditorGuiComp.h>


// Acf includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgui
{


// public methods

CLicenseInfoEditorGuiComp::CLicenseInfoEditorGuiComp()
	:m_collectionObserver(*this)
{
}


// protected methods

void CLicenseInfoEditorGuiComp::OnFeaturePackageCollectionUpdate()
{
	imtbase::IObjectCollection* collectionPtr = m_collectionObserver.GetObservedObject();
	imtbase::ICollectionInfo::Ids packageIds = collectionPtr->GetElementIds();

	Features->clear();

	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr packageDataPtr;
		if (collectionPtr->GetObjectData(packageId, packageDataPtr)){
			imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<imtlic::IFeatureInfoProvider*>(packageDataPtr.GetPtr());
			if (packagePtr != nullptr){
				QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({collectionPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString()});
				Features->addTopLevelItem(packageItemPtr);
				packageItemPtr->setCheckState(0, Qt::PartiallyChecked);
				packageItemPtr->setData(0, ID_ITEM_ID, packageId);
				packageItemPtr->setData(0, ID_ITEM_TYPE, IT_PACKAGE);

				imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureId : featureIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureId);
					if (featureInfoPtr != nullptr){
						QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({featureInfoPtr->GetFeatureName()});
						packageItemPtr->addChild(featureItemPtr);
						featureItemPtr->setCheckState(0, Qt::Unchecked);
						featureItemPtr->setData(0, ID_ITEM_ID, featureId);
						featureItemPtr->setData(0, ID_ITEM_TYPE, IT_FEATURE);
					}
				}
			}
		}
	}

	SynchronizeFeatureItems();
}


void CLicenseInfoEditorGuiComp::SynchronizeFeatureItems()
{
	for (int i = 0; i < m_featureIds.count(); i++){
		QTreeWidgetItem* itemPtr = FindChildItem(m_featureIds[i]);
		if (itemPtr != nullptr){
			itemPtr->setCheckState(0, Qt::Checked);
		}
		else{
			m_featureIds.removeAt(i);
			i = -1;
		}
	}

	for (int packageIndex = 0; packageIndex < Features->topLevelItemCount(); packageIndex++){
		QTreeWidgetItem* packageItemPtr = Features->topLevelItem(packageIndex);
		//packageItemPtr->setCheckState(0, Qt::Checked);

		for (int featureIndex = 0; featureIndex < packageItemPtr->childCount(); featureIndex++){
			if (packageItemPtr->child(featureIndex)->checkState(0) == Qt::Unchecked){
				//packageItemPtr->setCheckState(0, Qt::Unchecked);
				break;
			}
		}
	}
}


QTreeWidgetItem* CLicenseInfoEditorGuiComp::FindChildItem(const QByteArray& featureId)
{
	int packageCount = Features->topLevelItemCount();
	for (int packageIndex = 0; packageIndex < packageCount; packageIndex++){
		QTreeWidgetItem* packageItemPtr = Features->topLevelItem(packageIndex);

		int featureCount = packageItemPtr->childCount();
		for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
			QTreeWidgetItem* featurteItemPtr = packageItemPtr->child(featureIndex);

			if (featurteItemPtr->data(0, ID_ITEM_ID).toByteArray() == featureId){
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

	SynchronizeFeatureItems();
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

	BaseClass::OnGuiModelDetached();
}


void CLicenseInfoEditorGuiComp::UpdateModel() const
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	istd::CChangeGroup changeGroup(licenseInfoPtr);

	licenseInfoPtr->SetLicenseName(NameEdit->text());
	licenseInfoPtr->SetLicenseId(IdEdit->text().toUtf8());
	licenseInfoPtr->GetFeatures();
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
	//if (item->checkState(0) == Qt::Checked){
	//	if ()
	//}
	//else if (item->checkState(0) == Qt::PartiallyChecked){
	//	{

	//}
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


