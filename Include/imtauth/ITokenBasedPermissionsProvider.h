#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	Interface for getting the list of permission-IDs based on an access token.
*/
class ITokenBasedPermissionsProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get the permissions for a given access token.
	*/
	virtual const QByteArrayList GetPermissions(const QByteArray& token) const = 0;
};


} // namespace imtauth


