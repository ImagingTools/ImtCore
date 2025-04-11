#pragma once


// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>


namespace imtdb
{


class CComplexCollectionFilterConverter
{
public:
	static QString CreateSqlSortQuery(const imtbase::IComplexCollectionFilter& filter);
	static QString CreateSqlFilterQuery(const imtbase::IComplexCollectionFilter& filter, bool castTypes = true);

private:
	static QString ProcessColumn(const imtbase::IComplexCollectionFilter::FieldFilter& filter, bool castTypes = true);
	static QString ProcessGroup(const imtbase::IComplexCollectionFilter::GroupFilter& filter, bool castTypes = true);
};


}// namespace imtdb


