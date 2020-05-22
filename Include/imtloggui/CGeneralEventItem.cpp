#include <imtloggui/CGeneralEventItem.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


// public methods

CGeneralEventItem::CGeneralEventItem(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent)
	:BaseClass(message, parent)
{
}


// reimplemented (QGraphicsItem)

QRectF CGeneralEventItem::boundingRect() const
{
	return QRectF(-20, -20, 40, 40);
}


void CGeneralEventItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	QBrush brush;
	istd::IInformationProvider::InformationCategory category = m_messagePtr->GetInformationCategory();

	switch (category){
	case istd::IInformationProvider::IC_NONE:
		brush = QBrush(Qt::darkGray);
		break;
	case istd::IInformationProvider::IC_INFO:
		brush = QBrush(Qt::green);
		break;
	case istd::IInformationProvider::IC_WARNING:
		brush = QBrush(Qt::yellow);
		break;
	case istd::IInformationProvider::IC_ERROR:
		brush = QBrush(Qt::red);
		break;
	case istd::IInformationProvider::IC_CRITICAL:
		brush = QBrush(Qt::darkRed);
		break;
	}

	painter->setBrush(brush);
	painter->drawEllipse(-20, -20, 40, 40);
}


} // namespace imtloggui


