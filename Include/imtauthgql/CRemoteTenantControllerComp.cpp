// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantControllerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// reimplemented (imtauth::ITenantManager)

QByteArrayList CRemoteTenantControllerComp::GetTenantIds() const
{
	return GetElementIds(QByteArrayLiteral("Tenants"));
}


imtauth::ITenantInfoUniquePtr CRemoteTenantControllerComp::GetTenant(const QByteArray& tenantId) const
{
	if (!m_tenantFactoryCompPtr.IsValid()){
		return nullptr;
	}

	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, sdl::imtauth::Tenants::CGetTenantGqlRequest::GetCommandId());
	sdl::imtauth::Tenants::CGetTenantGqlRequest getTenantRequest(gqlRequest, false);

	QString errorMessage;
	sdl::imtauth::Tenants::CGetTenantPayload payload = OnGetTenant(getTenantRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->tenant.has_value()){
		return nullptr;
	}

	const auto& tenantData = *payload.Version_1_0->tenant;
	if (!tenantData.Version_1_0.has_value()){
		return nullptr;
	}

	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantFactoryCompPtr.CreateInstance();
	if (!tenantInfoPtr.IsValid()){
		return nullptr;
	}

	if (tenantData.Version_1_0->name){
		tenantInfoPtr->SetTenantName(*tenantData.Version_1_0->name);
	}
	if (tenantData.Version_1_0->description){
		tenantInfoPtr->SetTenantDescription(*tenantData.Version_1_0->description);
	}
	if (tenantData.Version_1_0->isActive){
		tenantInfoPtr->SetActive(*tenantData.Version_1_0->isActive);
	}

	return tenantInfoPtr;
}


QByteArray CRemoteTenantControllerComp::CreateTenant(const QString& tenantName, const QString& description)
{
	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_MUTATION, sdl::imtauth::Tenants::CCreateTenantGqlRequest::GetCommandId());
	sdl::imtauth::Tenants::CCreateTenantGqlRequest createTenantRequest(gqlRequest, false);

	QString errorMessage;
	sdl::imtauth::Tenants::CCreateTenantPayload payload = OnCreateTenant(createTenantRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->tenantId.has_value()){
		return QByteArray();
	}

	return *payload.Version_1_0->tenantId;
}


bool CRemoteTenantControllerComp::RemoveTenant(const QByteArray& tenantId)
{
	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_MUTATION, sdl::imtauth::Tenants::CRemoveTenantGqlRequest::GetCommandId());
	sdl::imtauth::Tenants::CRemoveTenantGqlRequest removeTenantRequest(gqlRequest, false);

	QString errorMessage;
	sdl::imtauth::Tenants::CRemoveTenantPayload payload = OnRemoveTenant(removeTenantRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->success.has_value()){
		return false;
	}

	return *payload.Version_1_0->success;
}


bool CRemoteTenantControllerComp::UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description)
{
	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_MUTATION, sdl::imtauth::Tenants::CUpdateTenantGqlRequest::GetCommandId());
	sdl::imtauth::Tenants::CUpdateTenantGqlRequest updateTenantRequest(gqlRequest, false);

	QString errorMessage;
	sdl::imtauth::Tenants::CUpdateTenantPayload payload = OnUpdateTenant(updateTenantRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->success.has_value()){
		return false;
	}

	return *payload.Version_1_0->success;
}


bool CRemoteTenantControllerComp::SetTenantActive(const QByteArray& tenantId, bool isActive)
{
	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_MUTATION, sdl::imtauth::Tenants::CSetTenantActiveGqlRequest::GetCommandId());
	sdl::imtauth::Tenants::CSetTenantActiveGqlRequest setTenantActiveRequest(gqlRequest, false);

	QString errorMessage;
	sdl::imtauth::Tenants::CSetTenantActivePayload payload = OnSetTenantActive(setTenantActiveRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->success.has_value()){
		return false;
	}

	return *payload.Version_1_0->success;
}


// reimplemented (sdl::imtauth::Tenants::CGraphQlHandlerCompBase)

sdl::imtauth::Tenants::CGetTenantPayload CRemoteTenantControllerComp::OnGetTenant(
			const sdl::imtauth::Tenants::CGetTenantGqlRequest& /*getTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Tenants::CGetTenantPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Tenants::CCreateTenantPayload CRemoteTenantControllerComp::OnCreateTenant(
			const sdl::imtauth::Tenants::CCreateTenantGqlRequest& /*createTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Tenants::CCreateTenantPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Tenants::CRemoveTenantPayload CRemoteTenantControllerComp::OnRemoveTenant(
			const sdl::imtauth::Tenants::CRemoveTenantGqlRequest& /*removeTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Tenants::CRemoveTenantPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Tenants::CUpdateTenantPayload CRemoteTenantControllerComp::OnUpdateTenant(
			const sdl::imtauth::Tenants::CUpdateTenantGqlRequest& /*updateTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Tenants::CUpdateTenantPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Tenants::CSetTenantActivePayload CRemoteTenantControllerComp::OnSetTenantActive(
			const sdl::imtauth::Tenants::CSetTenantActiveGqlRequest& /*setTenantActiveRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Tenants::CSetTenantActivePayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
