// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CCollectionDocumentManagerSubscriberComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CCollectionDocumentManagerSubscriberComp::OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData)
{
	if (!m_subscriptionIds.contains(subscriptionId)){
		return;
	}

	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	istd::CChangeNotifier notifier(this, &changeSet);
}


} // namespace imtclientgql


