#include <imtlicgql/CInstallationCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseInstance.h>
#include <imtlic/CProductInstanceMetaInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CInstallationCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	imtbase::IObjectCollection::MetaInfoPtr metaInfoPtr;

	if (m_objectCollectionCompPtr->GetDataMetaInfo(objectId, metaInfoPtr)){
		if (informationId == QByteArray("InstanceId")){
			return metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_INSTANCE_ID);
		}
		else if (informationId == QByteArray("Customer")){
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

		imtbase::IObjectCollection::MetaInfoPtr metaInfo;

		QByteArray productInstanceId = GetObjectIdFromInputParams(inputParams);

		int index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Modification Time", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		if (m_objectCollectionCompPtr->GetDataMetaInfo(productInstanceId, metaInfo)){
			QString date = metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime().toString("dd.MM.yyyy hh:mm:ss");
			childs->SetData("Value", date);
		}

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Instance-ID", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);
		childs->SetData("Value", productInstanceId);

		imtbase::IObjectCollection::DataPtr dataPtr;
		m_objectCollectionCompPtr->GetObjectData(productInstanceId, dataPtr);

		const imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());

		const imtbase::ICollectionInfo& licenseList = productInstancePtr->GetLicenseInstances();

		imtbase::ICollectionInfo::Ids licenseIds = licenseList.GetElementIds();

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Licenses", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		int childIndex;
		for (const QByteArray& licenseCollectionId : licenseIds){
			const imtlic::ILicenseInstance* licensePtr = productInstancePtr->GetLicenseInstance(licenseCollectionId);

			if (licensePtr != nullptr){
				childIndex = childs->InsertNewItem();
				QString licenseName = licensePtr->GetLicenseName();
				childs->SetData("Value", licenseName, childIndex);
			}
		}

		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


