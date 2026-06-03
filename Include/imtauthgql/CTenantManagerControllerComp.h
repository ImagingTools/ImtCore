// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IUserManager.h>
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/IContract.h>
#include <imtauth/ITenantConnectionRequest.h>
#include <imtauth/ICrossTenantMessage.h>
#include <imtauth/IOrderRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


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
	virtual sdl::V1_0::imtauth::CGetCrossOrgGrantsPayload OnGetCrossOrgGrants(
				const sdl::V1_0::imtauth::CGetCrossOrgGrantsGqlRequest& getCrossOrgGrantsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateCrossOrgGrantPayload OnCreateCrossOrgGrant(
				const sdl::V1_0::imtauth::CCreateCrossOrgGrantGqlRequest& createCrossOrgGrantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRevokeCrossOrgGrantPayload OnRevokeCrossOrgGrant(
				const sdl::V1_0::imtauth::CRevokeCrossOrgGrantGqlRequest& revokeCrossOrgGrantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetContractsPayload OnGetContracts(
				const sdl::V1_0::imtauth::CGetContractsGqlRequest& getContractsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateContractPayload OnCreateContract(
				const sdl::V1_0::imtauth::CCreateContractGqlRequest& createContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUpdateContractStatusPayload OnUpdateContractStatus(
				const sdl::V1_0::imtauth::CUpdateContractStatusGqlRequest& updateContractStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CTerminateContractPayload OnTerminateContract(
				const sdl::V1_0::imtauth::CTerminateContractGqlRequest& terminateContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CEnsureSystemTenantPayload OnEnsureSystemTenant(
				const sdl::V1_0::imtauth::CEnsureSystemTenantGqlRequest& ensureSystemTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetTenantConnectionRequestsPayload OnGetTenantConnectionRequests(
				const sdl::V1_0::imtauth::CGetTenantConnectionRequestsGqlRequest& getTenantConnectionRequestsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetTenantConnectCodeDetailsPayload OnGetTenantConnectCodeDetails(
				const sdl::V1_0::imtauth::CGetTenantConnectCodeDetailsGqlRequest& getTenantConnectCodeDetailsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateTenantConnectionRequestPayload OnCreateTenantConnectionRequest(
				const sdl::V1_0::imtauth::CCreateTenantConnectionRequestGqlRequest& createTenantConnectionRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateTenantConnectCodePayload OnCreateTenantConnectCode(
				const sdl::V1_0::imtauth::CCreateTenantConnectCodeGqlRequest& createTenantConnectCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAcceptTenantConnectionRequestPayload OnAcceptTenantConnectionRequest(
				const sdl::V1_0::imtauth::CAcceptTenantConnectionRequestGqlRequest& acceptTenantConnectionRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAcceptTenantConnectCodePayload OnAcceptTenantConnectCode(
				const sdl::V1_0::imtauth::CAcceptTenantConnectCodeGqlRequest& acceptTenantConnectCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRejectTenantConnectionRequestPayload OnRejectTenantConnectionRequest(
				const sdl::V1_0::imtauth::CRejectTenantConnectionRequestGqlRequest& rejectTenantConnectionRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRevokeTenantConnectionRequestPayload OnRevokeTenantConnectionRequest(
				const sdl::V1_0::imtauth::CRevokeTenantConnectionRequestGqlRequest& revokeTenantConnectionRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetCrossTenantMessagePayload OnGetCrossTenantMessage(
				const sdl::V1_0::imtauth::CGetCrossTenantMessageGqlRequest& getCrossTenantMessageRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetCrossTenantMessagesPayload OnGetCrossTenantMessages(
				const sdl::V1_0::imtauth::CGetCrossTenantMessagesGqlRequest& getCrossTenantMessagesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSendCrossTenantMessagePayload OnSendCrossTenantMessage(
				const sdl::V1_0::imtauth::CSendCrossTenantMessageGqlRequest& sendCrossTenantMessageRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusPayload OnUpdateCrossTenantMessageStatus(
				const sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusGqlRequest& updateCrossTenantMessageStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetOrderRequestPayload OnGetOrderRequest(
				const sdl::V1_0::imtauth::CGetOrderRequestGqlRequest& getOrderRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetOrderRequestsPayload OnGetOrderRequests(
				const sdl::V1_0::imtauth::CGetOrderRequestsGqlRequest& getOrderRequestsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CConfirmOrderRequestPayload OnConfirmOrderRequest(
				const sdl::V1_0::imtauth::CConfirmOrderRequestGqlRequest& confirmOrderRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRejectOrderRequestPayload OnRejectOrderRequest(
				const sdl::V1_0::imtauth::CRejectOrderRequestGqlRequest& rejectOrderRequestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUpdateOrderRequestStatusPayload OnUpdateOrderRequestStatus(
				const sdl::V1_0::imtauth::CUpdateOrderRequestStatusGqlRequest& updateOrderRequestStatusRequest,
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
