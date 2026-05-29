// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants>


namespace imtauthgql
{


class CRemoteTenantControllerComp:
			virtual public imtauth::ITenantManager,
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::imtauth::Tenants::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtauth::Tenants::CGraphQlHandlerCompBase> BaseClass;

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
	// reimplemented (sdl::imtauth::Tenants::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Tenants::CGetTenantIdsPayload OnGetTenantIds(
				const sdl::imtauth::Tenants::CGetTenantIdsGqlRequest& getTenantIdsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CGetTenantRelationshipsPayload OnGetTenantRelationships(
				const sdl::imtauth::Tenants::CGetTenantRelationshipsGqlRequest& getTenantRelationshipsRequest,
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
	virtual sdl::imtauth::Tenants::CAddTenantRelationshipPayload OnAddTenantRelationship(
				const sdl::imtauth::Tenants::CAddTenantRelationshipGqlRequest& addTenantRelationshipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CRemoveTenantRelationshipPayload OnRemoveTenantRelationship(
				const sdl::imtauth::Tenants::CRemoveTenantRelationshipGqlRequest& removeTenantRelationshipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_FACT(imtauth::ITenantInfo, m_tenantFactoryCompPtr);
};


} // namespace imtauthgql
