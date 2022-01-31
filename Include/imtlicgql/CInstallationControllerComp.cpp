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


imtbase::CTreeItemModel* CInstallationControllerComp::ListObjects(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		QByteArray productId = GetObjectIdFromInputParams(inputParams);

		QByteArrayList featureIds;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (productId != "" && m_objectCollectionCompPtr->GetObjectData(productId, dataPtr)){
			imtbase::IObjectCollection* licensePtr = dynamic_cast<imtbase::IObjectCollection*>(dataPtr.GetPtr());

			if (licensePtr != nullptr){
				QByteArrayList licenseCollectionIds = licensePtr->GetElementIds().toList();
				for (const QByteArray& licenseCollectionId : licenseCollectionIds){
					int itemIndex = itemsModel->InsertNewItem();
					QString licenseName = licensePtr->GetElementInfo(licenseCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString();
					QString licenseDescription = licensePtr->GetElementInfo(licenseCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
					itemsModel->SetData("Id", licenseCollectionId, itemIndex);
					itemsModel->SetData("Name", licenseName, itemIndex);
					itemsModel->SetData("Description", licenseDescription, itemIndex);
				}
			}
		}

		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("items", itemsModel);
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
			objectId = itemModel.GetData("Id").toByteArray();;
		}

		if (itemModel.ContainsKey("Name")) {
			name = itemModel.GetData("Name").toByteArray();;
		}

		QByteArray productId;
		if (itemModel.ContainsKey("ProductId")) {
			productId = itemModel.GetData("ProductId").toByteArray();;
		}

		QByteArray accountId;
		if (itemModel.ContainsKey("AccountId")) {
			accountId = itemModel.GetData("AccountId").toByteArray();;
		}

		productInstancePtr->SetupProductInstance(productId, objectId, accountId);

		imtbase::CTreeItemModel* activeLicenses = itemModel.GetTreeItemModel("ActiveLicenses");

		for (int i = 0; i < activeLicenses->GetItemsCount(); i++) {
			QByteArray licenseId;
			if (activeLicenses->ContainsKey("Id")) {
				licenseId = activeLicenses->GetData("Id", i).toByteArray();
			}

			QDateTime expirationDate;
			if (activeLicenses->ContainsKey("Expiration")) {
				QString dateExpirationStr = activeLicenses->GetData("Expiration", i).toString();
				expirationDate = QDateTime::fromString(dateExpirationStr, "dd.MM.yyyy");
				qDebug() << expirationDate.toString();
			}
			productInstancePtr->AddLicense(licenseId, expirationDate);
		}

		return productInstancePtr.PopPtr();
	}
	return nullptr;
}


} // namespace imtlicgql


