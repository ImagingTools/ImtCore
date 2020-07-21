#pragma once


// Qt includes
#include <QtGui/QIcon>

// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>
#include <imtloggui/CIconBasedEventItem.h>


namespace imtgui
{


class CApplicationChangedEventItem: public imtloggui::CIconBasedEventItem
{
public:
	typedef CIconBasedEventItem BaseClass;

	CApplicationChangedEventItem(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent = nullptr);

	void SetFont(const QFont& font);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QFont m_font;
};


} // namespace imtgui


