#pragma once


// ImtCore includes
#include <imtbase/ICollectionFilter.h>


namespace imtbase
{


/**
	Simple implementation for a collection of object references.
*/
class CCollectionFilter: virtual public ICollectionFilter
{
public:
	CCollectionFilter();

	// reimplemented (imtbase::ICollectionFilter)
	virtual QString GetTextFilter() const override;
	virtual void SetTextFilter(const QString& filter) override;
	virtual SortingOrder GetSortingOrder() const override;
	virtual void SetSortingOrder(SortingOrder sortingOrder) override;
	virtual QByteArrayList GetSortingInfoIds() const override;
	virtual void SetSortingInfoIds(const QByteArrayList& sortingInfoIds) override;
	virtual QByteArrayList GetFilteringInfoIds() const override;
	virtual void SetFilteringInfoIds(const QByteArrayList& filteringInfoIds) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode) override;
	virtual bool IsEqual(const IChangeable &object) const override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode) const override;
	virtual bool ResetData(CompatibilityMode mode) override;

private:
	QString m_filter;
	SortingOrder m_sortingOrder;
	QByteArrayList m_sortingInfoIds;
	QByteArrayList m_filteringInfoIds;
};


} // namespace imtbase



