#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtWidgets/QWidget>

// ACF includes
#include <iser/ISerializable.h>


namespace imtgui
{


class ILayout : public iser::ISerializable
{
public:
	typedef QList<int> IndexList;
	typedef QList<int> SizeList;

	enum LayoutType
	{
		LT_NONE = 0,
		LT_VERTICAL_SPLITTER,
		LT_HORIZONTAL_SPLITTER,
		LT_USER_LAYOUT
	};

	I_DECLARE_ENUM(LayoutType,
				LT_NONE,
				LT_VERTICAL_SPLITTER,
				LT_HORIZONTAL_SPLITTER,
				LT_USER_LAYOUT)

	/**
		Get layout's root item index.
	*/
	virtual int GetRootItemIndex() const = 0;

	/**
		Get child item indexes for given item with layout.
		\return IndexList item index list if it has layout.
	*/
	virtual IndexList GetChildIndexes(int itemIndex) const = 0;

	/**
		Get parent item index for given item.
		\return -1 for root item index, otherwise item index
	*/
	virtual int GetParentItemIndex(int itemIndex) const = 0;

	/**
		Get layout type for given item.
	*/
	virtual LayoutType GetLayoutType(int itemIndex) const = 0;

	/**
		Set specific layout for item.
		\note if already exists and itemCount is qual to old item count in layout, then just replace/update layout type.
		\return true if layout has been created.
	*/
	virtual bool SetItemLayout(int itemIndex, LayoutType layoutType, int itemCount) = 0;

	/**
		Get layout sizes for specific item.
	*/
	virtual SizeList GetLayoutSizes(int itemIndex) = 0;

	/**
		Set layout sizes for specific item.
	*/
	virtual bool SetLayoutSizes(int itemIndex, SizeList& sizeList) = 0;
};


} // namespace imtgui
