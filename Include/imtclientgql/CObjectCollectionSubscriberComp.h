#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtclientgql/CSubscriberCompBase.h>


namespace imtclientgql
{


class CObjectCollectionSubscriberComp: public CSubscriberCompBase
{
public:
	typedef CSubscriberCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionSubscriberComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Data model to be synchronized with the changes on the server side", true, "ObjectCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) override;

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtclientgql


