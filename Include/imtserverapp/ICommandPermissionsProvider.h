// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/IChangeable.h>


namespace imtserverapp
{

/**
	Common interface for command permissions.
*/
class ICommandPermissionsProvider: virtual public istd::IChangeable
{
public:
	enum LogicalCheckMode
	{
		LCM_OR,
		LCM_AND
	};

	virtual LogicalCheckMode GetCommandPermissionCheckMode() const = 0;
	virtual QByteArrayList GetCommandIds() const = 0;
	virtual QByteArrayList GetCommandPermissions(const QByteArray& commandId) const = 0;
};


} // namespace imtserverapp


