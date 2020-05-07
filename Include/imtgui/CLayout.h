#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QDataStream>
#include <QtGui/QIcon>

// ACF includes
#include <istd/TSmartPtr.h>
#include <istd/TChangeDelegator.h>

// ImtCore includes
#include <imtgui/ILayout.h>


namespace imtgui
{


/*
	Basic implementation for gui layout.
*/
class CLayout: public istd::TChangeDelegator<imtgui::ILayout>
{
public:
	CLayout(CLayout* parentLayoutPtr = nullptr);
	virtual ~CLayout();

	// reimplemented (imtgui::ILayout)
	virtual QByteArray GetLayoutId() const Q_DECL_OVERRIDE;
	virtual void SetType(const LayoutType& type) Q_DECL_OVERRIDE;
	virtual LayoutType GetType() const Q_DECL_OVERRIDE;
	virtual void SetTitle(const QString& title) Q_DECL_OVERRIDE;
	virtual QString GetTitle() const Q_DECL_OVERRIDE;
	virtual void SetTitleAlign(const AlignType& align) Q_DECL_OVERRIDE;
	virtual AlignType GetTitleAlign() const Q_DECL_OVERRIDE;
	virtual void SetIcon(const QPixmap& icon) Q_DECL_OVERRIDE;
	virtual QPixmap GetIcon() const Q_DECL_OVERRIDE;
	virtual void SetViewId(const QByteArray& viewId) Q_DECL_OVERRIDE;
	virtual QByteArray GetViewId() const Q_DECL_OVERRIDE;
	virtual void SetSizes(const SizeList& sizes) Q_DECL_OVERRIDE;
	virtual SizeList GetSizes() const Q_DECL_OVERRIDE;
	virtual void SetLayoutProperties(const LayoutProperties &properties) Q_DECL_OVERRIDE;
	virtual LayoutProperties GetLayoutProperties() const Q_DECL_OVERRIDE;
	virtual ILayout* GetParent() const Q_DECL_OVERRIDE;
	virtual int GetChildsCount() const Q_DECL_OVERRIDE;
	virtual ILayout* GetChild(int index) const Q_DECL_OVERRIDE;
	virtual void InsertChild(int index, ILayout* layout) Q_DECL_OVERRIDE;
	virtual void AppendChild(ILayout* layout) Q_DECL_OVERRIDE;
	virtual ILayout* TakeChild(int index) Q_DECL_OVERRIDE;
	virtual ILayout* TakeFirst() Q_DECL_OVERRIDE;
	virtual ILayout* TakeLast() Q_DECL_OVERRIDE;
	virtual void Clear() Q_DECL_OVERRIDE;
	virtual ILayout* FindChild(const QByteArray& id) const Q_DECL_OVERRIDE;
	virtual ILayout* RemoveChild(const QByteArray& id) Q_DECL_OVERRIDE;
	virtual void SplitLayout(ILayout::LayoutType type) Q_DECL_OVERRIDE;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) Q_DECL_OVERRIDE;

private:
	void CopyData(ILayout* source);
	bool InternalSerializeItemRecursive(iser::IArchive& archive);

private:
	SizeList m_sizes;
	LayoutType m_layoutType;
	AlignType m_alignType;
	QString m_title;
	QPixmap m_icon;
	QByteArray m_id;
	QByteArray m_viewId;
	CLayout* m_parent;
	QList<ILayout*> m_childs;
	LayoutProperties m_properties;

//	QDataStream &operator<<(QDataStream &dataStream, LayoutProperties* src);
};


QDataStream &operator<<(QDataStream &dataStream, const ILayout::LayoutProperties &src);
QDataStream &operator>>(QDataStream &dataStream, ILayout::LayoutProperties &src);
bool operator ==(const ILayout::LayoutProperties &left, const ILayout::LayoutProperties &right);
bool operator !=(const ILayout::LayoutProperties &left, const ILayout::LayoutProperties &right);


} // namespace imtgui


