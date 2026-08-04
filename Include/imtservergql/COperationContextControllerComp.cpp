// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/COperationContextControllerComp.h>


// ImtCore includes
#include <imtbase/COperationContext.h>
#include <imtauth/CUserInfo.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlRequestProvider.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtbase::IOperationContextController)

imtbase::IOperationContext* COperationContextControllerComp::CreateOperationContext(
			const QByteArray& operationTypeId,
			const QByteArray& objectId,
			const istd::IChangeable* objectPtr,
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

	const imtgql::IGqlContext* requestContextPtr = gqlRequestPtr->GetRequestContext();
	if (requestContextPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create operation context. Error: GraphQL context is invalid"), "COperationContextControllerComp");

		return nullptr;
	}

	imtbase::IOperationContext::IdentifableObjectInfo objectInfo;

	// An unknown initiator must not suppress the change history, the entry is then just not attributed.
	const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(requestContextPtr->GetUserInfo());
	if (userInfoPtr != nullptr){
		objectInfo.id = userInfoPtr->GetObjectUuid();
		objectInfo.name = userInfoPtr->GetName();
	}
	else{
		SendWarningMessage(0, QString("User info from GraphQL context is invalid, operation '%1' will not be attributed").arg(QString::fromUtf8(operationTypeId)), "COperationContextControllerComp");
	}

	istd::TDelPtr<imtbase::COperationContext> operationContextPtr;
	operationContextPtr.SetPtr(new imtbase::COperationContext);

	operationContextPtr->SetOperationOwnerId(objectInfo);

	QByteArray tenantId = requestContextPtr->GetTenantId();
	operationContextPtr->SetTenantId(tenantId);

	if (m_documentChangeGeneratorCompPtr.IsValid()){
		imtbase::CObjectCollection* changeCollectionPtr = dynamic_cast<imtbase::CObjectCollection*>(operationContextPtr->GetChangesCollection());
		if (changeCollectionPtr == nullptr){
			SendErrorMessage(0, QString("Unable to create operation context. Error: Change collection is invalid"), "COperationContextControllerComp");

			return nullptr;
		}

		QString errorMessage;
		if (!m_documentChangeGeneratorCompPtr->GenerateDocumentChanges(operationTypeId, objectId, objectPtr, *changeCollectionPtr, errorMessage, paramsPtr)){
			SendWarningMessage(
				0,
				QString("Unable to generate document changes for operation '%1' on object '%2'. '%3'")
							.arg(QString::fromUtf8(operationTypeId), QString::fromUtf8(objectId), errorMessage),
				"COperationContextControllerComp");
		}
	}

	return operationContextPtr.PopPtr();
}


} // namespace imtservergql

