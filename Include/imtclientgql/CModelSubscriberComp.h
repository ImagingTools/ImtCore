#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/CSubscriberCompBase.h>


namespace imtclientgql
{


class CModelSubscriberComp: public CSubscriberCompBase
{
public:
	typedef CSubscriberCompBase BaseClass;

	I_BEGIN_COMPONENT(CModelSubscriberComp);
		I_ASSIGN(m_changeableModelCompPtr, "Model", "Model", true, "Model");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) override;

private:
	I_REF(istd::IChangeable, m_changeableModelCompPtr);
};


} // namespace imtclientgql


