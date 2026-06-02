// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IUserManager.h>
<<<<<<< HEAD
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/IContract.h>
#include <imtauth/ITenantConnectionRequest.h>
#include <imtauth/ICrossTenantMessage.h>
#include <imtauth/IOrderRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>
=======
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants_fwd.h>
>>>>>>> origin/main


namespace imtauthgql
{


class CTenantManagerControllerComp: public sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantManagerControllerComp);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager providing CRUD operations", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for member operations", true, "MembershipManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
		I_ASSIGN(m_grantManagerCompPtr, "CrossOrgGrantManager", "Manager for cross-org grants", false, "CrossOrgGrantManager");
		I_ASSIGN(m_contractManagerCompPtr, "ContractManager", "Manager for cooperation contracts", false, "ContractManager");
		I_ASSIGN(m_connectionRequestManagerCompPtr, "TenantConnectionRequestManager", "Manager for tenant connection requests", false, "TenantConnectionRequestManager");
		I_ASSIGN(m_messageBrokerCompPtr, "CrossTenantMessageBroker", "Broker for cross-tenant messages", false, "CrossTenantMessageBroker");
		I_ASSIGN(m_orderRequestManagerCompPtr, "OrderRequestManager", "Manager for order requests", false, "OrderRequestManager");
	I_END_COMPONENT;

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
	void extracted(sdl::V1_0::imtauth::CTenantData &tenantData,
				   QByteArrayList &membershipIds) const;
	virtual sdl::V1_0::imtauth::CGetTenantPayload OnGetTenant(
			const sdl::V1_0::imtauth::CGetTenantGqlRequest &getTenantRequest,
			const ::imtgql::CGqlRequest &gqlRequest,
			QString &errorMessage) const override;
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
	virtual sdl::imtauth::Tenants::CGetContractsPayload OnGetContracts(
				const sdl::imtauth::Tenants::CGetContractsGqlRequest& getContractsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CCreateContractPayload OnCreateContract(
				const sdl::imtauth::Tenants::CCreateContractGqlRequest& createContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CUpdateContractStatusPayload OnUpdateContractStatus(
				const sdl::imtauth::Tenants::CUpdateContractStatusGqlRequest& updateContractStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CTerminateContractPayload OnTerminateContract(
				const sdl::imtauth::Tenants::CTerminateContractGqlRequest& terminateContractRequest,
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
	I_REF(imtauth::IContract, m_contractManagerCompPtr);
	I_REF(imtauth::ITenantConnectionRequest, m_connectionRequestManagerCompPtr);
	I_REF(imtauth::ICrossTenantMessage, m_messageBrokerCompPtr);
	I_REF(imtauth::IOrderRequest, m_orderRequestManagerCompPtr);
};


} // namespace imtauthgql
