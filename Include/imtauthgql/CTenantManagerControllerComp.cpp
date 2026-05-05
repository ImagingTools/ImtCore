// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantManagerControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Tenants::CGraphQlHandlerCompBase)

sdl::imtauth::Tenants::CGetTenantIdsPayload CTenantManagerControllerComp::OnGetTenantIds(
	const sdl::imtauth::Tenants::CGetTenantIdsGqlRequest& /*getTenantIdsRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantIdsPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArrayList ids = m_tenantManagerCompPtr->GetTenantIds();

	response.Version_1_0->tenantIds.Emplace().FromList(ids);
	for (const auto& id : ids){
		response.Version_1_0->tenantIds->push_back(id);
	}

	return response;
}


sdl::imtauth::Tenants::CGetTenantPayload CTenantManagerControllerComp::OnGetTenant(
	const sdl::imtauth::Tenants::CGetTenantGqlRequest& getTenantRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	sdl::imtauth::Tenants::GetTenantRequestArguments arguments = getTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	if (!tenantInfoPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant not found");
		return response;
	}

	sdl::imtauth::Tenants::CTenantData::V1_0 tenantData;
	tenantData.id = tenantInfoPtr->GetTenantId();
	tenantData.name = tenantInfoPtr->GetTenantName();
	tenantData.description = tenantInfoPtr->GetTenantDescription();
	tenantData.isActive = tenantInfoPtr->IsActive();

	response.Version_1_0->tenant = tenantData;

	return response;
}


sdl::imtauth::Tenants::CCreateTenantPayload CTenantManagerControllerComp::OnCreateTenant(
	const sdl::imtauth::Tenants::CCreateTenantGqlRequest& createTenantRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CCreateTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QString name;
	QString description;
	sdl::imtauth::Tenants::CreateTenantRequestArguments arguments = createTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->name){
		name = *arguments.input.Version_1_0->name;
	}
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}

	QByteArray tenantId = m_tenantManagerCompPtr->CreateTenant(name, description);

	if (tenantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant");
		return response;
	}

	response.Version_1_0->tenantId = tenantId;

	return response;
}


sdl::imtauth::Tenants::CRemoveTenantPayload CTenantManagerControllerComp::OnRemoveTenant(
	const sdl::imtauth::Tenants::CRemoveTenantGqlRequest& removeTenantRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRemoveTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	sdl::imtauth::Tenants::RemoveTenantRequestArguments arguments = removeTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	bool success = m_tenantManagerCompPtr->RemoveTenant(tenantId);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to remove tenant");
	}

	return response;
}


sdl::imtauth::Tenants::CUpdateTenantPayload CTenantManagerControllerComp::OnUpdateTenant(
	const sdl::imtauth::Tenants::CUpdateTenantGqlRequest& updateTenantRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CUpdateTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	QString name;
	QString description;
	sdl::imtauth::Tenants::UpdateTenantRequestArguments arguments = updateTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->name){
		name = *arguments.input.Version_1_0->name;
	}
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}

	bool success = m_tenantManagerCompPtr->UpdateTenant(tenantId, name, description);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to update tenant");
	}

	return response;
}


sdl::imtauth::Tenants::CSetTenantActivePayload CTenantManagerControllerComp::OnSetTenantActive(
	const sdl::imtauth::Tenants::CSetTenantActiveGqlRequest& setTenantActiveRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CSetTenantActivePayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	bool isActive = false;
	sdl::imtauth::Tenants::SetTenantActiveRequestArguments arguments = setTenantActiveRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->isActive){
		isActive = *arguments.input.Version_1_0->isActive;
	}

	bool success = m_tenantManagerCompPtr->SetTenantActive(tenantId, isActive);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to set tenant active state");
	}

	return response;
}


} // namespace imtauthgql
