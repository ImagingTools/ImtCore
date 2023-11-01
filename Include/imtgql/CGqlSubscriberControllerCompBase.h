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
			virtual public imtrest::CWebSocketRequest::IDestroyObserver
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlSubscriberControllerCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlSubscriberController);
		I_ASSIGN_MULTI_0(m_commandIdsAttrPtr, "CommandIds", "List of model-IDs for GraphQL-response", true);
		I_ASSIGN(m_commandPermissionsCompPtr, "CommandPermissions", "Command Permissions", false, "Command Permissions");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
		I_ASSIGN(m_requestHandlerCompPtr, "GqlRequestHandler", "Graphql request handler to create the subscription body", false, "GqlRequestHandler");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Request manager registered for the server", true, "RequestManager");
		I_ASSIGN(m_requestHandlerCommandIdAtrPtr, "RequestHandlerCommandId", "Request handler commandId to create the subscription body", true, "");
	I_END_COMPONENT;

	CGqlSubscriberControllerCompBase();

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool RegisterSubscribtion(
				const QByteArray& subscriptionId,
				const imtgql::CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) override;
	virtual bool UnRegisterSubscribtion(const QByteArray& subscriptionId) override;

	// reimplemented (imtrest::CWebSocketRequest::IDestroyObserver)
	virtual void OnRequestDestroyed(imtrest::CWebSocketRequest* webSocketRequest);

protected:
	virtual bool SetSubscriptions();
	virtual bool StartInternalSubscriber(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage);
	virtual iprm::IParamsSet* CreateContextParams(const imtgql::CGqlRequest& gqlRequest) const;
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

protected:
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_REF(imtbase::ICommandPermissionsProvider, m_commandPermissionsCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_requestHandlerCompPtr);
	I_REF(imtrest::IRequestManager, m_requestManagerCompPtr);
	I_ATTR(QByteArray, m_requestHandlerCommandIdAtrPtr);

	struct RequestNetworks
	{
		imtgql::CGqlRequest gqlRequest;
		QMap<QByteArray, const imtrest::IRequest*> networkRequests;
	};
	
	QList<RequestNetworks> m_registeredSubscribers;
};


} // namespace imtgql


