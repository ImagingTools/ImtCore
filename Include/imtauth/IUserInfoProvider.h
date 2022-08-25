#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IUserInfo;


/**
	Interface for a collection of users.
	\ingroup User
*/
class IUserInfoProvider: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		/**
			List of user infos given as QStringList.
		*/
		MIT_USER_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1
	};

	/**
		Get list of all available users.
	*/
	virtual const imtbase::ICollectionInfo& GetUserList() const = 0;

	/**
		Get information for user.
	*/
	virtual const imtauth::IUserInfo* GetUser(const QByteArray& userId) const = 0;
};


} // namespace imttest


