#include <imtgql/COperationContextControllerComp.h>


// ImtCore includes
#include <imtbase/COperationContext.h>
#include <imtgql/IGqlRequestProvider.h>


namespace imtgql
{


// protected methods

// reimplemented (imtbase::IOperationContextController)

imtbase::IOperationContext* COperationContextControllerComp::CreateOperationContext(
			imtbase::IOperationDescription::OperationType operationType,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const iprm::IParamsSet* paramsPtr)
{
	const imtgql::IGqlRequest* gqlRequestPtr = nullptr;
	imtgql::IGqlRequestProvider* gqlRequestProviderPtr = QueryInterface<imtgql::IGqlRequestProvider>(this);
	if (gqlRequestProviderPtr != nullptr){
		gqlRequestPtr = gqlRequestProviderPtr->GetGqlRequest();
	}

	if (gqlRequestPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create operation context. Error: GraphQL request invalid"), "COperationContextControllerComp");

		return nullptr;
	}

	imtgql::IGqlContext* requestContextPtr = gqlRequestPtr->GetRequestContext();
	if (requestContextPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create operation context. Error: GraphQL context is invalid"), "COperationContextControllerComp");

		return nullptr;
	}

	imtauth::IUserInfo* userInfoPtr = requestContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create operation context. Error:  User info from GraphQL context is invalid"), "COperationContextControllerComp");

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
		if (!m_documentChangeGeneratorCompPtr->GenerateDocumentChanges(operationType, objectId, object, *changeCollectionPtr, errorMessage, paramsPtr)){
			SendWarningMessage(0, QString("Unable to generate document changes. '%1'").arg(errorMessage), "COperationContextControllerComp");
		}
	}

	return operationContextPtr.PopPtr();
}


} // namespace imtgql


