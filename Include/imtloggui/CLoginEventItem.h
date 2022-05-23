#pragma once


// Qt includes
#include <QtCore/QRect>
#include <QtGui/QFont>
#include <QtGui/QPixmap>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class CLoginEventItem: public CEventItemBase
{
public:
	typedef CEventItemBase BaseClass;

	enum MetaInfoType
	{
		MIT_ACTION = idoc::IDocumentMetaInfo::MIT_USER + 200,
		MIT_USER_NAME
	};

	void SetParams(
				const QPixmap& iconLogin,
				const QPixmap& iconLogout,
				const ilog::IMessageConsumer::MessagePtr& messagePtr,
				QGraphicsItem* parentPtr = nullptr);

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
	virtual QVariant GetMetaInfo(int metaInfoType) const override;
	virtual QByteArray GetMetaInfoId(int metaInfoType) const override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QFont m_font;
	QPixmap m_iconLogin;
	QPixmap m_iconLogout;
	mutable QRectF m_boundingRect;
	mutable QRectF m_labelRect;
};


} // namespace imtloggui


