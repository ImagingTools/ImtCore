// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants_fwd.h>


namespace imtauthgql
{


class CRemoteTenantControllerComp:
			virtual public imtauth::ITenantManager,
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteTenantControllerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantManager);
		I_ASSIGN(m_tenantFactoryCompPtr, "TenantFactory", "Factory for tenant info objects", true, "TenantFactory");
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantManager)
	virtual QByteArrayList GetTenantIds() const override;
	virtual imtauth::ITenantInfoUniquePtr GetTenant(const QByteArray& tenantId) const override;
	virtual QByteArray CreateTenant(const QString& tenantName, const QString& description = QString(), const QByteArray& ownerId = QByteArray()) override;
	virtual bool RemoveTenant(const QByteArray& tenantId) override;
	virtual bool UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description, const QByteArray& ownerId = QByteArray(), bool updateOwner = false) override;
	virtual bool SetTenantActive(const QByteArray& tenantId, bool isActive) override;

protected:
	// reimplemented (sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetTenantIdsPayload OnGetTenantIds(
				const sdl::V1_0::imtauth::CGetTenantIdsGqlRequest& getTenantIdsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetTenantRelationshipsPayload OnGetTenantRelationships(
				const sdl::V1_0::imtauth::CGetTenantRelationshipsGqlRequest& getTenantRelationshipsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetTenantPayload OnGetTenant(
				const sdl::V1_0::imtauth::CGetTenantGqlRequest& getTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateTenantPayload OnCreateTenant(
				const sdl::V1_0::imtauth::CCreateTenantGqlRequest& createTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRemoveTenantPayload OnRemoveTenant(
				const sdl::V1_0::imtauth::CRemoveTenantGqlRequest& removeTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUpdateTenantPayload OnUpdateTenant(
				const sdl::V1_0::imtauth::CUpdateTenantGqlRequest& updateTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSetTenantActivePayload OnSetTenantActive(
				const sdl::V1_0::imtauth::CSetTenantActiveGqlRequest& setTenantActiveRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAddTenantRelationshipPayload OnAddTenantRelationship(
				const sdl::V1_0::imtauth::CAddTenantRelationshipGqlRequest& addTenantRelationshipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload OnRemoveTenantRelationship(
				const sdl::V1_0::imtauth::CRemoveTenantRelationshipGqlRequest& removeTenantRelationshipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_FACT(imtauth::ITenantInfo, m_tenantFactoryCompPtr);
};


} // namespace imtauthgql
