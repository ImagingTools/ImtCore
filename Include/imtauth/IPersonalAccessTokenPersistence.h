// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	Atomic updates of token fields. Implementations must not write fields unrelated to the operation.
*/
class IPersonalAccessTokenPersistence: virtual public istd::IPolymorphic
{
public:
	virtual bool RevokeToken(const QByteArray& tokenId) = 0;
	virtual bool SetTokenScopes(const QByteArray& tokenId, const QByteArrayList& scopes) = 0;
	virtual bool UpdateLastUsedAt(const QByteArray& tokenId, const QDateTime& usedAt) = 0;
};


} // namespace imtauth
