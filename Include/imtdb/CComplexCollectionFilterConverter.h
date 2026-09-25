// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

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

	/**
		Renders a field filter itself instead of the converter.
		Returns true if the field was handled; an empty result drops the field from the query.
	*/
	typedef std::function<bool(const imtbase::IComplexCollectionFilter::FieldFilter& filter, QString& sql)> FieldRenderer;

	static QString CreateSqlSortQuery(const imtbase::IComplexCollectionFilter& filter);
	static QString CreateSqlFilterQuery(const imtbase::IComplexCollectionFilter& filter, SqlContext sqlContext = SC_GENERAL);
	static QString CreateSqlFilterQuery(const imtbase::IComplexCollectionFilter& filter, SqlContext sqlContext, const FieldRenderer& fieldRenderer);

private:
	static QString ProcessColumn(const imtbase::IComplexCollectionFilter::FieldFilter& filter, SqlContext sqlContext = SC_GENERAL);
	static QString ProcessGroup(
				const imtbase::IComplexCollectionFilter::FilterExpression& filter,
				SqlContext sqlContext = SC_GENERAL,
				const FieldRenderer* fieldRendererPtr = nullptr);
	static QString ToSqlArray(const QVariantList& values);
};


}// namespace imtdb


