// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class IOperationDescription: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetOperationTypeId() const = 0;
	virtual QByteArray GetKey() const = 0;
	virtual QString GetKeyName() const = 0;
	virtual QByteArray GetOldValue() const = 0;
	virtual QByteArray GetNewValue() const = 0;
};


} // namespace imtbase


