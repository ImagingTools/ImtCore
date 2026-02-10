// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


/**
	Interface describing manage of server.
*/
class IServer: virtual public istd::IPolymorphic
{
public:
	enum ServerStatus
	{
		SS_NOT_STARTED,
		SS_LISTENING
	};

	/**
		Start server
	*/
	virtual bool StartServer() = 0;

	/**
		Stop server
	*/
	virtual bool StopServer() = 0;

	/**
		Get current server status
	*/
	virtual ServerStatus GetServerStatus() const = 0;
};


} // namespace imtrest


