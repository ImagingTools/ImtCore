// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	static QString ProcessGroup(const imtbase::IComplexCollectionFilter::FilterExpression& filter, SqlContext sqlContext = SC_GENERAL);
	static QString ToSqlArray(const QVariantList& values);
};


}// namespace imtdb


