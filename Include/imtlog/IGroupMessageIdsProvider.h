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


