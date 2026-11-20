// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CAccessChangePublisherComp.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CAccessChangePublisherComp::IsOpenFor(SubscriberKind subscriberKind, const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return subscriberKind == SK_USER || subscriberKind == SK_RECIPIENT;
}


bool CAccessChangePublisherComp::NotifyAccessChanged(const QByteArray& subscriptionId, const imtrest::IRequest& networkRequest) const
{
	return PushDataToSubscriber(subscriptionId, networkRequest.GetCommandId(), QByteArrayLiteral(R"({"changed": true})"), networkRequest);
}


} // namespace imtservergql


