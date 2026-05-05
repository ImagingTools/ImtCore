// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


class CTenantManagerControllerComp: public sdl::imtauth::Tenants::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Tenants::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantManagerControllerComp);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager providing CRUD operations", true, "TenantManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Tenants::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Tenants::CGetTenantIdsPayload OnGetTenantIds(
				const sdl::imtauth::Tenants::CGetTenantIdsGqlRequest& getTenantIdsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CGetTenantPayload OnGetTenant(
				const sdl::imtauth::Tenants::CGetTenantGqlRequest& getTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CCreateTenantPayload OnCreateTenant(
				const sdl::imtauth::Tenants::CCreateTenantGqlRequest& createTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CRemoveTenantPayload OnRemoveTenant(
				const sdl::imtauth::Tenants::CRemoveTenantGqlRequest& removeTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CUpdateTenantPayload OnUpdateTenant(
				const sdl::imtauth::Tenants::CUpdateTenantGqlRequest& updateTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CSetTenantActivePayload OnSetTenantActive(
				const sdl::imtauth::Tenants::CSetTenantActiveGqlRequest& setTenantActiveRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauthgql
