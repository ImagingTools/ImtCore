#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


class IDependentMetaInfoController: virtual public istd::IPolymorphic
{
public:
	struct DependentMetaInfo
	{
		QString objectId;
		QString dependentKey;
		QStringList metaInfoIds;
		QStringList metaInfoValues;
	};
	
	virtual bool UpdateDependentMetaInfo(const DependentMetaInfo& metaInfo) const = 0;
	virtual bool ClearDependentMetaInfo(const DependentMetaInfo& metaInfo) const = 0;
};


} // namespace imtdb


