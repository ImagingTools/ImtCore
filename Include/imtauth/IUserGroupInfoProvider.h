// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


// ACF includes
#include <imtauth/IUserGroupInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IUserGroupInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetUserGroupList() const = 0;
	virtual imtauth::IUserGroupInfoSharedPtr GetUserGroup(const QByteArray& groupId, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtauth


