#include <imtdb/CDatabaseServerConnectionControllerComp.h>


namespace imtdb
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CDatabaseServerConnectionControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_dbServerConnectionCheckerCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	bool isConnected = m_dbServerConnectionCheckerCompPtr->IsDatabaseServerConnected(errorMessage);

	dataModelPtr->SetData("IsConnected", isConnected);

	if (!isConnected){
		imtbase::CTreeItemModel* errorsModelPtr = rootModelPtr->AddTreeModel("errors");

		errorsModelPtr->SetData("message", errorMessage);
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtdb


