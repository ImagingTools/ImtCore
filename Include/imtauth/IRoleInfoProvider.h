// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IParamsSet.h>
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtbase
{
class ICollectionInfo;
}


namespace imtauth
{


class IRole;
typedef istd::TUniqueInterfacePtr<IRole> IRoleUniquePtr;


/**
	Interface for a collection of roles.
	\ingroup Role
*/
class IRoleInfoProvider: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		/**
			List of role infos given as QStringList.
		*/
		MIT_ROLE_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1
	};

	/**
		Get list of all available roles.
	*/
	virtual const imtbase::ICollectionInfo& GetRoleList() const = 0;

	/**
		Get information for role by Object-ID.
	*/
	virtual imtauth::IRoleUniquePtr GetRole(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtauth


