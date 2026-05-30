// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IUserManager.h>
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/ITenantConnectionRequest.h>
#include <imtauth/ICrossTenantMessage.h>
#include <imtauth/IOrderRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


class CTenantManagerControllerComp: public sdl::imtauth::Tenants::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Tenants::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantManagerControllerComp);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager providing CRUD operations", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for member operations", true, "MembershipManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
		I_ASSIGN(m_grantManagerCompPtr, "CrossOrgGrantManager", "Manager for cross-org grants", false, "CrossOrgGrantManager");
		I_ASSIGN(m_connectionRequestManagerCompPtr, "TenantConnectionRequestManager", "Manager for tenant connection requests", false, "TenantConnectionRequestManager");
		I_ASSIGN(m_messageBrokerCompPtr, "CrossTenantMessageBroker", "Broker for cross-tenant messages", false, "CrossTenantMessageBroker");
		I_ASSIGN(m_orderRequestManagerCompPtr, "OrderRequestManager", "Manager for order requests", false, "OrderRequestManager");
	I_END_COMPONENT;

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
	void extracted(sdl::imtauth::Tenants::CTenantData::V1_0 &tenantData,
				   QByteArrayList &membershipIds) const;
	virtual sdl::imtauth::Tenants::CGetTenantPayload OnGetTenant(
			const sdl::imtauth::Tenants::CGetTenantGqlRequest &getTenantRequest,
			const ::imtgql::CGqlRequest &gqlRequest,
			QString &errorMessage) const override;
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
	virtual sdl::imtauth::Tenants::CGetCrossOrgGrantsPayload OnGetCrossOrgGrants(
				const sdl::imtauth::Tenants::CGetCrossOrgGrantsGqlRequest& getCrossOrgGrantsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CCreateCrossOrgGrantPayload OnCreateCrossOrgGrant(
				const sdl::imtauth::Tenants::CCreateCrossOrgGrantGqlRequest& createCrossOrgGrantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CRevokeCrossOrgGrantPayload OnRevokeCrossOrgGrant(
				const sdl::imtauth::Tenants::CRevokeCrossOrgGrantGqlRequest& revokeCrossOrgGrantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CEnsureSystemTenantPayload OnEnsureSystemTenant(
				const sdl::imtauth::Tenants::CEnsureSystemTenantGqlRequest& ensureSystemTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CGetTenantConnectionRequestsPayload OnGetTenantConnectionRequests(
				const sdl::imtauth::Tenants::CGetTenantConnectionRequestsGqlRequest& getTenantConnectionRequestsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CCreateTenantConnectionRequestPayload OnCreateTenantConnectionRequest(
				const sdl::imtauth::Tenants::CCreateTenantConnectionRequestGqlRequest& createTenantConnectionRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CCreateTenantConnectCodePayload OnCreateTenantConnectCode(
				const sdl::imtauth::Tenants::CCreateTenantConnectCodeGqlRequest& createTenantConnectCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CAcceptTenantConnectionRequestPayload OnAcceptTenantConnectionRequest(
				const sdl::imtauth::Tenants::CAcceptTenantConnectionRequestGqlRequest& acceptTenantConnectionRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CAcceptTenantConnectCodePayload OnAcceptTenantConnectCode(
				const sdl::imtauth::Tenants::CAcceptTenantConnectCodeGqlRequest& acceptTenantConnectCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CRejectTenantConnectionRequestPayload OnRejectTenantConnectionRequest(
				const sdl::imtauth::Tenants::CRejectTenantConnectionRequestGqlRequest& rejectTenantConnectionRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CRevokeTenantConnectionRequestPayload OnRevokeTenantConnectionRequest(
				const sdl::imtauth::Tenants::CRevokeTenantConnectionRequestGqlRequest& revokeTenantConnectionRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CGetCrossTenantMessagePayload OnGetCrossTenantMessage(
				const sdl::imtauth::Tenants::CGetCrossTenantMessageGqlRequest& getCrossTenantMessageRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CGetCrossTenantMessagesPayload OnGetCrossTenantMessages(
				const sdl::imtauth::Tenants::CGetCrossTenantMessagesGqlRequest& getCrossTenantMessagesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CSendCrossTenantMessagePayload OnSendCrossTenantMessage(
				const sdl::imtauth::Tenants::CSendCrossTenantMessageGqlRequest& sendCrossTenantMessageRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CUpdateCrossTenantMessageStatusPayload OnUpdateCrossTenantMessageStatus(
				const sdl::imtauth::Tenants::CUpdateCrossTenantMessageStatusGqlRequest& updateCrossTenantMessageStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CGetOrderRequestPayload OnGetOrderRequest(
				const sdl::imtauth::Tenants::CGetOrderRequestGqlRequest& getOrderRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CGetOrderRequestsPayload OnGetOrderRequests(
				const sdl::imtauth::Tenants::CGetOrderRequestsGqlRequest& getOrderRequestsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CConfirmOrderRequestPayload OnConfirmOrderRequest(
				const sdl::imtauth::Tenants::CConfirmOrderRequestGqlRequest& confirmOrderRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CRejectOrderRequestPayload OnRejectOrderRequest(
				const sdl::imtauth::Tenants::CRejectOrderRequestGqlRequest& rejectOrderRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CUpdateOrderRequestStatusPayload OnUpdateOrderRequestStatus(
				const sdl::imtauth::Tenants::CUpdateOrderRequestStatusGqlRequest& updateOrderRequestStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtauth::ICrossOrgGrant, m_grantManagerCompPtr);
	I_REF(imtauth::ITenantConnectionRequest, m_connectionRequestManagerCompPtr);
	I_REF(imtauth::ICrossTenantMessage, m_messageBrokerCompPtr);
	I_REF(imtauth::IOrderRequest, m_orderRequestManagerCompPtr);
};


} // namespace imtauthgql
