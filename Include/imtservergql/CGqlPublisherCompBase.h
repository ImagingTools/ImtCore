// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>

// Qt includes
#include <QtCore/QPointer>

// ImtCore includes
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlSubscriberController.h>


namespace imtservergql
{


class CGqlPublisherCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlSubscriberController,
			virtual public imtrest::IRequestEventHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlPublisherCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlSubscriberController);
		I_ASSIGN_MULTI_0(m_commandIdsAttrPtr, "CommandIds", "List of model-IDs for GraphQL-response", true);
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Response dispatcher registered for the server", true, "RequestManager");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const imtgql::CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) override;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) override;

	// reimplemented (imtrest::IRequestEventHandler)
	virtual void OnRequestDestroyed(imtrest::IRequest* request) override;

protected:
	virtual bool PushDataToSubscriber(const QByteArray& subscriptionId, const QByteArray& commandId, const QByteArray& data, const imtrest::IRequest& networkRequest, const bool useAwsStyle = true) const;
	virtual bool PublishData(const QByteArray& commandId, const QByteArray& data) const;
	virtual bool PublishDataFiltered(
		const QByteArray& commandId,
		const QByteArray& data,
		std::function<bool(const imtgql::CGqlRequest&)> predicate) const;

protected:
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_REF(imtrest::IResponseDispatcher, m_requestManagerCompPtr);

	struct RequestNetworks
	{
		imtgql::CGqlRequest gqlRequest;
		QMap<QByteArray, const imtrest::IRequest*> networkRequests; // Subscription-ID -> NetworkRequest
		// Lifetime guards for each network request (keyed by Subscription-ID).
		// CWebSocketRequest is a QObject; QPointer auto-clears when it is destroyed,
		// so PublishDataFiltered can detect a dangling pointer and skip sending —
		// closing the race between subscription teardown (OnSocketDisconnected /
		// ~CWebSocketRequest → UnregisterSubscription) and concurrent publishing.
		QMap<QByteArray, QPointer<QObject>> requestLifetimeGuards;

		RequestNetworks()
			:gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION)
		{
		}
	};

	mutable QList<RequestNetworks> m_registeredSubscribers;
	mutable QMutex m_mutex;
};


} // namespace imtservergql


