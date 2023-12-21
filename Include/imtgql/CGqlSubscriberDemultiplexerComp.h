#pragma once


// ImtCore includes
#include <imtgql/CGqlSubscriberControllerCompBase.h>


namespace imtgql
{


class CGqlSubscriberDemultiplexerComp: public imtgql::CGqlSubscriberControllerCompBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlSubscriberDemultiplexerComp);
		I_ASSIGN_MULTI_0(m_gqlSubscriberControllersCompPtr, "GqlSubscriberControllers", "Demultiplexer for GraphQL subscriber controllers", false)
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
	I_MULTIREF(imtgql::IGqlSubscriberController, m_gqlSubscriberControllersCompPtr);
};


} // namespace imtgql


