#pragma once

#include <imtbase/ICollectionInfo.h>

namespace imtdb
{

class IDependentMetaInfoNotifier : virtual public istd::IPolymorphic
{
public:
	virtual void BeginDeferredNotifications() = 0;
	virtual void NotifyMetadataChanged(const imtbase::ICollectionInfo::Ids& ids) = 0;
	virtual void CompleteDeferredNotifications(bool committed) = 0;
};

}