#pragma once

#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IDependentMetaInfoNotifier.h>

namespace imtdb
{

class IDatabaseDependencyProcessor : virtual public IDependentMetaInfoNotifier
{
public:
	virtual bool ProcessDependencies(const IDatabaseEngine& engine,
		const imtbase::ICollectionInfo::Ids& sourceIds, bool removed) = 0;
};

}