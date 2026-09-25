// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>


namespace imtdb
{


/**
	Builds SQL for the reserved 'Tags' field of a complex collection filter.

	The field is matched against the TagAssignments table instead of a column:
	ArrayHasAny (label:a,b), ArrayHasAll (label:a label:b), their negations (-label:a)
	and ArrayIsEmpty (no:label). Only EXISTS/COUNT subqueries are produced, so the same
	SQL runs on PostgreSQL and SQLite.
*/
class CTagFilterSqlBuilder
{
public:
	struct Config
	{
		/**
			Quoted (and schema-qualified if needed) name of the tag assignments table.
		*/
		QString assignmentsTable;

		/**
			Quoted (and schema-qualified if needed) name of the tag catalog table.
		*/
		QString tagsTable;

		/**
			Entity type the assignments are stored under.
		*/
		QString entityType;

		/**
			SQL expression of the object ID in the outer query, evaluating to text.
		*/
		QString entityIdExpression;

		bool isSqlite = false;
	};

	static const QByteArray s_tagsFieldId;

	/**
		Create the condition of one 'Tags' field filter, or an empty string if the filter selects nothing.
	*/
	static QString CreateCondition(const imtbase::IComplexCollectionFilter::FieldFilter& filter, const Config& config);

	/**
		Convert a complex filter to SQL, rendering the 'Tags' field filters with CreateCondition().
		\param postProcessor	Applied to the converted query before the tag conditions are inserted.
	*/
	static QString CreateFilterQuery(
				const imtbase::IComplexCollectionFilter& collectionFilter,
				const Config& config,
				const std::function<void(QString&)>& postProcessor = nullptr);
};


} // namespace imtdb


