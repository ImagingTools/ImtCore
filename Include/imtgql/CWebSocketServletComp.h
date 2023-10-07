#pragma once


// ImtCore includes
#include <imtrest/CHttpRootServletComp.h>
#include <imtgql/IGqlSubscriberController.h>


namespace imtgql
{


class CWebSocketServletComp:
			public ilog::CLoggerComponentBase,
			virtual public imtrest::IRequestServlet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketServletComp);
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN_MULTI_0(m_gqlSubscriberControllersCompPtr, "GqlSubscriberControllers", "List of gql subscriber controller for corresponding command-IDs", false);
	I_END_COMPONENT

	// reimplemented (IRequestServlet)
	virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

protected:
	virtual imtrest::ConstResponsePtr InitConnection(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr RegisterSubscription(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr UnregisterSubscription(const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr CreateDataResponse(QByteArray data, const imtrest::IRequest& request) const;
	virtual imtrest::ConstResponsePtr CreateErrorResponse(QByteArray errorMessage, const imtrest::IRequest& request) const;

private:
	I_MULTIREF(IGqlSubscriberController, m_gqlSubscriberControllersCompPtr);

	typedef QMap<QByteArray, IGqlSubscriberController*> SubscriberControllersMap;

	SubscriberControllersMap m_subscriberControllersMap;
	mutable QMutex m_subscriberControllersMapMutex;
};


} // namespace imtgql


