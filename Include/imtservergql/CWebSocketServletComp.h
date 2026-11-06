// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

// ImtCore includes
#include <imtrest/CHttpRootServletComp.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtgql/IGqlSubscriberController.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlContextCreator.h>


namespace imtservergql
{


class CWebSocketServletComp:
			public ilog::CLoggerComponentBase,
			virtual public imtrest::IRequestServlet,
			virtual public imtrest::IResponseDispatcher,
			virtual public imtrest::IRequestEventHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketServletComp);
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN_MULTI_0(m_gqlSubscriberControllersCompPtr, "GqlSubscriberControllers", "List of gql subscriber controller for corresponding command-IDs", false);
		I_ASSIGN(m_workerManagerCompPtr, "WorkerManager", "GraphQl multithread worker manager", false, "WorkerManager");
		I_ASSIGN(m_gqlContextCreatorCompPtr, "GqlContextCreator", "GraphQL-related context creator", false, "AuthenticationManager");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Response dispatcher of the server, tells a client its subscription was dropped", false, "RequestManager");
		I_ASSIGN(m_isAnonymousDeniedAttrPtr, "DenyAnonymousSubscriptions", "Only channels declared open take a subscriber who has not logged in. For a server with authorization", true, false);
		I_ASSIGN(m_isUndeclaredUserChannelDeniedAttrPtr, "DenyUndeclaredUserSubscriptions", "A user subscribes only to channels that declare who reads them", true, false);
		I_ASSIGN(m_tokenCheckIntervalAttrPtr, "TokenCheckInterval", "Interval in seconds of checking the access tokens that hold subscriptions", true, 30);
	I_END_COMPONENT

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (imtrest::IResponseDispatcher)
	virtual bool SendResponse(const QByteArray& requestId, imtrest::ConstResponsePtr& response) const override;
	virtual bool SendRequest(const QByteArray& requestId, imtrest::ConstRequestPtr& request) const override;

	// reimplemented (imtrest::IRequestEventHandler)
	virtual void OnRequestDestroyed(imtrest::IRequest* request) override;

protected:
	virtual imtrest::ConstResponsePtr InitConnection(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr KeepAliveAcknowledge(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr ProcessGqlRequest(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr RegisterSubscription(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr UnregisterSubscription(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr CreateDataResponse(const QByteArray& data, const imtrest::IRequest& request) const;
	/**
		Build a GraphQL-over-WebSocket error frame for \p request.
		\param errorMessage human-readable reason (must already include any status prefix).
		\param logMessage when true (default), also write \p errorMessage to the component log.
			Callers that already logged a richer line (e.g. auth failure with masked token)
			should pass false to avoid a second, less useful log entry.
	*/
	virtual imtrest::ConstResponsePtr CreateErrorResponse(
				const QByteArray& errorMessage,
				const imtrest::IRequest& request,
				bool logMessage = true) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

private:
	struct TokenSubscription
	{
		QByteArray subscriptionId;
		imtgql::IGqlSubscriberController* controllerPtr = nullptr;
		QByteArray token;
		const imtrest::IRequest* requestPtr = nullptr;

		// Only compared: the request is destroyed with it
		const QObject* connectionPtr = nullptr;
	};

	bool IsSubscriberAllowed(const imtgql::IGqlSubscriberController& controller, const imtgql::CGqlRequest& gqlRequest) const;
	void AddTokenSubscription(const TokenSubscription& subscription) const;
	void CheckTokenSubscriptions() const;
	void DropTokenSubscription(const TokenSubscription& subscription, const QString& reason) const;

	I_MULTIREF(imtgql::IGqlSubscriberController, m_gqlSubscriberControllersCompPtr);
	I_REF(imtrest::IRequestServlet, m_workerManagerCompPtr);
	I_REF(imtgql::IGqlContextCreator, m_gqlContextCreatorCompPtr);
	I_REF(imtrest::IResponseDispatcher, m_requestManagerCompPtr);
	I_ATTR(bool, m_isAnonymousDeniedAttrPtr);
	I_ATTR(bool, m_isUndeclaredUserChannelDeniedAttrPtr);
	I_ATTR(int, m_tokenCheckIntervalAttrPtr);

	mutable QList<TokenSubscription> m_tokenSubscriptions;
	mutable QMutex m_tokenSubscriptionsMutex;
	mutable QWaitCondition m_tokenCheckStopCondition;
	mutable QThread* m_tokenCheckThreadPtr = nullptr;
	mutable bool m_isTokenCheckStopped = false;

	typedef QMap<QByteArray, imtgql::IGqlSubscriberController*> SubscriberControllersMap;

	SubscriberControllersMap m_subscriberControllersMap;
	mutable QMutex m_subscriberControllersMapMutex;
};


} // namespace imtservergql

