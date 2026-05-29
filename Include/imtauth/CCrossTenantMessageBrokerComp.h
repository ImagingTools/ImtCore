// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <functional>

// ImtCore includes
#include <imtauth/ICrossTenantMessage.h>
#include <imtauth/ICrossTenantMessageData.h>
#include <imtauth/ICrossTenantMessageProjection.h>
#include <imtauth/ICrossTenantMessageHandler.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Cross-tenant message broker (RFC phase 2, §10/§13).

	Implements ICrossTenantMessage on top of an IObjectCollection. On SendMessage
	the broker:
	- validates that an active relationship identified by the supplied
	  relationship ID exists between source and target tenant (when a tenant
	  manager is wired);
	- applies the configured projection to the payload (when a projection is
	  wired);
	- stores the message; and
	- dispatches it to the first registered handler that declares it can process
	  the message type, advancing the message status accordingly.

	All references except the collection and factory are optional so the broker
	can be composed incrementally as the domain handlers are added.
*/
class CCrossTenantMessageBrokerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ICrossTenantMessage
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCrossTenantMessageBrokerComp);
		I_REGISTER_INTERFACE(imtauth::ICrossTenantMessage);
		I_ASSIGN(m_messageCollectionCompPtr, "MessageCollection", "Cross-tenant message collection", false, "MessageCollection");
		I_ASSIGN(m_messageFactoryCompPtr, "MessageFactory", "Cross-tenant message factory", false, "CrossTenantMessageInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager used for relationship validation", false, "TenantManager");
		I_ASSIGN(m_projectionCompPtr, "Projection", "Projection applied to outgoing payloads", false, "Projection");
		I_ASSIGN_MULTI_0(m_handlersCompPtr, "Handlers", "Domain handlers dispatched for delivered messages", false);
	I_END_COMPONENT;

	virtual QByteArray SendMessage(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& relationshipId,
				CrossTenantMessageType messageType,
				const QByteArray& payload,
				const QByteArray& sourceObjectId = QByteArray(),
				const QString& customType = QString(),
				const QString& expiresAt = QString()) override;
	virtual CrossTenantMessageInfo GetMessage(const QByteArray& messageId) const override;
	virtual CrossTenantMessages GetIncomingMessages(const QByteArray& targetTenantId) const override;
	virtual CrossTenantMessages GetOutgoingMessages(const QByteArray& sourceTenantId) const override;
	virtual CrossTenantMessages GetMessagesByRelationship(const QByteArray& relationshipId) const override;
	virtual bool UpdateMessageStatus(
				const QByteArray& messageId,
				CrossTenantMessageStatus status,
				const QString& errorMessage = QString()) override;
	virtual bool SetTargetObjectId(const QByteArray& messageId, const QByteArray& targetObjectId) override;

private:
	CrossTenantMessages CollectMessages(const std::function<bool(const CrossTenantMessageInfo&)>& predicate) const;
	bool StoreMessageInfo(const QByteArray& messageId, const CrossTenantMessageInfo& info);
	bool ValidateRelationship(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& relationshipId) const;
	void DispatchToHandlers(CrossTenantMessageInfo& info);

private:
	I_REF(imtbase::IObjectCollection, m_messageCollectionCompPtr);
	I_FACT(imtauth::ICrossTenantMessageData, m_messageFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ICrossTenantMessageProjection, m_projectionCompPtr);
	I_MULTIREF(imtauth::ICrossTenantMessageHandler, m_handlersCompPtr);
};


} // namespace imtauth
