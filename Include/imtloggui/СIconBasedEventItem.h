#pragma once


// Qt includes
#include <QtGui/QIcon>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class ÑIconBasedEventItem: public CEventItemBase
{
public:
	typedef CEventItemBase BaseClass;

	ÑIconBasedEventItem(ilog::IMessageConsumer::MessagePtr message, QGraphicsItem* parent = nullptr);

	void SetIcon(const QIcon& icon);
	void SetIconSize(const QSize& size);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

protected:
	QIcon m_icon;
	QSize m_iconSize;
};


} // namespace imtloggui


