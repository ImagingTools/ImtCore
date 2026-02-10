// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <iprm/IParamsSet.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


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
	virtual imtauth::IUserInfoUniquePtr GetUser(const QByteArray& userId, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtauth


