#pragma once


// ImtCore includes
#include <imtservergql/CGqlSubscriberControllerCompBase.h>


namespace imtservergql
{


class CSlaveSubscriberControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlSubscriberController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSlaveSubscriberControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlSubscriberController)
		I_ASSIGN_MULTI_0(m_subscriberControllerListCompPtr, "SubscriberController", "Slave subscriber controller", false);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool RegisterSubscription(
		const QByteArray& subscriptionId,
		const imtgql::CGqlRequest& gqlRequest,
		const imtrest::IRequest& networkRequest,
		QString& errorMessage) override;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId) override;

protected:
	I_MULTIREF(imtgql::IGqlSubscriberController, m_subscriberControllerListCompPtr);
};


} // namespace imtservergql


