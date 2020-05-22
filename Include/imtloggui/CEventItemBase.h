#pragma once


// Qt includes
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QStyleOptionGraphicsItem>

// ImtCore includes
#include <ilog/IMessageConsumer.h>


namespace imtloggui
{


class CEventItemBase: public QGraphicsObject
{
public:
	typedef QGraphicsObject BaseClass;

	CEventItemBase(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent = nullptr);

	QDateTime GetEventTimeStamp() const;

protected:
	ilog::IMessageConsumer::MessagePtr m_messagePtr;
};


} // namespace imtloggui


