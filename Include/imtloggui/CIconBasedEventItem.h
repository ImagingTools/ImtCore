#pragma once


// Qt includes
#include <QtGui/QIcon>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class CIconBasedEventItem: public CEventItemBase
{
public:
	typedef CEventItemBase BaseClass;

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


