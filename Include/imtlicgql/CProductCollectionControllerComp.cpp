#include <imtlicgql/CProductCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <idoc/IDocumentMetaInfo.h>
// ImtCore includes
//#include <imtbase/IObjectCollectionInfo.h>
//#include <imtlic/IProductInfo.h>

#include <imtlic/CProductLicensingInfo.h>
#include <imtlic/CLicenseInfo.h>


namespace imtlicgql
{


QVariant CProductCollectionControllerComp::GetObjectInformation(
		const QByteArray &informationId,
		const QByteArray &objectId) const
{
	imtbase::ICollectionInfo::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
	if (metaInfo.IsValid()){
		if (informationId == QByteArray("Added")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("ModificationTime")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CProductCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* metaInfoModel = nullptr;
	imtbase::CTreeItemModel* childs = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error");
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		metaInfoModel = new imtbase::CTreeItemModel();

		QByteArray productId = GetObjectIdFromInputParams(inputParams);

		int index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Modification Time", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		imtbase::ICollectionInfo::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(productId);
		if (metaInfo.IsValid()){
			QString date = metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime().toString("dd.MM.yyyy hh:mm:ss");
			childs->SetData("Value", date);
		}

		imtbase::IObjectCollection::DataPtr dataPtr;

		if (!m_objectCollectionCompPtr->GetObjectData(productId, dataPtr)){
			errorMessage = QT_TR_NOOP("Unable to get an product object data");
			return nullptr;
		}

		imtbase::IObjectCollection* licensePtr = dynamic_cast<imtbase::IObjectCollection*>(dataPtr.GetPtr());
		if (licensePtr != nullptr){
			QByteArrayList licenseCollectionIds = licensePtr->GetElementIds().toList();
			index = metaInfoModel->InsertNewItem();
			metaInfoModel->SetData("Name", "Licenses", index);
			childs = metaInfoModel->AddTreeModel("Childs", index);
			int childIndex;
			for (const QByteArray& licenseCollectionId : licenseCollectionIds){
				childIndex = childs->InsertNewItem();
				QString licenseName = licensePtr->GetElementInfo(licenseCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString();
				QString value = licenseName + " (" + licenseCollectionId + ")";
				childs->SetData("Value", value, childIndex);
			}
		}
		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


