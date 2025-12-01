#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


class ITokenBasedPermissionsProvider: virtual public istd::IPolymorphic
{
public:
	virtual const QByteArrayList GetPermissions(const QByteArray& token) const = 0;
};


} // namespace imtauth


