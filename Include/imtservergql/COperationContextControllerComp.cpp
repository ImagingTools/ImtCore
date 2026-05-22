// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/COperationContextControllerComp.h>


// ImtCore includes
#include <imtbase/COperationContext.h>
#include <imtauth/CUserInfo.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlRequestProvider.h>


namespace imtservergql
{


namespace
{


QByteArray GetInputArgument(const imtgql::IGqlRequest& gqlRequest, const QByteArray& argumentId)
{
	const imtgql::CGqlRequest* concreteRequestPtr = dynamic_cast<const imtgql::CGqlRequest*>(&gqlRequest);
	if (concreteRequestPtr == nullptr){
		return QByteArray();
	}

	const imtgql::CGqlParamObject* inputParamPtr = concreteRequestPtr->GetParamObject("input");
	if (inputParamPtr == nullptr || !inputParamPtr->ContainsParam(argumentId)){
		return QByteArray();
	}

	return inputParamPtr->GetParamArgumentValue(argumentId).toByteArray();
}


bool IsTenantScopedProductCommand(const QByteArray& commandId)
{
	return commandId == QByteArrayLiteral("RoleAdd")
			|| commandId == QByteArrayLiteral("RoleUpdate")
			|| commandId == QByteArrayLiteral("GroupAdd")
			|| commandId == QByteArrayLiteral("GroupUpdate")
			|| commandId == QByteArrayLiteral("UserAdd")
			|| commandId == QByteArrayLiteral("UserUpdate");
}


QByteArray GetOperationTenantId(
			const imtgql::IGqlRequest& gqlRequest,
			const imtgql::IGqlContext& requestContext)
{
	QByteArray tenantId = GetInputArgument(gqlRequest, "tenantId");
	if (!tenantId.isEmpty()){
		return tenantId;
	}

	if (IsTenantScopedProductCommand(gqlRequest.GetCommandId())){
		tenantId = GetInputArgument(gqlRequest, "productId");
		if (!tenantId.isEmpty()){
			return tenantId;
		}
	}

	return requestContext.GetTenantId();
}


} // namespace


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

	const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(requestContextPtr->GetUserInfo());
	if (userInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create operation context. Error:  User info from GraphQL context is invalid"), "COperationContextControllerComp");

		return nullptr;
	}

	imtbase::IOperationContext::IdentifableObjectInfo objectInfo;
	objectInfo.id = userInfoPtr->GetObjectUuid();
	objectInfo.name = userInfoPtr->GetName();

	istd::TDelPtr<imtbase::COperationContext> operationContextPtr;
	operationContextPtr.SetPtr(new imtbase::COperationContext);

	operationContextPtr->SetOperationOwnerId(objectInfo);

	QByteArray tenantId = GetOperationTenantId(*gqlRequestPtr, *requestContextPtr);
	operationContextPtr->SetTenantId(tenantId);

	if (m_documentChangeGeneratorCompPtr.IsValid()){
		imtbase::CObjectCollection* changeCollectionPtr = dynamic_cast<imtbase::CObjectCollection*>(operationContextPtr->GetChangesCollection());

		QString errorMessage;
		if (!m_documentChangeGeneratorCompPtr->GenerateDocumentChanges(operationTypeId, objectId, objectPtr, *changeCollectionPtr, errorMessage, paramsPtr)){
			SendWarningMessage(0, QString("Unable to generate document changes. '%1'").arg(errorMessage), "COperationContextControllerComp");
		}
	}

	return operationContextPtr.PopPtr();
}


} // namespace imtservergql

