// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IAccountInfo;


/**
	Provider of accounts.
	\ingroup Authentification
*/
class IAccountInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetAccountList() const = 0;
	virtual const IAccountInfo* GetAccount(const QByteArray& accountId) const = 0;
};


} // namespace imtauth


