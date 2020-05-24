#include <imtloggui/CEventItemBase.h>


// Qt includes
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CEventItemBase::CEventItemBase(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent)
	:BaseClass(parent)
{
	m_messagePtr = message;
}


QDateTime CEventItemBase::GetEventTimeStamp() const
{
	if (m_messagePtr.IsValid()){
		return m_messagePtr->GetInformationTimeStamp();
	}

	return QDateTime();
}


} // namespace imtloggui


