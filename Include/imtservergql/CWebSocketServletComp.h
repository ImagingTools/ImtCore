// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QThreadPool>
#include <QtCore/QWaitCondition>

// ACF includes
#include <imod/CMultiModelObserverBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtrest/CHttpRootServletComp.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtgql/IGqlSubscriberController.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlContextCreator.h>
#include <imtservergql/CSubscriptionCheckSchedule.h>
#include <imtservergql/IServerTokenReceiver.h>
#include <imtservergql/imtservergql.h>


namespace imtservergql
{


class CWebSocketServletComp:
			public ilog::CLoggerComponentBase,
			virtual public imtrest::IRequestServlet,
			virtual public imtrest::IResponseDispatcher,
			virtual public imtrest::IRequestEventHandler,
			protected imod::CMultiModelObserverBase
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
		I_ASSIGN_MULTI_0(m_accessModelsCompPtr, "AccessModels", "Models whose changes may change the access of subscriptions: the subscriptions with credentials are checked again. Only models a check does not change", false);
		I_ASSIGN(m_serverTokenReceiverCompPtr, "ServerTokenReceiver", "Server tokens issued by this server: a change of one checks the subscriptions again", false, "ServerTokenReceiver");
		I_ASSIGN(m_maxCheckIntervalAttrPtr, "MaxCheckInterval", "Seconds after which the access of a subscription with credentials is checked again without a change; a change without an event is found this late", true, 300);
		I_ASSIGN(m_gracePeriodAttrPtr, "GracePeriod", "Seconds a subscription waits for its access to be confirmed once a check could not confirm it; then it is closed as temporarily unavailable", true, 60);
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
				bool logMessage = true,
				const QByteArray& reasonCode = QByteArray()) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelObserverBase)
	virtual void OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	struct TokenSubscription
	{
		QByteArray subscriptionId;
		imtgql::IGqlSubscriberController* controllerPtr = nullptr;

		// The credentials it was registered with, kept here: a client cannot change what its subscription is checked with
		imtgql::IGqlContext::Headers headers;

		// The access it was registered with: it is served with it while the current access holds all of it
		AuthorizationData registeredAccess;
		// Its channel goes to its recipient whatever its permissions (SK_RECIPIENT)
		bool isRecipientChannel = false;
		const imtrest::IRequest* requestPtr = nullptr;

		// Only compared: the request is destroyed with it
		const QObject* connectionPtr = nullptr;

	};

	bool IsSubscriberAllowed(const imtgql::IGqlSubscriberController& controller, const imtgql::CGqlRequest& gqlRequest) const;

	/**
		The context of a subscription built afresh; a check the access changed during is repeated a few times.
		\return Null when the access is refused or nothing is known; \c error tells which.
	*/
	imtgql::IGqlContextUniquePtr CreateCurrentContext(
				const imtgql::IGqlContext::Headers& headers,
				bool isEstablished,
				imtgql::IGqlContextCreator::ContextCreationError& error,
				QDateTime* sessionExpiresAtPtr = nullptr) const;
	/**
		Tell whether the access of the credentials of a new subscription still serves it, read afresh.
		\param reasonCode	Why not, see s_subscriptionUnauthorizedCode and its siblings.
	*/
	bool IsAuthorizationCurrent(
				const imtgql::IGqlContext::Headers& headers,
				const AuthorizationData& registeredAccess,
				bool isRecipientChannel,
				QString& errorMessage,
				QByteArray& reasonCode) const;
	/**
		Tell whether a subscription is still served with the access it was registered with: the current access holds all of it,
		or its channel goes to its recipient, which is the same.
	*/
	static bool IsServedBy(const AuthorizationData& registeredAccess, bool isRecipientChannel, const AuthorizationData& currentAccess);
	/// The machine code of a refusal of credentials.
	static QByteArray GetRefusalCode(const imtgql::IGqlContextCreator::ContextCreationError& error);
	static QByteArray GetCredentialKey(const imtgql::IGqlContext::Headers& headers);
	/// A time of the wall clock on the monotonic clock of the schedule; -1 for an invalid one.
	qint64 ToScheduleTime(const QDateTime& time, qint64 nowMs) const;

	void AddTokenSubscription(const TokenSubscription& subscription, const QDateTime& sessionExpiresAt, quint64 accessGeneration) const;
	void RemoveTokenSubscriptions(const std::function<bool(const TokenSubscription&)>& predicate) const;
	/// The thread of the schedule: it starts the checks due and ends the subscriptions whose grace period is over.
	void RunTokenChecks() const;
	/// A check of a set of credentials, on a thread of the pool: it may wait for the network.
	void CheckCredentials(const QByteArray& credentialKey, qint64 attemptId, const imtgql::IGqlContext::Headers& headers) const;
	/// Observe a model of the access once.
	void ObserveAccessModel(imod::IModel* modelPtr);
	/// End a subscription; the lock of the subscriptions is held.
	void DropTokenSubscriptionLocked(const TokenSubscription& subscription, const QByteArray& reasonCode, const QString& reason) const;
	/// End every subscription of a set of credentials and stop checking it; the lock of the subscriptions is held.
	void DropCredentialsLocked(const QByteArray& credentialKey, const QByteArray& reasonCode, const QString& reason) const;

	I_MULTIREF(imtgql::IGqlSubscriberController, m_gqlSubscriberControllersCompPtr);
	I_REF(imtrest::IRequestServlet, m_workerManagerCompPtr);
	I_REF(imtgql::IGqlContextCreator, m_gqlContextCreatorCompPtr);
	I_REF(imtrest::IResponseDispatcher, m_requestManagerCompPtr);
	I_ATTR(bool, m_isAnonymousDeniedAttrPtr);
	I_ATTR(bool, m_isUndeclaredUserChannelDeniedAttrPtr);
	I_MULTIREF(imod::IModel, m_accessModelsCompPtr);
	I_REF(IServerTokenReceiver, m_serverTokenReceiverCompPtr);
	I_ATTR(int, m_maxCheckIntervalAttrPtr);
	I_ATTR(int, m_gracePeriodAttrPtr);

	mutable QHash<QByteArray, QList<TokenSubscription>> m_tokenSubscriptions;
	mutable QMutex m_tokenSubscriptionsMutex;
	quint64 m_accessChangeGeneration = 0;
	mutable QWaitCondition m_tokenCheckCondition;
	mutable QThread* m_tokenCheckThreadPtr = nullptr;
	mutable bool m_isTokenCheckStopped = false;
	mutable CSubscriptionCheckSchedule m_checkSchedule;
	QElapsedTimer m_clock;
	mutable QThreadPool m_checkPool;

};


} // namespace imtservergql
