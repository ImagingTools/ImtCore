#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtlic/CFeatureContainer.h>
#include <imtlic/ILicenseInfo.h>


namespace imtauth
{


/**
	Interface for providing permissions.
	\ingroup Permission
*/
class IPermissionsProvider: virtual public istd::IChangeable
{
public:
	/**
		Get list of all permissions.
	*/
	virtual const imtlic::ILicenseInfo::FeatureInfos GetPermissionInfos(const QByteArray& productId) const = 0;
};


} // namespace imtauth


