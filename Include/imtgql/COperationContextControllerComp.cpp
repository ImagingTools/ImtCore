#include <imtgql/COperationContextControllerComp.h>


// ImtCore includes
#include <imtbase/COperationContext.h>


namespace imtgql
{


// protected methods

// reimplemented (imtbase::IOperationContextController)

imtbase::IOperationContext* COperationContextControllerComp::CreateOperationContext(
			int operationType,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& objectId,
			const istd::IChangeable* objectDataPtr,
			const iprm::IParamsSet* paramsPtr)
{
	imtgql::IGqlContext* requestContextPtr = gqlRequest.GetRequestContext();
	if (requestContextPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create operation context. GraphQL context is nullptr."), "COperationContextControllerComp");

		return nullptr;
	}

	imtauth::IUserInfo* userInfoPtr = requestContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create operation context. User info from GraphQL context is nullptr."), "COperationContextControllerComp");

		return nullptr;
	}

	imtbase::IOperationContext::IdentifableObjectInfo objectInfo;
	objectInfo.id = userInfoPtr->GetId();
	objectInfo.name = userInfoPtr->GetName();

	istd::TDelPtr<imtbase::COperationContext> operationContextPtr;
	operationContextPtr.SetPtr(new imtbase::COperationContext);

	operationContextPtr->SetOperationOwnerId(objectInfo);

	if (m_documentChangeGeneratorCompPtr.IsValid()){
		imtbase::CObjectCollection* changeCollectionPtr = dynamic_cast<imtbase::CObjectCollection*>(operationContextPtr->GetChangesCollection());

		QString errorMessage;
		if (!m_documentChangeGeneratorCompPtr->GenerateDocumentChanges(operationType, objectId, objectDataPtr, *changeCollectionPtr, errorMessage, paramsPtr)){
			SendWarningMessage(0, QString("Unable to generate document changes. %1").arg(errorMessage), "COperationContextControllerComp");
		}
	}

	return operationContextPtr.PopPtr();
}


} // namespace imtgql


