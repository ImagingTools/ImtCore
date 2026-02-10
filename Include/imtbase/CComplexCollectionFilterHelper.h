// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>


namespace imtbase
{


class CComplexCollectionFilterHelper
{
public:
	static IComplexCollectionFilter::SortingOrder GetSortingOrder(const IComplexCollectionFilter& filter, const QByteArrayList& fieldIds);
	static void SetSortingOrder(IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, IComplexCollectionFilter::SortingOrder order);

	static QSet<QByteArray> GetSortingFieldIds(const IComplexCollectionFilter& filter);
	static QSet<QByteArray> GetFilteringFieldIds(const IComplexCollectionFilter::FilterExpression& filter);

	static QString GetTextFilter(const imtbase::IComplexCollectionFilter::FilterExpression& filter);
	static void SetTextFilter(imtbase::IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, const QString& text);

	static void FillTextFilter(imtbase::IComplexCollectionFilter::FilterExpression& groupFilter, const QByteArrayList& fieldIds, const QString& text);
};


}// namespace imtbase


