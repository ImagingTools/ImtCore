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
	virtual imtauth::IUserGroupInfoSharedPtr GetUserGroup(const QByteArray& groupId) const = 0;
};


} // namespace imtauth


