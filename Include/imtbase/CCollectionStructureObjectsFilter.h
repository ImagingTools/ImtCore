#pragma once


// ImtCore includes
#include <iser/ISerializable.h>


namespace imtbase
{


class CCollectionStructureObjectsFilter : virtual public iser::ISerializable
{
public:
	enum SortingAttribute
	{
		/*
			Sorting by node ID
		*/
		SM_ID = 1,
	};

	enum SortingDirection
	{
		SD_ASCENDING = 1,
		SD_DESCENDING
	};

	CCollectionStructureObjectsFilter(
				const QByteArray& nodeId,
				SortingAttribute sortingAttribute = SM_ID,
				SortingDirection sortingDirection = SD_ASCENDING);

	QByteArray GetNodeId();
	SortingAttribute GetSortingAttribute();
	SortingDirection GetSortingDirection();

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	QByteArray m_nodeId;
	SortingAttribute m_sortingAttribute;
	SortingDirection m_sortingDirection;
};


} // namespace imtbase


