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
		Get layout's ID.
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
	virtual ILayout* GetParent()  const = 0;

	/**
		Get layout's childs count.
	*/
	virtual int GetChildsCount() const = 0;

	/**
		Get layout's child from index.
	*/
	virtual ILayout* GetChild(int index) const = 0;

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
	virtual ILayout* FindChild(const QByteArray& id) const = 0;

	/**
		Remove layout's child recursive.
	*/
	virtual ILayout* RemoveChild(const QByteArray& id) = 0;

	/**
		Split current layout and insert new sublayout.
	*/
	virtual void SplitLayout(ILayout::LayoutType type) = 0;
};


} // namespace imtgui


