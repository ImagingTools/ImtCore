// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservice/IServiceConnectionInfo.h>


namespace imtservice
{


/**
	Interface for describing a service connections info.
	\ingroup Service
*/
class IServiceConnectionLinkParam: virtual public IServiceConnectionInfo
{
public:
	/**
		Get dependant service connection-Id.
	*/
	virtual QByteArray GetDependantServiceConnectionId() const = 0;
};


} // namespace imtservice


