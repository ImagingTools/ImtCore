#include <imtlicgui/CProductInstanceInfoViewComp.h>


// Qt includes
#include <QtWidgets/QTreeWidgetItem>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/ILicenseInstance.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoViewComp::CProductInstanceInfoViewComp()
{
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CProductInstanceInfoViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	UpdateProductName();
	UpdateFeatureTree();

	ProductInstanceIdEdit->setText(productInstanceInfoPtr->GetProductInstanceId());
	CustomerNameEdit->setText(productInstanceInfoPtr->GetCustomerId());
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductInstanceInfoViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	FeatureTree->header()->setSectionResizeMode(QHeaderView::Stretch);
}


void CProductInstanceInfoViewComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private methods

void CProductInstanceInfoViewComp::UpdateProductName()
{
	ProductNameEdit->clear();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	QString selectedProductId = productInstanceInfoPtr->GetProductId();
	ProductNameEdit->setText(qPrintable(selectedProductId));

	const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
	if (productsCollectionPtr != nullptr){
		const imtbase::IObjectCollectionInfo::Ids productIds = productsCollectionPtr->GetElementIds();
		for (const QByteArray& productId : productIds){
			QString productName = productsCollectionPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();

			ProductNameEdit->setText(productName);
		}
	}
}


void CProductInstanceInfoViewComp::UpdateFeatureTree()
{
	FeatureTree->clear();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	const imtbase::ICollectionInfo& licenseList = productInstanceInfoPtr->GetLicenseInstances();

	const imtbase::IObjectCollectionInfo::Ids allInstanceLicenseIds = licenseList.GetElementIds();
	for (const QByteArray& licenseCollectionId : allInstanceLicenseIds){
		const imtlic::ILicenseInstance* licenseInstancePtr = productInstanceInfoPtr->GetLicenseInstance(licenseCollectionId);
		if (licenseInstancePtr != nullptr){
			QString licenseName = licenseInstancePtr->GetLicenseName();
			QByteArray licenseId = licenseInstancePtr->GetLicenseId();

			licenseName += " (" + licenseId + ")";
			QString licenseNameText = QString(licenseName);

			QString licenseExpirationText;

			QDate date = licenseInstancePtr->GetExpiration().date();
			if (date.isValid()){
				licenseExpirationText = licenseInstancePtr->GetExpiration().date().toString(Qt::DateFormat::SystemLocaleDate);
			}
			else{
				licenseExpirationText = tr("Unlimited");
			}

			imtlic::ILicenseInstance::FeatureInfos features = licenseInstancePtr->GetFeatureInfos();
			for (const imtlic::ILicenseInstance::FeatureInfo& feature : features){
				QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem();

				featureItemPtr->setText(0, feature.name + " (" + feature.id + ")");
				featureItemPtr->setText(0, feature.name + " (" + feature.id + ")");
				featureItemPtr->setText(1, licenseNameText);
				featureItemPtr->setText(2, licenseExpirationText);

				FeatureTree->addTopLevelItem(featureItemPtr);
			}
		}
	}
}


} // namespace imtlicgui


