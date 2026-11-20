// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


/**
	The channel telling a signed-in user that the own access changed (\c OnAccessChanged): permissions, roles, groups or
	the organization of the session. The WebSocket servlet finds the change when it checks the subscriptions again.
	Each message carries \c {"changed": true}; the client reads its permissions again on it.
	The channel goes to its recipient whatever its permissions, so a narrowed access does not end it.
*/
class CAccessChangePublisherComp:
			public CGqlPublisherCompBase
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CAccessChangePublisherComp);
	I_END_COMPONENT

	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsOpenFor(SubscriberKind subscriberKind, const imtgql::CGqlRequest& gqlRequest) const override;

	bool NotifyAccessChanged(const QByteArray& subscriptionId, const imtrest::IRequest& networkRequest) const;
};


} // namespace imtservergql


