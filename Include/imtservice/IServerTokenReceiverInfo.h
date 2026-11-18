// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtservice
{


/**
	The input connections of a service through which it receives the servers of other products:
	the service registers them as its clients and issues their server tokens.
	A separate interface, so that the plugins built before it keep working.
	\ingroup Service
*/
class IServerTokenReceiverInfo: virtual public istd::IPolymorphic
{
public:
	/**
		\param connectionId	ID of an input connection of the service.
	*/
	virtual bool IsServerTokenReceiver(const QByteArray& connectionId) const = 0;
};


} // namespace imtservice


