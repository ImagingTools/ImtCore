#include <imtlicgql/CInstallationControllerComp.h>

// ImtCore includes
#include <imtlic/CFeaturePackageCollectionUtility.h>
//#include <imtlic/CFeatureInfo.h>
//#include <imtlic/IFeaturePackage.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>

#include <imtlic/CLicenseInstance.h>
#include <imtlic/CLicenseInfo.h>

#include <imtlic/IProductLicensingInfo.h>

#include <imtlic/ILicenseInstance.h>
#include <imtlic/CFeaturePackage.h>

#include <imtbase/ICollectionInfo.h>


namespace imtlicgql
{


imtbase::CTreeItemModel* CInstallationControllerComp::GetObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemModel = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemModel = new imtbase::CTreeItemModel();

		QByteArray instanceId = GetObjectIdFromInputParams(inputParams);

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(instanceId, dataPtr)){

			imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());

			if (productInstancePtr == nullptr){
				errorMessage = "Unable to get an product instance";
				return nullptr;
			}

			QByteArray accountId = productInstancePtr->GetCustomerId();

			QByteArray productId = productInstancePtr->GetProductId();

			if (instanceId != ""){
				itemModel->SetData("Id", instanceId);
			}

			if (accountId != ""){
				itemModel->SetData("AccountId", accountId);
			}

			if (productId != ""){
				itemModel->SetData("ProductId", productId);
			}

			imtbase::CTreeItemModel* activeLicenses = itemModel->AddTreeModel("ActiveLicenses");
			const imtbase::ICollectionInfo& licenseInstances = productInstancePtr->GetLicenseInstances();
			imtbase::ICollectionInfo::Ids licenseIds = licenseInstances.GetElementIds();

			int index;

			for (const QByteArray& licenseCollectionId : licenseIds){
				const imtlic::ILicenseInstance* licenseInstancePtr = productInstancePtr->GetLicenseInstance(licenseCollectionId);

				if (licenseInstancePtr != nullptr){

					QDate date = licenseInstancePtr->GetExpiration().date();

					QString name = licenseInstancePtr->GetLicenseName();

					QString licenseExpirationText;

					index = activeLicenses->InsertNewItem();
					activeLicenses->SetData("Id", licenseCollectionId, index);
					activeLicenses->SetData("Name", name, index);

					if (date.isValid()){
						licenseExpirationText = date.toString(Qt::DateFormat::SystemLocaleDate);
						activeLicenses->SetData("Expiration", licenseExpirationText, index);
					}
					else{
						activeLicenses->SetData("Expiration", "Unlimited", index);
					}

				}
			}
		}

		dataModel->SetExternTreeModel("item", itemModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable* CInstallationControllerComp::CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray& objectId,
																	 QString& name, QString& description, QString &errorMessage) const
{
	if (inputParams.isEmpty()) {
		return nullptr;
	}

	if (!m_productInstanceFactCompPtr.IsValid()) {
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()) {
		istd::TDelPtr<imtlic::IProductInstanceInfo> productInstancePtr = m_productInstanceFactCompPtr.CreateInstance();

		if (!productInstancePtr.IsValid()) {
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		if (itemModel.ContainsKey("Id")) {
			objectId = itemModel.GetData("Id").toByteArray();
		}

		if (itemModel.ContainsKey("Name")) {
			name = itemModel.GetData("Name").toByteArray();
		}

		QByteArray productId;
		if (itemModel.ContainsKey("ProductId")) {
			productId = itemModel.GetData("ProductId").toByteArray();
		}

		QByteArray accountId;
		if (itemModel.ContainsKey("AccountId")) {
			accountId = itemModel.GetData("AccountId").toByteArray();
		}

		productInstancePtr->SetupProductInstance(productId, objectId, accountId);

		imtbase::CTreeItemModel* activeLicenses = itemModel.GetTreeItemModel("ActiveLicenses");
		int licenseState, expirationState = 0;

		for (int i = 0; i < activeLicenses->GetItemsCount(); i++) {
			QByteArray licenseId;
			if (activeLicenses->ContainsKey("Id")) {
				licenseId = activeLicenses->GetData("Id", i).toByteArray();
			}

			QDateTime expirationDate;

			if (activeLicenses->ContainsKey("LicenseState")){
				licenseState = activeLicenses->GetData("LicenseState", i).toInt();
			}

			if (licenseState == 2){

				if (activeLicenses->ContainsKey("ExpirationState")){
					expirationState = activeLicenses->GetData("ExpirationState", i).toInt();
				}

				if (expirationState == 2 && activeLicenses->ContainsKey("Expiration")) {
					QString dateExpirationStr = activeLicenses->GetData("Expiration", i).toString();
					expirationDate = QDateTime::fromString(dateExpirationStr, "dd.MM.yyyy");
				}

				productInstancePtr->AddLicense(licenseId, expirationDate);
			}

		}

		return productInstancePtr.PopPtr();
	}
	return nullptr;
}


} // namespace imtlicgql


