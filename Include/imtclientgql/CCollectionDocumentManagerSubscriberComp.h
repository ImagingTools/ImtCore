// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/CSubscriberCompBase.h>


namespace imtclientgql
{


class CCollectionDocumentManagerSubscriberComp: public CSubscriberCompBase
{
public:
	typedef CSubscriberCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentManagerSubscriberComp);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID for filtering", true, "DummyCollection");
	I_END_COMPONENT;

	QByteArray GetLastNotificationData() const;

protected:
	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) override;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);

	QByteArray m_lastNotificationData;
};


} // namespace imtclientgql


