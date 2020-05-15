#pragma once


// Qt includes
#include <QtGui/QIcon>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class ÑLoginEventItem: public CEventItemBase
{
public:
	typedef CEventItemBase BaseClass;

	ÑLoginEventItem(ilog::IMessageConsumer::MessagePtr message, QGraphicsItem* parent = nullptr);

	void SetFont(const QFont& font);
	void SetIcons(const QIcon& iconLogin, const QIcon& iconLogout);
	void SetIconSize(const QSize& size);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QFont m_font;
	QIcon m_iconLogin;
	QIcon m_iconLogout;
	QSize m_iconSize;
};


} // namespace imtloggui


