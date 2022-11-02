#pragma once


// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


/**
	Interface for check permissions.
	\ingroup Permission
*/
class ICheckPermission: virtual public istd::IPolymorphic
{
public:
	/**
		Check user permissions.
	*/
	virtual bool CheckPermission(const imtauth::IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissions) = 0;
};


} // namespace imtauth


