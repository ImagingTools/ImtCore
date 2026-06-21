// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserInfo.h>


/**
	Library containing interface and implementation related to user authentification and account management.
*/
namespace imtauth
{


inline QString GetUserName(const imtbase::IObjectCollection& userCollection, const QByteArray& userId)
{
	if (userId.isEmpty()){
		return QString();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (userCollection.GetObjectData(userId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			return userInfoPtr->GetName();
		}
	}

	return QString();
}


} // namespace imtauth


