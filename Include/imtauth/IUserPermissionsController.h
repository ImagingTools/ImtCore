#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


class IUserPermissionsController: virtual public istd::IChangeable
{
public:
	virtual QByteArrayList GetPermissions(const QByteArray& userId) const = 0;
	virtual void SetPermissions(const QByteArray& userId, const QByteArrayList& permissions) = 0;
};


} // namespace imtauth


