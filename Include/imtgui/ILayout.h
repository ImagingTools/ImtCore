#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>

// ACF includes
#include <iser/ISerializable.h>


namespace imtgui
{


class ILayout : virtual public iser::ISerializable
{
public:
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
		LT_USER_LAYOUT);

	enum AlignType
	{
		AT_LEFT = 0,
		AT_RIGHT,
		AT_H_CENTER
	};

	I_DECLARE_ENUM(AlignType, AT_LEFT, AT_RIGHT, AT_H_CENTER);


	/**
		Set layout's id.
	*/
	virtual void SetLayoutId(const QByteArray& id) = 0;

	/**
		Get layout's id.
	*/
	virtual QByteArray GetLayoutId() const = 0;

	/**
		Set layout's type.
	*/
	virtual void SetType(const LayoutType& type) = 0;

	/**
		Get layout's type.
	*/
	virtual LayoutType GetType() const = 0;

	/**
		Set layout's title.
	*/
	virtual void SetTitle(const QString& title) = 0;

	/**
		Get layout's title.
	*/
	virtual QString GetTitle() const  = 0;

	/**
		Set layout's title aling.
	*/
	virtual void SetTitleAlign(const AlignType& align) = 0;

	/**
		Get layout's title align.
	*/
	virtual AlignType GetTitleAlign() const = 0;

	/**
		Set layout's icon.
	*/
	virtual void SetIcon(const QPixmap& icon) = 0;

	/**
		Get layout's icon.
	*/
	virtual QPixmap GetIcon() const = 0;

	/**
		Set layout's Widget viewId.
	*/
	virtual void SetViewId(const QByteArray& viewId) = 0;

	/**
		Get layout's Widget viewId.
	*/
	virtual QByteArray GetViewId() const  = 0;

	/**
		Set layout's child sizes.
	*/
	virtual void SetSizes(const SizeList& sizes) = 0;

	/**
		Get layout's child sizes.
	*/
	virtual SizeList GetSizes() const = 0;

	/**
		Get layout's parent.
	*/
	virtual ILayout* GetParent() = 0;

	/**
		Get root layout.
	*/
	virtual ILayout* GetRoot() = 0;

	/**
		Get layout's childs count.
	*/
	virtual int GetChildsCount() = 0;

	/**
		Get layout's child from index.
	*/
	virtual ILayout* GetChild(int index) = 0;

	/**
		Insert layout's child to index.
	*/
	virtual void InsertChild(int index, ILayout* layout) = 0;

	/**
		Append layout's child.
	*/
	virtual void AppendChild(ILayout* layout) = 0;

	/**
		Take layout's child from index.
	*/
	virtual ILayout* TakeChild(int index) = 0;

	/**
		Take first layout's child.
	*/
	virtual ILayout* TakeFirst() = 0;

	/**
		Take last layout's child.
	*/
	virtual ILayout* TakeLast() = 0;

	/**
		Clear all layout's child.
	*/
	virtual void Clear() = 0;

	/**
		Find layout's child recursive.
	*/
	virtual ILayout* FindChild(const QByteArray& id) = 0;

	/**
		Remove layout's child recursive.
	*/
	virtual ILayout* RemoveChild(const QByteArray& id) = 0;
};


//class ILayout: virtual public iser::ISerializable
//{
//public:
//	typedef QList<int> IndexList;
//	typedef QList<int> SizeList;
//
//	enum LayoutType
//	{
//		LT_NONE = 0,
//		LT_VERTICAL_SPLITTER,
//		LT_HORIZONTAL_SPLITTER,
//		LT_USER_LAYOUT
//	};
//
//	I_DECLARE_ENUM(LayoutType,
//				LT_NONE,
//				LT_VERTICAL_SPLITTER,
//				LT_HORIZONTAL_SPLITTER,
//				LT_USER_LAYOUT)
//
//	/**
//		Get layout's root item index.
//	*/
//	virtual int GetRootItemIndex() const = 0;
//
//	/**
//		Get child item indexes for given item with layout.
//		\return IndexList item index list if it has layout.
//	*/
//	virtual IndexList GetChildIndexes(int itemIndex) const = 0;
//
//	/**
//		Get parent item index for given item.
//		\return -1 for root item index, otherwise item index
//	*/
//	virtual int GetParentItemIndex(int itemIndex) const = 0;
//
//	/**
//		Get layout type for given item.
//	*/
//	virtual LayoutType GetLayoutType(int itemIndex) const = 0;
//
//	/**
//		Set specific layout for item.
//		\note if already exists and itemCount is qual to old item count in layout, then just replace/update layout type.
//		\return true if layout has been created.
//	*/
//	virtual bool SetItemLayout(int itemIndex, LayoutType layoutType, int itemCount) = 0;
//
//	/**
//		Get layout sizes for specific item.
//	*/
//	virtual SizeList GetLayoutSizes(int itemIndex) = 0;
//
//	/**
//		Set layout sizes for specific item.
//	*/
//	virtual bool SetLayoutSizes(int itemIndex, SizeList& sizeList) = 0;
//};


} // namespace imtgui


