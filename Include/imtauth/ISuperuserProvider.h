// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


class ISuperuserProvider: virtual public istd::IChangeable
{
public:
	enum ExistsStatus
	{
		ES_EXISTS,
		ES_NOT_EXISTS,
		ES_UNKNOWN
	};

	/**
		Get information for exists superuser.
	*/
	virtual ExistsStatus SuperuserExists(QString& errorMessage) const = 0;

	/**
		Get information for superuser ID.
	*/
	virtual QByteArray GetSuperuserId() const = 0;
};


} // namespace imtauth


