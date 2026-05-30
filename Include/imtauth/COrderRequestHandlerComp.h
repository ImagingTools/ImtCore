// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <functional>

// ImtCore includes
#include <imtauth/IOrderRequest.h>
#include <imtauth/IOrderRequestData.h>
#include <imtauth/ICrossTenantMessage.h>
#include <imtauth/ICrossTenantMessageHandler.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Order-request domain handler (RFC phase 3, §15).

	Implements the first concrete cross-tenant scenario (Order -> OrderRequest)
	on both directions:
	- As an ICrossTenantMessageHandler input handler it processes delivered
	  CTMT_ORDER_REQUEST messages: it materializes a brand-new, local order
	  request object inside the target tenant from the (already projected)
	  payload and links it back to the message through its object ID.
	- As an IOrderRequest manager it exposes the status-feedback (output)
	  operations (confirm / reject / status update) that report the local
	  processing result back to the originating source tenant. The feedback is
	  delivered through the cross-tenant message broker as a new message in the
	  reverse direction, so the source tenant can update its own order without
	  ever gaining direct access to the target tenant's data.

	All references except the collection and factory are optional so the handler
	can be composed incrementally.
*/
class COrderRequestHandlerComp:
			public ilog::CLoggerComponentBase,
			virtual public ICrossTenantMessageHandler,
			virtual public IOrderRequest
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(COrderRequestHandlerComp);
		I_REGISTER_INTERFACE(ICrossTenantMessageHandler);
		I_REGISTER_INTERFACE(IOrderRequest);
		I_ASSIGN(m_orderRequestCollectionCompPtr, "OrderRequestCollection", "Order request collection", false, "OrderRequestCollection");
		I_ASSIGN(m_orderRequestFactoryCompPtr, "OrderRequestFactory", "Order request factory", false, "OrderRequestInfo");
		I_ASSIGN(m_brokerCompPtr, "MessageBroker", "Broker used to send status feedback back to the source tenant", false, "MessageBroker");
	I_END_COMPONENT;

	// reimplemented (ICrossTenantMessageHandler)
	virtual bool CanHandle(CrossTenantMessageType messageType, const QString& customType) const override;
	virtual bool HandleMessage(
				const CrossTenantMessageInfo& messageInfo,
				QByteArray& targetObjectId,
				QString& errorMessage) override;

	// reimplemented (IOrderRequest)
	virtual OrderRequestInfo GetOrderRequest(const QByteArray& orderRequestId) const override;
	virtual OrderRequests GetOrderRequests(const QByteArray& targetTenantId) const override;
	virtual OrderRequests GetOrderRequestsByRelationship(const QByteArray& relationshipId) const override;
	virtual bool ConfirmOrderRequest(const QByteArray& orderRequestId, const QString& note = QString()) override;
	virtual bool RejectOrderRequest(const QByteArray& orderRequestId, const QString& reason = QString()) override;
	virtual bool UpdateOrderRequestStatus(
				const QByteArray& orderRequestId,
				OrderRequestStatus status,
				const QString& note = QString()) override;

private:
	OrderRequests CollectOrderRequests(const std::function<bool(const OrderRequestInfo&)>& predicate) const;
	bool ApplyStatusFeedback(
				const QByteArray& orderRequestId,
				OrderRequestStatus status,
				CrossTenantMessageType feedbackType,
				const QString& note);

private:
	I_REF(imtbase::IObjectCollection, m_orderRequestCollectionCompPtr);
	I_FACT(imtauth::IOrderRequestData, m_orderRequestFactoryCompPtr);
	I_REF(imtauth::ICrossTenantMessage, m_brokerCompPtr);
};


} // namespace imtauth
