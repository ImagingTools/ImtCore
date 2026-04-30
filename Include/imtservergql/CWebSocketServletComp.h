// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpRootServletComp.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtgql/IGqlSubscriberController.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlContextCreator.h>


namespace imtservergql
{


class CWebSocketServletComp:
			public ilog::CLoggerComponentBase,
			virtual public imtrest::IRequestServlet,
			virtual public imtrest::IResponseDispatcher
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketServletComp);
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN_MULTI_0(m_gqlSubscriberControllersCompPtr, "GqlSubscriberControllers", "List of gql subscriber controller for corresponding command-IDs", false);
		I_ASSIGN(m_workerManagerCompPtr, "WorkerManager", "GraphQl multithread worker manager", false, "WorkerManager");
		I_ASSIGN(m_gqlContextCreatorCompPtr, "GqlContextCreator", "GraphQL-related context creator", false, "GqlContextCreator");
	I_END_COMPONENT

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (imtrest::IResponseDispatcher)
	virtual bool SendResponse(const QByteArray& requestId, imtrest::ConstResponsePtr& response) const override;
	virtual bool SendRequest(const QByteArray& requestId, imtrest::ConstRequestPtr& request) const override;

protected:
	virtual imtrest::ConstResponsePtr InitConnection(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr KeepAliveAcknowledge(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr ProcessGqlRequest(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr RegisterSubscription(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr UnregisterSubscription(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr CreateDataResponse(const QByteArray& data, const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr CreateErrorResponse(const QByteArray& errorMessage, const imtrest::IRequest& request) const;

private:
	I_MULTIREF(imtgql::IGqlSubscriberController, m_gqlSubscriberControllersCompPtr);
	I_REF(imtrest::IRequestServlet, m_workerManagerCompPtr);
	I_REF(imtgql::IGqlContextCreator, m_gqlContextCreatorCompPtr);

	typedef QMap<QByteArray, imtgql::IGqlSubscriberController*> SubscriberControllersMap;

	SubscriberControllersMap m_subscriberControllersMap;
	mutable QMutex m_subscriberControllersMapMutex;
};


} // namespace imtservergql

