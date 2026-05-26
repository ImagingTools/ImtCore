// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRoleCollectionDocumentServiceComp.h>


// ImtCore includes
#include <imtauth/IRole.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CGraphQlHandlerCompBase)

sdl::imtauth::Roles::CRoleData CRoleCollectionDocumentServiceComp::OnGetRoleRepresentation(
		const sdl::imtauth::RoleCollectionDocumentService::CGetRoleRepresentationGqlRequest& getRoleRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::RoleCollectionDocumentService::GetRoleRepresentationRequestArguments arguments = getRoleRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtauth::Roles::CRoleData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::imtauth::Roles::CRoleData();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::imtauth::Roles::CRoleData();
	}

	const imtauth::IRole* rolePtr = dynamic_cast<const imtauth::IRole*>(documentPtr.GetPtr());
	if (rolePtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::imtauth::Roles::CRoleData();
	}

	sdl::imtauth::Roles::CRoleData response;
	response.Version_1_0.Emplace();

	response.Version_1_0->id = rolePtr->GetRoleId();
	response.Version_1_0->name = rolePtr->GetRoleName();
	response.Version_1_0->description = rolePtr->GetRoleDescription();
	response.Version_1_0->roleId = rolePtr->GetRoleId();
	response.Version_1_0->productId = rolePtr->GetProductId();
	response.Version_1_0->isDefault = rolePtr->IsDefault();
	response.Version_1_0->isGuest = rolePtr->IsGuest();

	response.Version_1_0->parentRoles.Emplace();
	QByteArrayList includedRoles = rolePtr->GetIncludedRoles();
	for (const QByteArray& parentRoleId : includedRoles){
		response.Version_1_0->parentRoles->push_back(parentRoleId);
	}

	// permissions is a flat ID-list of local permissions joined with the
	// schema-defined separator (kept as a single string field for parity
	// with the existing RoleData representation).
	QByteArrayList localPermissions = rolePtr->GetLocalPermissions();
	response.Version_1_0->permissions = localPermissions.join(',');

	return response;
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CRoleCollectionDocumentServiceComp::OnUpdateRoleFromRepresentation(
		const sdl::imtauth::RoleCollectionDocumentService::CUpdateRoleFromRepresentationGqlRequest& updateRoleFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::RoleCollectionDocumentService::UpdateRoleFromRepresentationRequestArguments arguments = updateRoleFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	if (documentId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return response;
	}

	QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::IRole* rolePtr = dynamic_cast<imtauth::IRole*>(documentPtr.GetPtr());
	if (rolePtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::imtauth::Roles::CRoleData::V1_0 roleData;
	if (arguments.input.Version_1_0->role){
		roleData = *arguments.input.Version_1_0->role;
	}

	if (roleData.name){
		rolePtr->SetRoleName(*roleData.name);
	}
	if (roleData.description){
		rolePtr->SetRoleDescription(*roleData.description);
	}
	if (roleData.roleId){
		rolePtr->SetRoleId(*roleData.roleId);
	}
	if (roleData.productId){
		rolePtr->SetProductId(*roleData.productId);
	}
	if (roleData.isDefault){
		rolePtr->SetDefault(*roleData.isDefault);
	}
	if (roleData.isGuest){
		rolePtr->SetGuest(*roleData.isGuest);
	}

	if (roleData.parentRoles){
		// Reset included roles to the new set: exclude current ones,
		// then include the new ones.
		QByteArrayList currentParents = rolePtr->GetIncludedRoles();
		for (const QByteArray& parentRoleId : currentParents){
			rolePtr->ExcludeRole(parentRoleId);
		}
		for (const auto& parentRoleIdPtr : *roleData.parentRoles){
			if (parentRoleIdPtr){
				rolePtr->IncludeRole(*parentRoleIdPtr);
			}
		}
	}

	if (roleData.permissions){
		QByteArrayList permissions;
		QByteArray joined = *roleData.permissions;
		if (!joined.isEmpty()){
			permissions = joined.split(',');
		}
		rolePtr->SetLocalPermissions(permissions);
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
