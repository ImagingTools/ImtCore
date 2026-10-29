// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtauth/IAccessTokenProvider.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/ITransport.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISubscriptionMessageHandler.h>
#include <imtgql/CGqlRequest.h>
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtclientgql
{


/**
	Subscription manager for GQL subscriptions carried over a WebSocket transport.

	This component is modeled after \c CSubscriptionManagerComp but is independent
	from it (no inheritance relation). The main difference is the way incoming
	WebSocket messages are delivered: instead of implementing
	\c imtrest::IRequestServlet and parsing an \c imtrest::CWebSocketRequest,
	this component implements \c imtrest::ISubscriptionMessageHandler and
	receives the subscription ID, method type and raw message already split
	up by the caller.
*/
class CWebSocketSubscriptionManagerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imod::CSingleModelObserverBase,
			virtual public IGqlSubscriptionManager,
			virtual public imtrest::ISubscriptionMessageHandler
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	CWebSocketSubscriptionManagerComp();

	I_BEGIN_COMPONENT(CWebSocketSubscriptionManagerComp);
		I_REGISTER_INTERFACE(IGqlSubscriptionManager);
		I_REGISTER_INTERFACE(imtrest::ISubscriptionMessageHandler);
		I_ASSIGN(m_subscriptionSenderCompPtr, "SubscriptionSender", "Subscription sender", false, "SubscriptionSender");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Response dispatcher for sending a request", false, "RequestManager");
		I_ASSIGN(m_engineCompPtr, "ProtocolEngine", "Protocol engine for subscription", true, "ProtocolEngine");
		I_ASSIGN(m_connectionStatusProviderCompPtr, "WebLoginStatus", "Web login status", false, "WebLoginStatus");
		I_ASSIGN_TO(m_connectionStatusProviderModelCompPtr, m_connectionStatusProviderCompPtr, true);
		I_ASSIGN(m_accessTokenProviderCompPtr, "AccessTokenProvider", "Provider of the current access token", false, "AccessTokenProvider");
		I_ASSIGN_TO(m_accessTokenProviderModelCompPtr, m_accessTokenProviderCompPtr, false);
	I_END_COMPONENT;

	// reimplemented (imtclientgql::IGqlSubscriptionManager)
	virtual QByteArray RegisterSubscription(
			const imtgql::IGqlRequest& subscriptionRequest,
			imtclientgql::IGqlSubscriptionClient& subscriptionClient) override;
	virtual bool UnregisterSubscription(
			const QByteArray& subscriptionId,
			const imtclientgql::IGqlSubscriptionClient& subscriptionClient) override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imtrest::ISubscriptionMessageHandler)
	virtual imtrest::ConstResponsePtr ProcessMessage(
			const QByteArray& subscriptionId,
			const QByteArray& methodType,
			const QByteArray& message) const override;

protected:
	virtual bool SubscriptionRegister(const imtgql::CGqlRequest& subscriptionRequest, const QByteArray& subscriptionId) const;
	virtual bool SubscriptionUnregister(const imtgql::CGqlRequest& subscriptionRequest, const QByteArray& subscriptionId) const;
	virtual bool SendRequestInternal(const imtgql::IGqlRequest& request, imtrest::ConstRequestPtr& requestPtr) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	/**
		Re-send the "start" message of every known subscription.
		The server binds a subscription to the identity presented at registration
		time, so a replaced access token has to be carried to it explicitly.
		Every re-registration is preceded by a "stop" message with the same
		subscription ID, because server side controllers append a new
		registration instead of replacing the existing one.
	*/
	void ReregisterSubscriptions() const;

	class AccessTokenObserver: public imod::CSingleModelObserverBase
	{
	public:
		AccessTokenObserver(CWebSocketSubscriptionManagerComp& parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CWebSocketSubscriptionManagerComp& m_parent;
	};

	/**
		Minimal \c imtrest::IRequestServlet implementation used only to satisfy
		\c imtrest::IProtocolEngine::CreateRequestForSend, which requires a
		handler reference. Incoming messages are never routed through it: they
		reach this component via \c ProcessMessage instead.
	*/
	class RequestServletStub: virtual public imtrest::IRequestServlet
	{
	public:
		// reimplemented (imtrest::IRequestServlet)
		virtual bool IsCommandSupported(const QByteArray& commandId) const override;
		virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;
	};

private:
	void UpdateCustomerSubscriptionStatuses(const QByteArray& subscriptionId, const QString& message = QString()) const;

	class SubscriptionHelper
	{
	public:
		imtgql::CGqlRequest m_request;
		QByteArray m_clientId;
		IGqlSubscriptionClient::SubscriptionStatus m_status;
		QList<IGqlSubscriptionClient*> m_clients;
	};

private:
	I_REF(imtrest::ITransport, m_subscriptionSenderCompPtr);
	I_REF(imtrest::IResponseDispatcher, m_requestManagerCompPtr);
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusProviderCompPtr);
	I_REF(imod::IModel, m_connectionStatusProviderModelCompPtr);
	I_REF(imtauth::IAccessTokenProvider, m_accessTokenProviderCompPtr);
	I_REF(imod::IModel, m_accessTokenProviderModelCompPtr);
	I_REF(imtrest::IProtocolEngine, m_engineCompPtr);

	AccessTokenObserver m_accessTokenObserver;
	RequestServletStub m_requestServletStub;

	mutable QMap<QByteArray, SubscriptionHelper> m_registeredClients;
	mutable QMutex m_registeredClientsMutex;
};


} // namespace imtclientgql
