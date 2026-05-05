// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantManagerControllerComp.h>


namespace imtauthgql
{


// reimplemented (sdl::imtauth::Tenants::CGraphQlHandlerCompBase)

sdl::imtauth::Tenants::CGetTenantIdsPayload CTenantManagerControllerComp::OnGetTenantIds(
			const sdl::imtauth::Tenants::CGetTenantIdsGqlRequest& /*getTenantIdsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantIdsPayload payload;
	payload.Version_1_0 = sdl::imtauth::Tenants::CGetTenantIdsPayload::V1_0();

	if (!m_tenantManagerCompPtr.IsValid()){
		payload.Version_1_0->errorMessage = QStringLiteral("Tenant manager not available");
		return payload;
	}

	QByteArrayList ids = m_tenantManagerCompPtr->GetTenantIds();

	payload.Version_1_0->tenantIds = std::vector<std::optional<QByteArray>>();
	for (const auto& id : ids){
		payload.Version_1_0->tenantIds->push_back(id);
	}

	return payload;
}


sdl::imtauth::Tenants::CGetTenantPayload CTenantManagerControllerComp::OnGetTenant(
			const sdl::imtauth::Tenants::CGetTenantGqlRequest& getTenantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantPayload payload;
	payload.Version_1_0 = sdl::imtauth::Tenants::CGetTenantPayload::V1_0();

	if (!m_tenantManagerCompPtr.IsValid()){
		payload.Version_1_0->errorMessage = QStringLiteral("Tenant manager not available");
		return payload;
	}

	if (!getTenantRequest.arguments.input.Version_1_0.has_value() ||
		!getTenantRequest.arguments.input.Version_1_0->tenantId.has_value()){
		payload.Version_1_0->errorMessage = QStringLiteral("Missing tenantId argument");
		return payload;
	}

	QByteArray tenantId = *getTenantRequest.arguments.input.Version_1_0->tenantId;
	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	if (!tenantInfoPtr.IsValid()){
		payload.Version_1_0->errorMessage = QStringLiteral("Tenant not found");
		return payload;
	}

	sdl::imtauth::Tenants::CTenantData tenantData;
	tenantData.id = tenantInfoPtr->GetTenantId();
	tenantData.name = tenantInfoPtr->GetTenantName();
	tenantData.description = tenantInfoPtr->GetTenantDescription();
	tenantData.isActive = tenantInfoPtr->IsActive();

	payload.Version_1_0->tenant = tenantData;

	return payload;
}


sdl::imtauth::Tenants::CCreateTenantPayload CTenantManagerControllerComp::OnCreateTenant(
			const sdl::imtauth::Tenants::CCreateTenantGqlRequest& createTenantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CCreateTenantPayload payload;
	payload.Version_1_0 = sdl::imtauth::Tenants::CCreateTenantPayload::V1_0();

	if (!m_tenantManagerCompPtr.IsValid()){
		payload.Version_1_0->errorMessage = QStringLiteral("Tenant manager not available");
		return payload;
	}

	if (!createTenantRequest.arguments.input.Version_1_0.has_value() ||
		!createTenantRequest.arguments.input.Version_1_0->name.has_value()){
		payload.Version_1_0->errorMessage = QStringLiteral("Missing name argument");
		return payload;
	}

	QString name = *createTenantRequest.arguments.input.Version_1_0->name;
	QString description;
	if (createTenantRequest.arguments.input.Version_1_0->description.has_value()){
		description = *createTenantRequest.arguments.input.Version_1_0->description;
	}

	QByteArray tenantId = const_cast<imtauth::ITenantManager*>(m_tenantManagerCompPtr.GetPtr())->CreateTenant(name, description);

	if (tenantId.isEmpty()){
		payload.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant");
		return payload;
	}

	payload.Version_1_0->tenantId = tenantId;

	return payload;
}


sdl::imtauth::Tenants::CRemoveTenantPayload CTenantManagerControllerComp::OnRemoveTenant(
			const sdl::imtauth::Tenants::CRemoveTenantGqlRequest& removeTenantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRemoveTenantPayload payload;
	payload.Version_1_0 = sdl::imtauth::Tenants::CRemoveTenantPayload::V1_0();

	if (!m_tenantManagerCompPtr.IsValid()){
		payload.Version_1_0->errorMessage = QStringLiteral("Tenant manager not available");
		return payload;
	}

	if (!removeTenantRequest.arguments.input.Version_1_0.has_value() ||
		!removeTenantRequest.arguments.input.Version_1_0->tenantId.has_value()){
		payload.Version_1_0->errorMessage = QStringLiteral("Missing tenantId argument");
		return payload;
	}

	QByteArray tenantId = *removeTenantRequest.arguments.input.Version_1_0->tenantId;
	bool success = const_cast<imtauth::ITenantManager*>(m_tenantManagerCompPtr.GetPtr())->RemoveTenant(tenantId);

	payload.Version_1_0->success = success;
	if (!success){
		payload.Version_1_0->errorMessage = QStringLiteral("Failed to remove tenant");
	}

	return payload;
}


sdl::imtauth::Tenants::CUpdateTenantPayload CTenantManagerControllerComp::OnUpdateTenant(
			const sdl::imtauth::Tenants::CUpdateTenantGqlRequest& updateTenantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CUpdateTenantPayload payload;
	payload.Version_1_0 = sdl::imtauth::Tenants::CUpdateTenantPayload::V1_0();

	if (!m_tenantManagerCompPtr.IsValid()){
		payload.Version_1_0->errorMessage = QStringLiteral("Tenant manager not available");
		return payload;
	}

	if (!updateTenantRequest.arguments.input.Version_1_0.has_value() ||
		!updateTenantRequest.arguments.input.Version_1_0->tenantId.has_value() ||
		!updateTenantRequest.arguments.input.Version_1_0->name.has_value()){
		payload.Version_1_0->errorMessage = QStringLiteral("Missing required arguments");
		return payload;
	}

	QByteArray tenantId = *updateTenantRequest.arguments.input.Version_1_0->tenantId;
	QString name = *updateTenantRequest.arguments.input.Version_1_0->name;
	QString description;
	if (updateTenantRequest.arguments.input.Version_1_0->description.has_value()){
		description = *updateTenantRequest.arguments.input.Version_1_0->description;
	}

	bool success = const_cast<imtauth::ITenantManager*>(m_tenantManagerCompPtr.GetPtr())->UpdateTenant(tenantId, name, description);

	payload.Version_1_0->success = success;
	if (!success){
		payload.Version_1_0->errorMessage = QStringLiteral("Failed to update tenant");
	}

	return payload;
}


sdl::imtauth::Tenants::CSetTenantActivePayload CTenantManagerControllerComp::OnSetTenantActive(
			const sdl::imtauth::Tenants::CSetTenantActiveGqlRequest& setTenantActiveRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CSetTenantActivePayload payload;
	payload.Version_1_0 = sdl::imtauth::Tenants::CSetTenantActivePayload::V1_0();

	if (!m_tenantManagerCompPtr.IsValid()){
		payload.Version_1_0->errorMessage = QStringLiteral("Tenant manager not available");
		return payload;
	}

	if (!setTenantActiveRequest.arguments.input.Version_1_0.has_value() ||
		!setTenantActiveRequest.arguments.input.Version_1_0->tenantId.has_value() ||
		!setTenantActiveRequest.arguments.input.Version_1_0->isActive.has_value()){
		payload.Version_1_0->errorMessage = QStringLiteral("Missing required arguments");
		return payload;
	}

	QByteArray tenantId = *setTenantActiveRequest.arguments.input.Version_1_0->tenantId;
	bool isActive = *setTenantActiveRequest.arguments.input.Version_1_0->isActive;

	bool success = const_cast<imtauth::ITenantManager*>(m_tenantManagerCompPtr.GetPtr())->SetTenantActive(tenantId, isActive);

	payload.Version_1_0->success = success;
	if (!success){
		payload.Version_1_0->errorMessage = QStringLiteral("Failed to set tenant active state");
	}

	return payload;
}


} // namespace imtauthgql
