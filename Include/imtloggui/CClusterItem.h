#pragma once


// ImtCore includes
#include <imtloggui/IClusterItem.h>
#include <imtloggui/IEventItem.h>
#include <imtloggui/CItemBase.h>


namespace imtloggui
{


class CClusterItem:
			public CItemBase,
			virtual public IClusterItem
{
public:
	typedef CItemBase BaseClass;

	enum MetaInfoType
	{
		MIT_NONE_COUNTER = idoc::IDocumentMetaInfo::MIT_USER + 100,
		MIT_INFO_COUNTER,
		MIT_WARNING_COUNTER,
		MIT_ERROR_COUNTER,
		MIT_CRITICAL_COUNTER
	};

	CClusterItem();

	void SetParams(const QSize& size, QGraphicsItem *parent = nullptr);

	// reimplemented (imtloggui::IClusterItem)
	virtual void Attach(IEventItem* itemPtr) override;
	virtual void DetachAll() override;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
	virtual QVariant GetMetaInfo(int metaInfoType) const override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QRect m_rect;

	quint32 m_counters[5];	
};


} // namespace imtloggui


