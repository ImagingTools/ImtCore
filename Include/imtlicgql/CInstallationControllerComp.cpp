#include <imtlicgql/CInstallationControllerComp.h>

// ImtCore includes
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtlic/CLicenseInstance.h>
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

		QByteArray objectId = GetObjectIdFromInputParams(inputParams);

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
			if (productInstancePtr == nullptr){
				errorMessage = QT_TR_NOOP("Unable to get an product instance");
				return nullptr;
			}

			QByteArray instanceId = objectId;

			if (m_separatorObjectIdAttrPtr.IsValid()){
				QString objectIdStr = objectId;
				QStringList splitData = objectIdStr.split(*m_separatorObjectIdAttrPtr);
				instanceId = splitData[0].toUtf8();
			}

			QByteArray accountId = productInstancePtr->GetCustomerId();
			QByteArray productId = productInstancePtr->GetProductId();

			itemModel->SetData("Id", instanceId);
			itemModel->SetData("AccountId", accountId);
			itemModel->SetData("ProductId", productId);

			imtbase::CTreeItemModel* activeLicenses = itemModel->AddTreeModel("ActiveLicenses");
			const imtbase::ICollectionInfo& licenseInstances = productInstancePtr->GetLicenseInstances();
			imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();

			const imtbase::IObjectCollection* productsCollectionPtr = productInstancePtr->GetProductDatabase();
			imtbase::IObjectCollection::DataPtr dataPtr;

			if (productsCollectionPtr->GetObjectData(productId, dataPtr)){
				imtbase::IObjectCollection* productPtr = dynamic_cast<imtbase::IObjectCollection*>(dataPtr.GetPtr());

				if (productPtr != nullptr){
					QByteArrayList licenseIds = productPtr->GetElementIds().toList();
					for (const QByteArray& licenseId : licenseIds){
						QString name = productPtr->GetElementInfo(licenseId, imtbase::ICollectionInfo::EIT_NAME).toString();

						int index = activeLicenses->InsertNewItem();

						activeLicenses->SetData("Id", licenseId, index);
						activeLicenses->SetData("Name", name, index);
						activeLicenses->SetData("ProductId", productId, index);

						activeLicenses->SetData("LicenseState", 0, index);
						activeLicenses->SetData("ExpirationState", 0, index);
						activeLicenses->SetData("Expiration", "Unlimited", index);

						for (const QByteArray& activeLicenseId : activeLicenseIds){
							const imtlic::ILicenseInstance* licenseInstancePtr = productInstancePtr->GetLicenseInstance(activeLicenseId);
							if (licenseId == activeLicenseId){
								activeLicenses->SetData("LicenseState", 2, index);
								QDate date = licenseInstancePtr->GetExpiration().date();

								QString licenseExpirationText;
								if (date.isValid()){
									licenseExpirationText = date.toString("yyyy-MM-dd");
									activeLicenses->SetData("ExpirationState", 2, index);
									activeLicenses->SetData("Expiration", licenseExpirationText, index);
								}
							}
						}
					}
				}
			}
		}
		dataModel->SetExternTreeModel("item", itemModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable* CInstallationControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray& objectId,
		QString& name,
		QString& description,
		QString &errorMessage) const
{
	if (inputParams.isEmpty()) {
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtlic::IProductInstanceInfo> productInstancePtr = m_productInstanceFactCompPtr.CreateInstance();
		if (!productInstancePtr.IsValid()) {
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		QByteArray instanceId;

		if (itemModel.ContainsKey("Id")){
			instanceId = itemModel.GetData("Id").toByteArray();
		}

		if (instanceId.isEmpty()){
			errorMessage = QT_TR_NOOP("Installation-ID can not be empty!");
			return nullptr;
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toByteArray();
		}

		QByteArray accountId;
		if (itemModel.ContainsKey("AccountId")){
			accountId = itemModel.GetData("AccountId").toByteArray();
		}

		if (accountId.isEmpty()){
			errorMessage = QT_TR_NOOP("Account can not be empty!");
			return nullptr;
		}

		QByteArray productId;
		if (itemModel.ContainsKey("ProductId")){
			productId = itemModel.GetData("ProductId").toByteArray();
		}

		if (productId.isEmpty()){
			errorMessage = QT_TR_NOOP("Product can not be empty!");
			return nullptr;
		}

		objectId = instanceId;

		if (m_separatorObjectIdAttrPtr.IsValid()){
			objectId += *m_separatorObjectIdAttrPtr + productId;
		}

		productInstancePtr->SetupProductInstance(productId, instanceId, accountId);
		imtbase::CTreeItemModel* activeLicenses = itemModel.GetTreeItemModel("ActiveLicenses");
		if (activeLicenses != nullptr){
			for (int i = 0; i < activeLicenses->GetItemsCount(); i++){
				if (activeLicenses->ContainsKey("LicenseState")){
					int state = activeLicenses->GetData("LicenseState", i).toInt();
					if (state == 2){
						QByteArray licenseId;
						if (activeLicenses->ContainsKey("Id")){
							licenseId = activeLicenses->GetData("Id", i).toByteArray();
						}

						QDateTime expirationDate;
						if (activeLicenses->ContainsKey("Expiration")){
							QString dateExpirationStr = activeLicenses->GetData("Expiration", i).toString();
							expirationDate = QDateTime::fromString(dateExpirationStr, "yyyy-MM-dd");
						}

						productInstancePtr->AddLicense(licenseId, expirationDate);
					}
				}
			}
		}

		return productInstancePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtlicgql


