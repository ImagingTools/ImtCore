// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgql/CGqlEnum.h>


namespace imtgql
{


// public methods

CGqlEnum::CGqlEnum(const QByteArray& value)
	:m_value(value)
{
}


QByteArray CGqlEnum::GetValue()
{
	return m_value;
}


CGqlEnum::operator QVariant() const
{
	return QVariant::fromValue(*this);
}


} // namespace imtgql


