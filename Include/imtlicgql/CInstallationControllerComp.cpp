#include <imtlicgql/CInstallationControllerComp.h>

// ImtCore includes
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtlic/CLicenseInstance.h>
#include <imtbase/ICollectionInfo.h>


namespace imtlicgql
{


imtbase::CTreeItemModel* CInstallationControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	dataModel->SetData("Id", "");
	dataModel->SetData("Name", "");
	dataModel->SetData("AccountId", "");
	dataModel->SetData("ProductId", "");

	imtbase::CTreeItemModel* activeLicenses = dataModel->AddTreeModel("ActiveLicenses");

	const QList<imtgql::CGqlObject>* inputParams = gqlRequest.GetParams();
	QByteArray objectId = GetObjectIdFromInputParams(*inputParams);

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

		dataModel->SetData("Id", instanceId);

		QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		dataModel->SetData("Name", name);

		dataModel->SetData("AccountId", accountId);
		dataModel->SetData("ProductId", productId);

		const imtbase::ICollectionInfo& licenseInstances = productInstancePtr->GetLicenseInstances();
		imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();

		for (const QByteArray& activeLicenseId : activeLicenseIds){
			const imtlic::ILicenseInstance* licenseInstancePtr = productInstancePtr->GetLicenseInstance(activeLicenseId);

			int index = activeLicenses->InsertNewItem();

			QString licenseName = licenseInstancePtr->GetLicenseName();

			activeLicenses->SetData("Id", activeLicenseId, index);
			activeLicenses->SetData("Name", name, index);

			QDate date = licenseInstancePtr->GetExpiration().date();
			QString licenseExpiration = date.toString("yyyy-MM-dd");
			activeLicenses->SetData("Expiration", licenseExpiration, index);
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
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
		itemModel.CreateFromJson(itemData);

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

		return productInstancePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtlicgql


