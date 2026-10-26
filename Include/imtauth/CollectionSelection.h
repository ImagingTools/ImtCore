// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>

// ACF includes
#include <iprm/CParamsSet.h>
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/imtbase.h>


namespace imtauth
{


/**
	Selections of the organization collections by their columns, so a lookup reads its candidates instead of every element.
	A collection that does not know the filter returns every id, so the caller still verifies the loaded objects.
*/
namespace CollectionSelection
{


typedef imtbase::IComplexCollectionFilter::FilterExpression FilterExpression;
typedef imtbase::IComplexCollectionFilter::FieldFilter FieldFilter;


inline FilterExpression ColumnEquals(const QByteArray& columnId, const QByteArray& value)
{
	FilterExpression retVal;
	retVal.fieldFilters << FieldFilter(columnId, QString::fromUtf8(value));

	return retVal;
}


inline FilterExpression AnyColumnEquals(const QByteArray& firstColumnId, const QByteArray& secondColumnId, const QByteArray& value)
{
	FilterExpression retVal = ColumnEquals(firstColumnId, value);
	retVal.fieldFilters << FieldFilter(secondColumnId, QString::fromUtf8(value));
	retVal.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;

	return retVal;
}


inline FilterExpression PairInEitherOrder(const QByteArray& firstColumnId, const QByteArray& secondColumnId, const QByteArray& firstValue, const QByteArray& secondValue)
{
	FilterExpression direct = ColumnEquals(firstColumnId, firstValue);
	direct.fieldFilters << FieldFilter(secondColumnId, QString::fromUtf8(secondValue));

	FilterExpression reversed = ColumnEquals(firstColumnId, secondValue);
	reversed.fieldFilters << FieldFilter(secondColumnId, QString::fromUtf8(firstValue));

	FilterExpression retVal;
	retVal.filterExpressions << direct << reversed;
	retVal.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;

	return retVal;
}


/**
	Ids of the elements whose columns match the expression.
	\param textColumnIds	The columns of the expression that hold text; all others hold UUIDs, and another value there matches no row
							(in a filter it would be an SQL error).
	\param isReadPtr		[optional] False when the collection reported an error, so an empty result is not a selection of nothing.
*/
inline QByteArrayList SelectIds(
			const imtbase::IObjectCollection& collection,
			const FilterExpression& filterExpression,
			const QByteArrayList& textColumnIds = QByteArrayList(),
			bool* isReadPtr = nullptr)
{
	if (isReadPtr != nullptr){
		*isReadPtr = true;
	}

	QList<FilterExpression> expressions{filterExpression};
	while (!expressions.isEmpty()){
		const FilterExpression expression = expressions.takeLast();
		for (const FieldFilter& fieldFilter : expression.fieldFilters){
			if (!textColumnIds.contains(fieldFilter.fieldId) && !imtbase::isValidUuid(fieldFilter.filterValue.toString().toUtf8())){
				return QByteArrayList();
			}
		}
		for (const FilterExpression& nested : expression.filterExpressions){
			expressions << nested;
		}
	}

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpression);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	ilog::CMessageContainer readErrors;
	const QByteArrayList retVal = collection.GetElementIds(0, -1, &filterParams, &readErrors);
	if (isReadPtr != nullptr && readErrors.GetWorstCategory() >= istd::IInformationProvider::IC_ERROR){
		*isReadPtr = false;
	}

	return retVal;
}


} // namespace CollectionSelection


} // namespace imtauth


