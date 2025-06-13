#pragma once


// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>


namespace imtdb
{


class CComplexCollectionFilterConverter
{
public:
	enum SqlContext
	{
		SC_GENERAL,
		SC_POSTGRES
	};

	static QString CreateSqlSortQuery(const imtbase::IComplexCollectionFilter& filter);
	static QString CreateSqlFilterQuery(const imtbase::IComplexCollectionFilter& filter, SqlContext sqlContext = SC_GENERAL);

private:
	static QString ProcessColumn(const imtbase::IComplexCollectionFilter::FieldFilter& filter, SqlContext sqlContext = SC_GENERAL);
	static QString ProcessGroup(const imtbase::IComplexCollectionFilter::GroupFilter& filter, SqlContext sqlContext = SC_GENERAL);
};


}// namespace imtdb


