#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtrepo/IRepositoryItemInfo.h>


namespace imtrepo
{


class IRepositoryItemInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetRepositoryItems() = 0;
	virtual const IRepositoryItemInfo* GetRepositoryItemInfo(const QByteArray& itemId) const = 0;
};


} // namespace imtrepo


