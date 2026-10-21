// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CDocumentShareControllerComp.h>


// ImtCore includes
#include <imtservergql/DocumentSharing.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentShares.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::V1_0::imtbase::CDocumentSharesGqlHandlerCompBase)

sdl::V1_0::imtbase::CGetOrganizationDocumentSharesPayload CDocumentShareControllerComp::OnGetOrganizationDocumentShares(
			const sdl::V1_0::imtbase::CGetOrganizationDocumentSharesGqlRequest& getOrganizationDocumentSharesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetOrganizationDocumentSharesPayload response;

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const QByteArray tenantId = gqlContextPtr != nullptr ? gqlContextPtr->GetTenantId() : QByteArray();
	if (tenantId.isEmpty()){
		errorMessage = QStringLiteral("Document shares exist only within an organization");
		return response;
	}

	sdl::V1_0::imtbase::GetOrganizationDocumentSharesRequestArguments arguments = getOrganizationDocumentSharesRequest.GetRequestedArguments();
	const QString direction = arguments.input->direction.value_or(QString());

	imtauth::DocumentShares shares;
	if (direction == QStringLiteral("Incoming")){
		shares = m_documentShareManagerCompPtr->GetIncomingShares(tenantId);
	}
	else if (direction == QStringLiteral("Outgoing")){
		shares = m_documentShareManagerCompPtr->GetOutgoingShares(tenantId);
	}
	else{
		errorMessage = QStringLiteral("Invalid direction '%1'").arg(direction);
		return response;
	}

	response.shares.Emplace();
	for (const imtauth::IDocumentShareInfoSharedPtr& sharePtr : shares){
		response.shares->push_back(CreateDocumentShareRepresentation(*sharePtr));
	}

	return response;
}


sdl::V1_0::imtbase::CSetDocumentSharesStatusPayload CDocumentShareControllerComp::OnSetDocumentSharesStatus(
			const sdl::V1_0::imtbase::CSetDocumentSharesStatusGqlRequest& setDocumentSharesStatusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CSetDocumentSharesStatusPayload response;
	response.success = false;

	sdl::V1_0::imtbase::SetDocumentSharesStatusRequestArguments arguments = setDocumentSharesStatusRequest.GetRequestedArguments();
	QByteArrayList shareIds;
	if (arguments.input->shareIds){
		shareIds = arguments.input->shareIds->ToList();
	}

	const QString status = arguments.input->status.value_or(QString());

	imtauth::IDocumentShareInfo::ShareStatus shareStatus = imtauth::IDocumentShareInfo::DSS_PENDING;
	if (status == QStringLiteral("Active")){
		shareStatus = imtauth::IDocumentShareInfo::DSS_ACTIVE;
	}
	else if (status == QStringLiteral("Declined")){
		shareStatus = imtauth::IDocumentShareInfo::DSS_DECLINED;
	}
	else{
		errorMessage = QStringLiteral("Invalid status '%1'").arg(status);
		return response;
	}

	if (!CheckShareSide(gqlRequest, shareIds, SSI_TARGET, errorMessage)){
		return response;
	}

	response.success = m_documentShareManagerCompPtr->SetShareStatus(shareIds, shareStatus);

	return response;
}


sdl::V1_0::imtbase::CSetDocumentShareGroupsPayload CDocumentShareControllerComp::OnSetDocumentShareGroups(
			const sdl::V1_0::imtbase::CSetDocumentShareGroupsGqlRequest& setDocumentShareGroupsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CSetDocumentShareGroupsPayload response;
	response.success = false;

	sdl::V1_0::imtbase::SetDocumentShareGroupsRequestArguments arguments = setDocumentShareGroupsRequest.GetRequestedArguments();
	const QByteArray shareId = arguments.input->shareId.value_or(QByteArray());

	QByteArrayList groupIds;
	if (arguments.input->groupIds){
		groupIds = arguments.input->groupIds->ToList();
	}

	if (!CheckShareSide(gqlRequest, {shareId}, SSI_TARGET, errorMessage)){
		return response;
	}

	response.success = m_documentShareManagerCompPtr->SetShareTargetGroups(shareId, groupIds);

	return response;
}


sdl::V1_0::imtbase::CRemoveOrganizationDocumentSharesPayload CDocumentShareControllerComp::OnRemoveOrganizationDocumentShares(
			const sdl::V1_0::imtbase::CRemoveOrganizationDocumentSharesGqlRequest& removeOrganizationDocumentSharesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRemoveOrganizationDocumentSharesPayload response;
	response.success = false;

	sdl::V1_0::imtbase::RemoveOrganizationDocumentSharesRequestArguments arguments = removeOrganizationDocumentSharesRequest.GetRequestedArguments();
	QByteArrayList shareIds;
	if (arguments.input->shareIds){
		shareIds = arguments.input->shareIds->ToList();
	}

	if (!CheckShareSide(gqlRequest, shareIds, SSI_OWNER, errorMessage)){
		return response;
	}

	response.success = m_documentShareManagerCompPtr->RemoveShares(shareIds);

	return response;
}


// private methods

bool CDocumentShareControllerComp::CheckShareSide(const imtgql::CGqlRequest& gqlRequest, const QByteArrayList& shareIds, ShareSide side, QString& errorMessage) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const QByteArray tenantId = gqlContextPtr != nullptr ? gqlContextPtr->GetTenantId() : QByteArray();

	if (tenantId.isEmpty() || shareIds.isEmpty()){
		errorMessage = QStringLiteral("No document shares of an organization selected");
		return false;
	}

	for (const QByteArray& shareId : shareIds){
		const imtauth::IDocumentShareInfoUniquePtr sharePtr = m_documentShareManagerCompPtr->GetShare(shareId);
		const QByteArray sideTenantId = !sharePtr.IsValid() ? QByteArray() : side == SSI_OWNER ? sharePtr->GetOwnerTenantId() : sharePtr->GetTargetTenantId();
		if (sideTenantId.isEmpty() || sideTenantId != tenantId){
			errorMessage = side == SSI_OWNER
						? QStringLiteral("Only the organization owning the document can remove its shares")
						: QStringLiteral("Only the organization receiving the document can change its share");
			return false;
		}
	}

	return true;
}


} // namespace imtservergql


