#pragma once


// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>


namespace imtbase
{


IComplexCollectionFilter::SortingOrder GetSortingOrder(const IComplexCollectionFilter& filter, const QByteArrayList& fieldIds);
void SetSortingOrder(IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, IComplexCollectionFilter::SortingOrder order);

QSet<QByteArray> GetSortingFieldIds(const IComplexCollectionFilter& filter);
QSet<QByteArray> GetFilteringFieldIds(const IComplexCollectionFilter::GroupFilter& filter);

QString GetTextFilter(const imtbase::IComplexCollectionFilter::GroupFilter& filter);
void SetTextFilter(imtbase::IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, const QString& text);

QString CreateDefaultSqlSortingQuery(const imtbase::IComplexCollectionFilter& filter);
QString CreateDefaultSqlFilterQuery(const imtbase::IComplexCollectionFilter& filter);


}// namespace imtbase


