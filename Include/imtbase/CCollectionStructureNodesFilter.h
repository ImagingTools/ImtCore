#pragma once


// ImtCore includes
#include <iser/ISerializable.h>


namespace imtbase
{


class CCollectionNodesFilter: virtual public iser::ISerializable
{
public:
	enum FilterMode
	{
		/*
			Get parent for the given node ID
		*/
		FM_PARENT,

		/*
			Get child nodes for the given node ID
		*/
		FM_CHILDS
	};

	enum SortingAttribute
	{
		/*
			Sorting by node ID
		*/
		SM_ID = 1,

		/*
			Sorting by node name
		*/
		SM_NAME
	};

	enum SortingDirection
	{
		SD_ASCENDING = 1,
		SD_DESCENDING
	};

	CCollectionNodesFilter(
				FilterMode filterMode,
				SortingAttribute sortingAttribute = SM_NAME,
				SortingDirection sortingDirection = SD_ASCENDING);

	FilterMode GetFilterMode();
	SortingAttribute GetSortingAttribute();
	SortingDirection GetSortingDirection();

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	FilterMode m_mode;
	SortingAttribute m_sortingAttribute;
	SortingDirection m_sortingDirection;
};


} // namespace imtbase


