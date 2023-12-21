#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtbase/ICommandPermissionsProvider.h>
#include <imtauth/IPermissionChecker.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/IRequestManager.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlSubscriberController.h>


namespace imtgql
{


class CGqlSubscriberControllerCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlSubscriberController,
			virtual public imtrest::IRequestEventHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlSubscriberControllerCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlSubscriberController);
		I_ASSIGN_MULTI_0(m_commandIdsAttrPtr, "CommandIds", "List of model-IDs for GraphQL-response", true);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
		I_ASSIGN(m_requestHandlerCompPtr, "GqlRequestHandler", "Graphql request handler to create the subscription body", false, "GqlRequestHandler");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Request manager registered for the server", true, "RequestManager");
		I_ASSIGN(m_requestHandlerCommandIdAtrPtr, "RequestHandlerCommandId", "Request handler commandId to create the subscription body", true, "");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const imtgql::CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) override;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId) override;

	// reimplemented (imtrest::IRequestEventHandler)
	virtual void OnRequestDestroyed(imtrest::IRequest* request) override;

protected:
	virtual bool SetSubscriptions();
	virtual bool SetAllSubscriptions(const QByteArray& data);
	virtual bool StartInternalSubscriber(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage);

protected:
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_requestHandlerCompPtr);
	I_REF(imtrest::IRequestManager, m_requestManagerCompPtr);
	I_ATTR(QByteArray, m_requestHandlerCommandIdAtrPtr);

	struct RequestNetworks
	{
		imtgql::CGqlRequest gqlRequest;
		QMap<QByteArray, const imtrest::IRequest*> networkRequests;

		RequestNetworks():gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION){}
	};
	
	QList<RequestNetworks> m_registeredSubscribers;
};


} // namespace imtgql


