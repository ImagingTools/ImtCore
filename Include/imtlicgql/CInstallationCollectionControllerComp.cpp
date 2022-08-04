#include <imtlicgql/CInstallationCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/ILicenseInstance.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CInstallationCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);

	if (metaInfoPtr.IsValid()){
		if (informationId == QByteArray("InstanceId")){
			return metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_INSTANCE_ID);
		}
		else if (informationId == QByteArray("ProductId")){
			return metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME);
		}
		else if (informationId == QByteArray("AccountId")){
			return metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME);
		}
		else if (informationId == QByteArray("Added")){
			return metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("ModificationTime")){
			return metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CInstallationCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* metaInfoModel = nullptr;
	imtbase::CTreeItemModel* childs = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		metaInfoModel = new imtbase::CTreeItemModel();

		idoc::MetaInfoPtr metaInfo;

		QByteArray objectId = GetObjectIdFromInputParams(inputParams);
		QByteArray productInstanceId = objectId;

		if (m_separatorObjectIdAttrPtr.IsValid()){
			QString productInstanceIdStr = productInstanceId;

			QStringList splitData = productInstanceIdStr.split(*m_separatorObjectIdAttrPtr);
			productInstanceId = splitData[0].toUtf8();
		}

		int index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", QT_TR_NOOP("Instance-ID"), index);
		childs = metaInfoModel->AddTreeModel("Childs", index);
		childs->SetData("Value", productInstanceId);

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			errorMessage = QT_TR_NOOP("Unable to get an installation object data");
			return nullptr;
		}

		const imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (productInstancePtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an installation info");
			return nullptr;
		}

		QByteArray productId = productInstancePtr->GetProductId();

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", QT_TR_NOOP("Product-ID"), index);
		childs = metaInfoModel->AddTreeModel("Childs", index);
		childs->SetData("Value", productId);

		QByteArray accountId = productInstancePtr->GetCustomerId();

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", QT_TR_NOOP("Account-ID"), index);
		childs = metaInfoModel->AddTreeModel("Childs", index);
		childs->SetData("Value", accountId);

		const imtbase::ICollectionInfo& licenseList = productInstancePtr->GetLicenseInstances();

		imtbase::ICollectionInfo::Ids licenseIds = licenseList.GetElementIds();

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", QT_TR_NOOP("Licenses"), index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		int childIndex;
		for (const QByteArray& licenseCollectionId : licenseIds){
			const imtlic::ILicenseInstance* licensePtr = productInstancePtr->GetLicenseInstance(licenseCollectionId);

			QDateTime date = licensePtr->GetExpiration();
			if (licensePtr != nullptr){
				childIndex = childs->InsertNewItem();
				QString licenseName = licensePtr->GetLicenseName();
				QString value = licenseName + " (" + QT_TR_NOOP("Valid until") + ": ";
				if (date.isValid()){
					value += date.toString("dd.MM.yyyy") + ")";
				}
				else{
					value += "Unlimited)";
				}
				childs->SetData("Value", value, childIndex);
			}
		}
		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


