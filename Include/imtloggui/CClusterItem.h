#pragma once


// ImtCore includes
#include <imtloggui/IClusterItem.h>
#include <imtloggui/CItemBase.h>


namespace imtloggui
{


class CClusterItem:
			public CItemBase,
			virtual public IClusterItem
{
public:
	typedef CItemBase BaseClass;

	CClusterItem();

	// reimplemented (imtloggui::IEventItemCluster)
	virtual void SetParams(const QSize& size) override;
	virtual void Attach(IEventItem* itemPtr) override;
	virtual void DetachAll() override;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
	virtual QVariant GetMetaInfo(int metaInfoType) const override;
	virtual bool SetMetaInfo(int metaInfoType, const QVariant& metaInfo) override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;
	virtual QString GetMetaInfoDescription(int metaInfoType) const override;
	virtual bool IsMetaInfoWritable(int metaInfoType) const override;

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QRect m_rect;

	quint32 m_counters[5];	
};


} // namespace imtloggui


