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

	I_DECLARE_ENUM(SortingOrder, SO_NO_ORDER, SO_DESC, SO_ASC);

	virtual QByteArray GetObjectTypeId() const = 0;
	virtual void SetObjectTypeId(const QByteArray& typeId) = 0;

	virtual QString GetTextFilter() const = 0;
	virtual void SetTextFilter(const QString& filter) = 0;

	virtual SortingOrder GetSortingOrder() const = 0;
	virtual void SetSortingOrder(SortingOrder sortingOrder) = 0;

	/**
		Get ID of the meta-information used for sorting the elements in the collection.
	*/
	virtual QByteArrayList GetSortingInfoIds() const = 0;

	/**
		Set ID of the meta-information used for sorting the elements in the collection.
	*/
	virtual void SetSortingInfoIds(const QByteArrayList& sortingInfoIds) = 0;

	/**
		Get IDs of the meta-information used for filtering the elements in the collection.
	*/
	virtual QByteArrayList GetFilteringInfoIds() const = 0;

	/**
		Set IDs of the meta-information used for filtering the elements in the collection.
	*/
	virtual void SetFilteringInfoIds(const QByteArrayList& filteringInfoIds) = 0;
};


} // namespace imtbase


