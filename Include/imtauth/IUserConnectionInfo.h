// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/IObject.h>


namespace imtauth
{


class IUserConnectionInfo: virtual public iser::IObject
{
public:
	/**
		Get ID of the user.
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set ID of the user.
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
		Get last connection time of the user.
	*/
	virtual QDateTime GetLastConnection() const = 0;

	/**
		Set last connection time of the user.
	*/
	virtual void SetLastConnection(const QDateTime& lastConnection) = 0;
};


} // namespace imtauth


