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
	virtual imtrest::IRequestServlet::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

protected:
	virtual ConstResponsePtr InitConnection(const imtrest::IRequest& request) const;
	virtual ConstResponsePtr RegisterSubscription(const imtrest::IRequest& request) const;
	virtual ConstResponsePtr UnRegisterSubscription(const imtrest::IRequest& request) const;
	virtual ConstResponsePtr CreateDataResponse(QByteArray data, const imtrest::IRequest& request) const;
	virtual ConstResponsePtr CreateErrorResponse(QByteArray errorMessage, const imtrest::IRequest& request) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIREF(IGqlSubscriberController, m_gqlSubscriberControllersCompPtr);

	typedef QMap<QByteArray, IGqlSubscriberController*> SubscriberControllersMap;

	SubscriberControllersMap m_subscriberControllersMap;
	mutable QMutex m_subscriberControllersMapMutex;
};


} // namespace imtgql


