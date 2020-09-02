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


// public methods

CEventItemBase::CEventItemBase()
{
	AddMetaInfo(MIT_TIMESTAMP, QObject::tr("Timestamp"), true);
	AddMetaInfo(MIT_SOURCE, QObject::tr("Source"), true);
	AddMetaInfo(MIT_MESSAGE, QObject::tr("Message"), true);
	AddMetaInfo(MIT_STATUS, QObject::tr("Status"), true);
}

void CEventItemBase::SetParams(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parentPtr)
{
	BaseClass::SetParams(parentPtr);

	m_messagePtr = message;
}


// reimplemented (imtloggui::IEventItem)

const istd::IInformationProvider* CEventItemBase::GetInformationProvider() const
{
	return m_messagePtr.GetPtr();
}


} // namespace imtloggui


