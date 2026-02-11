// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imtrest/ISender.h>


namespace imtrest
{


/**
	Common interface for a request manager object.
*/
class IRequestManager: virtual public istd::IPolymorphic
{
public:
	/**
		Get sender instance using for sending responses to the clients.
	*/
	virtual const ISender* GetSender(const QByteArray& requestId) const = 0;
};


} // namespace imtrest


