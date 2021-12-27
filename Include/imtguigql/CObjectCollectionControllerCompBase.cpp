#include <imtguigql/CObjectCollectionControllerCompBase.h>

#include <imtqml/CCommandDataEnumProviderComp.h>
#include <iqtgui/CHierarchicalCommand.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::CreateResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, errorMessage, operationType)){
		return nullptr;
	}

	Q_ASSERT(operationType != OT_UNKNOWN);

	switch (operationType){
	case OT_NEW:
		return InsertObject(gqlRequest, errorMessage);
	case OT_UPDATE:
		return UpdateObject(gqlRequest, errorMessage);
	case OT_DELETE:
		return DeleteObject(gqlRequest, errorMessage);
	case OT_RENAME:
		return RenameObject(gqlRequest, errorMessage);
	case OT_SET_DESCRIPTION:
		return SetObjectDescription(gqlRequest, errorMessage);
	case OT_LIST:
		return ListObjects(gqlRequest, errorMessage);
	case OT_HEADERS:
		return Headers(gqlRequest, errorMessage);
	}

	return nullptr;
}


// protected methods

bool CObjectCollectionControllerCompBase::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage,
			int& operationType) const
{
	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	if(fieldList == nullptr){
		return false;
	}

	int count = fieldList->count();
	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "headers"){
			operationType = OT_HEADERS;
			return true;
		}
		if (fieldList->at(i).GetId() == "items"){
			operationType = OT_LIST;
			return true;
		}
	}
	return false;
}


QByteArray CObjectCollectionControllerCompBase::GetObjectIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	return QByteArray();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::InsertObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::UpdateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::RenameObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::SetObjectDescription(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";

		return nullptr;
	}

	QByteArray objectId = GetObjectIdFromRequest(gqlRequest);
	if (objectId.isEmpty()){
		errorMessage = "No object-ID could not be extracted from the request";

		return nullptr;
	}

	bool retVal = m_objectCollectionCompPtr->RemoveObject(objectId);
	if (retVal){
		// CREATE SUCCESS RESPONSE!

		Q_ASSERT_X(false, "CObjectCollectionControllerCompBase::DeleteObject", "Not implemented");
	}

	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetHeaders(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	bool isSetResponce = false;
	QByteArrayList fields;

	if (!m_viewDelegateCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		const imtbase::ICollectionInfo& fieldCollection = m_viewDelegateCompPtr->GetSummaryInformationTypes();
		QVector<QByteArray> fieldIds = fieldCollection.GetElementIds();

		for (QByteArray fieldId : fieldIds){
			QString headerName = fieldCollection.GetElementInfo(fieldId, imtbase::ICollectionInfo::EIT_NAME).toString();
			int index = itemsModel->InsertNewItem();
			itemsModel->SetData(imtqml::CCommandDataEnumProviderComp::NAME, headerName,index);
		}
		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("headers", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetCommands(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	bool isSetResponce = false;
	QByteArrayList fields;

	if (!m_viewDelegateCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();

		const imtbase::ICollectionInfo& fieldCollection = m_viewDelegateCompPtr->GetSummaryInformationTypes();
		QVector<QByteArray> fieldIds = fieldCollection.GetElementIds();

		for (QByteArray fieldId : fieldIds){
			QString headerName = fieldCollection.GetElementInfo(fieldId, imtbase::ICollectionInfo::EIT_NAME).toString();
			int index = itemsModel->InsertNewItem();
			itemsModel->SetData(imtqml::CCommandDataEnumProviderComp::NAME, headerName,index);
		}
		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("headers", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtgql


