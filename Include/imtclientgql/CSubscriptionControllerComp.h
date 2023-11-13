#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionManager.h>

// Qt includes
#include <QtCore/QByteArray>


namespace imtclientgql
{


/**
	\todo Rename to CSubscriptionClientComp or CSubscriberComp
*/
class CSubscriptionControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtclientgql::IGqlSubscriptionClient
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSubscriptionControllerComp);
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionClient);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager", true, "SubscriptionManager");
		I_ASSIGN(m_changeableModelCompPtr, "Model", "Model", true, "Model");
		I_ASSIGN(m_commandAttrPtr, "Command", "Command", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(
				const QByteArray& subscriptionId,
				const QByteArray& subscriptionData) override;
	virtual void OnSubscriptionStatusChanged(
				const QByteArray& subscriptionId,
				const SubscriptionStatus& status,
				const QString& message) override;

private:
	I_REF(imtclientgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
	I_REF(istd::IChangeable, m_changeableModelCompPtr);
	I_ATTR(QByteArray, m_commandAttrPtr);

	QByteArray m_subscriptionId;
};


} // namespace imtclientgql


