#pragma once


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/IChangeable.h>


namespace imtgql
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


} // namespace imtgql


