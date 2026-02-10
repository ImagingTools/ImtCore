// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlog
{


class IGroupMessageIdsProvider: virtual public istd::IChangeable
{
public:
	virtual bool GetGroupMessageIds(const QByteArray& groupId, QSet<int>& messageIds) const = 0;
};


} // namespace imtlog


