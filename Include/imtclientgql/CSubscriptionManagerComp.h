// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFuture>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QPromise>
#include <QtCore/QReadWriteLock>
#include <QtCore/QTimer>

// stdlib
#include <memory>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>
#include <imtrest/ITransport.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtgql/CGqlRequest.h>
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtclientgql
{


/**
	Subscription manager and **asynchronous** outbound GQL client over WebSocket.

	Implements \c IAsyncGqlClient only (no nested event loop).
	For synchronous \c IGqlClient semantics, wire \c CGqlClientSyncAdapterComp
	on top of this component (see WebSocketServerFramework).
*/
class CSubscriptionManagerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imod::CSingleModelObserverBase,
			virtual public IGqlSubscriptionManager,
			virtual public imtrest::IRequestServlet,
			virtual public IAsyncGqlClient
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	typedef IAsyncGqlClient::GqlRequestPtr GqlRequestPtr;
	typedef IAsyncGqlClient::GqlResponsePtr GqlResponsePtr;

	I_BEGIN_COMPONENT(CSubscriptionManagerComp);
		I_REGISTER_INTERFACE(IGqlSubscriptionManager);
		I_REGISTER_INTERFACE(IAsyncGqlClient);
		I_REGISTER_INTERFACE(imtrest::IRequestServlet);
		I_ASSIGN(m_subscriptionSenderCompPtr, "SubscriptionSender", "Subscription sender", false, "SubscriptionSender");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Response dispatcher for sending a request", false, "RequestManager");
		I_ASSIGN(m_engineCompPtr, "ProtocolEngine", "Protocol engine for subscription", true, "ProtocolEngine");
		I_ASSIGN(m_connectionStatusProviderCompPtr, "WebLoginStatus", "Web login status", false, "WebLoginStatus");
		I_ASSIGN_TO(m_connectionStatusProviderModelCompPtr, m_connectionStatusProviderCompPtr, true);
		I_ASSIGN(m_requestTimeoutMsAttrPtr, "RequestTimeoutMs", "Async request timeout (ms)", false, 100000);
	I_END_COMPONENT;

	// reimplemented (imtclientgql::IGqlSubscriptionManager)
	virtual QByteArray RegisterSubscription(const imtgql::IGqlRequest& subscriptionRequest, imtclientgql::IGqlSubscriptionClient * subscriptionClientPtr) override;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (IAsyncGqlClient)
	virtual QFuture<GqlResponsePtr> SendRequest(
				GqlRequestPtr requestPtr,
				IAsyncGqlResponseHandler* handlerPtr,
				imtbase::IUrlParam* urlParamPtr = nullptr) const override;

protected:
	virtual bool SubscriptionRegister(const imtgql::CGqlRequest& subscriptionRequest, const QByteArray& subscriptionId) const;
	virtual bool SendRequestInternal(const imtgql::IGqlRequest& request, imtrest::ConstRequestPtr& requestPtr) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

Q_SIGNALS:
	void OnQueryDataReceived(int resultCode = 1) const;

private:
	void UpdateCustomerSubscriptionStatuses(const QByteArray& subscriptionId, const QString& message = QString()) const;
	virtual imtrest::ConstResponsePtr CreateErrorResponse(const QByteArray& errorMessage, const imtrest::IRequest& request) const;

	struct PendingAsync
	{
		std::shared_ptr<QPromise<GqlResponsePtr>> promisePtr;
		IAsyncGqlResponseHandler* handlerPtr = nullptr;
		GqlRequestPtr requestPtr;
	};

	void CompletePending(const QString& key, const QByteArray& body, bool isError) const;
	void FailPending(const QString& key, IAsyncGqlResponseHandler::ErrorCategory category, const QString& message) const;

private:
	I_REF(imtrest::ITransport, m_subscriptionSenderCompPtr);
	I_REF(imtrest::IResponseDispatcher, m_requestManagerCompPtr);
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusProviderCompPtr);
	I_REF(imod::IModel, m_connectionStatusProviderModelCompPtr);
	I_REF(imtrest::IProtocolEngine, m_engineCompPtr);
	I_ATTR(int, m_requestTimeoutMsAttrPtr);

	class SubscriptionHelper
	{
	public:
		imtgql::CGqlRequest m_request;
		QByteArray m_clientId;
		IGqlSubscriptionClient::SubscriptionStatus m_status;
		QList<IGqlSubscriptionClient*> m_clients;
	};

	mutable QMap <QByteArray, SubscriptionHelper> m_registeredClients;
	mutable QMutex m_registeredClientsMutex;

	mutable QHash<QString, PendingAsync> m_pendingAsync;
	mutable QMutex m_pendingAsyncMutex;
	int m_requestTimeoutMs = 100000;
};


} // namespace imtclientgql
