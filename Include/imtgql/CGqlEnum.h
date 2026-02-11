// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QVariant>



namespace imtgql
{


class CGqlEnum
{
public:
	CGqlEnum(const QByteArray& value = QByteArray());

	QByteArray GetValue();
	operator QVariant() const;

protected:
	QByteArray m_value;
};


} // namespace imtgql


Q_DECLARE_METATYPE(imtgql::CGqlEnum)

