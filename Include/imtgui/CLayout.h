#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtGui/QIcon>

// Acf includes
#include <istd/TSmartPtr.h>

// icarmagui includes
#include <imtgui/ILayout.h>


namespace imtgui
{


/*
	Basic implementation for gui layout.
*/
class CLayout: public imtgui::ILayout
{
public:
	CLayout(CLayout *root = NULL);
	virtual ~CLayout();

	//typedef istd::TSmartPtr<iser::ISerializable> ObjectInstancePtr;
	//virtual ObjectInstancePtr CreateObjectInstance(const QByteArray& objectType) = 0;

	void SetSplitterLayout(ILayout::LayoutType type);


	// reimplemented (icarmagui::ILayout)

	virtual void SetLayoutId(const QByteArray& id) Q_DECL_OVERRIDE;
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

	virtual ILayout* GetParent() Q_DECL_OVERRIDE;
	virtual ILayout* GetRoot() Q_DECL_OVERRIDE;
	virtual int GetChildsCount() Q_DECL_OVERRIDE;
	virtual ILayout* GetChild(int index) Q_DECL_OVERRIDE;
	virtual void InsertChild(int index, ILayout* layout) Q_DECL_OVERRIDE;
	virtual void AppendChild(ILayout* layout) Q_DECL_OVERRIDE;
	virtual ILayout* TakeChild(int index) Q_DECL_OVERRIDE;
	virtual ILayout* TakeFirst() Q_DECL_OVERRIDE;
	virtual ILayout* TakeLast() Q_DECL_OVERRIDE;
	virtual void Clear() Q_DECL_OVERRIDE;
	virtual ILayout* FindChild(const QByteArray& id) Q_DECL_OVERRIDE;
	virtual ILayout* RemoveChild(const QByteArray& id) Q_DECL_OVERRIDE;

	// reimplemented (iser::ISerializable)

	virtual bool Serialize(iser::IArchive& archive) Q_DECL_OVERRIDE;

private:
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
	CLayout* m_root;
	QList<ILayout*> m_childs;
};


} // namespace imtgui
