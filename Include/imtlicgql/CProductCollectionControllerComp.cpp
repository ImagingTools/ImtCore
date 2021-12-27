#include <imtlicgql/CProductCollectionControllerComp.h>

// ImtCore includes
#include <imtqml/CCommandDataEnumProviderComp.h>

namespace imtlicgql
{


CProductCollectionControllerComp::CProductCollectionControllerComp()
	: imtguigql::CObjectCollectionControllerCompBase()
{

}

imtbase::CTreeItemModel *CProductCollectionControllerComp::ListObjects(const imtgql::CGqlRequest &gqlRequest, QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	bool isSetResponce = false;
	QByteArrayList fields;


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

		imtbase::ICollectionInfo::Ids ids = m_objectCollectionCompPtr->GetElementIds();
		imtbase::IObjectCollection::DataPtr dataPtr;
		for (int index = 0; index < ids.count(); index++){
//			m_objectCollectionCompPtr->GetObjectData(ids[index], dataPtr);
			QString packageId = m_objectCollectionCompPtr->GetElementInfo(ids[index], imtbase::ICollectionInfo::EIT_TYPE_ID).toString();
			QString packageName = m_objectCollectionCompPtr->GetElementInfo(ids[index], imtbase::ICollectionInfo::EIT_NAME).toString();
			QString packageDescription = m_objectCollectionCompPtr->GetElementInfo(ids[index], imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			int itemIndex = itemsModel->InsertNewItem();
//			itemsModel->SetData(imtqml::CCommandDataEnumProviderComp::ID, packageId,itemIndex);
			itemsModel->SetData("Name", packageName,itemIndex);
			itemsModel->SetData("Description", packageDescription,itemIndex);
			itemsModel->SetData("Added", "",itemIndex);
			itemsModel->SetData("LastModified", "",itemIndex);
		}

		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("items", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtgql


