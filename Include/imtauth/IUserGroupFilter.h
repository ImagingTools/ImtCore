// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <iser/ISerializable.h>


namespace imtauth
{


class IUserGroupFilter: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& userId) = 0;
	virtual QByteArrayList GetGroupIds() const = 0;
	virtual void SetGroupIds(const QByteArrayList& groupIds) = 0;
};


} // namespace imtauth


