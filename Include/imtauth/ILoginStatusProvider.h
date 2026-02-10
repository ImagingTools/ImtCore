// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


class ILoginStatusProvider: virtual public istd::IChangeable
{
public:
	enum LoginStatusFlags
	{
		/**
			There are cached credentials of the last user
		*/
		LSF_CACHED = 1,

		/**
			User is logged in
		*/
		LSF_LOGGED_IN = 2,

		/**
			Try to login in progress
		*/
		LSF_TRY_TO_LOGIN = 4,

		/**
			Try to refresh credentials in progress
		*/
		LSF_TRY_TO_REFRESH = 8
	};

	virtual int GetLoginStatus(const QByteArray& clientId = QByteArray()) const = 0;
};


} // namespace imtauth


