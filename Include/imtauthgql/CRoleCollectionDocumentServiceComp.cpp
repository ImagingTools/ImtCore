// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRoleCollectionDocumentServiceComp.h>


// Qt includes
#include <QUuid>

// ImtCore includes
#include <imtauth/CRole.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/RoleCollectionDocumentService.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CRoleCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CRoleData CRoleCollectionDocumentServiceComp::OnGetRoleRepresentation(
		const sdl::V1_0::imtauth::CGetRoleRepresentationGqlRequest& getRoleRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetRoleRepresentationRequestArguments arguments = getRoleRepresentationRequest.GetRequestedArguments();
	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input && arguments.input->id){
		objectId = *arguments.input->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::V1_0::imtauth::CRoleData();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::V1_0::imtauth::CRoleData();
	}

	const imtauth::CIdentifiableRoleInfo* rolePtr = dynamic_cast<const imtauth::CIdentifiableRoleInfo*>(documentPtr.GetPtr());
	if (rolePtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::V1_0::imtauth::CRoleData();
	}

	sdl::V1_0::imtauth::CRoleData response;
	response.id = rolePtr->GetObjectUuid();
	response.name = rolePtr->GetRoleName();
	response.description = rolePtr->GetRoleDescription();
	response.roleId = rolePtr->GetRoleId();
	response.productId = rolePtr->GetProductId();
	response.isDefault = rolePtr->IsDefault();
	response.isGuest = rolePtr->IsGuest();

	response.parentRoles.Emplace();
	QByteArrayList includedRoles = rolePtr->GetIncludedRoles();
	for (const QByteArray& parentRoleId : includedRoles){
		response.parentRoles->push_back(parentRoleId);
	}

	// permissions is a flat ID-list of local permissions joined with the
	// schema-defined separator (kept as a single string field for parity
	// with the existing RoleData representation).
	QByteArrayList localPermissions = rolePtr->GetLocalPermissions();
	response.permissions = localPermissions.join(';');

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CRoleCollectionDocumentServiceComp::OnUpdateRoleFromRepresentation(
		const sdl::V1_0::imtauth::CUpdateRoleFromRepresentationGqlRequest& updateRoleFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::UpdateRoleFromRepresentationRequestArguments arguments = updateRoleFromRepresentationRequest.GetRequestedArguments();
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
	}

	if (documentId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return response;
	}

	QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::CIdentifiableRoleInfo* rolePtr = dynamic_cast<imtauth::CIdentifiableRoleInfo*>(documentPtr.GetPtr());
	if (rolePtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::V1_0::imtauth::CRoleData roleData;
	if (arguments.input->role){
		roleData = *arguments.input->role;
	}

	if (roleData.id){
		rolePtr->SetObjectUuid(*roleData.id);
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
			permissions = joined.split(';');
		}
		rolePtr->SetLocalPermissions(permissions);
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
