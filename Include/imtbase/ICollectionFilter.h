#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


/**
	Interface for a collection filter.
	\ingroup Collection
*/
class ICollectionFilter: virtual public iser::ISerializable
{
public:
	enum SortingOrder
	{
		SO_NO_ORDER,
		SO_DESC,
		SO_ASC
	};

	virtual QString GetTextFilter() const = 0;
	virtual void SetTextFilter(const QString& filter) = 0;

	virtual SortingOrder GetSortingOrder() const = 0;
	virtual void SetSortingOrder(SortingOrder sortingOrder) = 0;

	/**
		Get ID of the meta-information used for sorting the elements in the collection.
	*/
	virtual QByteArray GetSortingInfoId() const = 0;

	/**
		Set ID of the meta-information used for sorting the elements in the collection.
	*/
	virtual void SetSortingInfoId(const QByteArray& sortingInfoId) = 0;
};


} // namespace imtbase


