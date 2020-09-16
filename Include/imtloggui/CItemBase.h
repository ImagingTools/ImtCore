#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// ImtCore includes
#include <imtloggui/IItemBase.h>


namespace imtloggui
{


class CItemBase:
			public QGraphicsItem,
			virtual public IItemBase
{
public:
	typedef QGraphicsItem BaseClass;

	void SetParams(QGraphicsItem *parent = nullptr);

	// reimplemented (imtloggui::IItemBase)
	virtual bool CollidesWithItem(const IItemBase* otherPtr, double scale) const override;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual bool CItemBase::SetMetaInfo(int metaInfoType, const QVariant& metaInfo) override;
	virtual QString GetMetaInfoDescription(int metaInfoType) const override;
	virtual bool IsMetaInfoWritable(int metaInfoType) const override;
};


} // namespace imtloggui


