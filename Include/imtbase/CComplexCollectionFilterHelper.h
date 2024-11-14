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
	static QSet<QByteArray> GetFilteringFieldIds(const IComplexCollectionFilter::GroupFilter& filter);

	static QString GetTextFilter(const imtbase::IComplexCollectionFilter::GroupFilter& filter);
	static void SetTextFilter(imtbase::IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, const QString& text);
};


}// namespace imtbase


