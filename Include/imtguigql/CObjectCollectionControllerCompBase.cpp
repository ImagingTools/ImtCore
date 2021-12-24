#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::CreateResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (gqlRequest.GetCommandId() != *m_modelIdCompPtr){
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
	}

	return nullptr;
}


// protected methods

bool CObjectCollectionControllerCompBase::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage,
			int& operationType) const
{
	// IMPLEMENT!!!
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


} // namespace imtgql


