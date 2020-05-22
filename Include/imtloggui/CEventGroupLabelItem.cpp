#include <imtloggui/CEventGroupLabelItem.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>


namespace imtloggui
{


// public methods

CEventGroupLabelItem::CEventGroupLabelItem(QGraphicsItem* parent)
	:BaseClass(parent),
	m_height(0)
{
	m_backgroundColor = QColor("#00000000");
	m_name = QObject::tr("Noname");
	m_font = QFont("Helvetica");
	m_font.setPointSize(24);
}


void CEventGroupLabelItem::SetBackgroundColor(const QColor& color)
{
	m_backgroundColor = color;
	m_backgroundColor.setAlpha(255);
}


void CEventGroupLabelItem::SetGroupName(const QString& name)
{
	m_name = name;
}

void CEventGroupLabelItem::SetHeight(double height)
{
	prepareGeometryChange();
	m_height = height;
}


// reimplemented (QGraphicsItem)

QRectF CEventGroupLabelItem::boundingRect() const
{
	QRectF bounding;
	QFontMetrics metrics(m_font);
	
	bounding.setTop(-m_height / 2);
	bounding.setBottom(m_height / 2);
	bounding.setLeft(0);
	bounding.setRight(metrics.boundingRect("Gg").height() / scene()->views().first()->viewportTransform().m11());

	return bounding; 
}


void CEventGroupLabelItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	QBrush brush(m_backgroundColor);

	painter->save();
	painter->setBrush(brush);
	painter->setPen(Qt::NoPen);
	painter->drawRect(boundingRect());
	painter->restore();

	QFontMetricsF metrics(m_font);
	painter->setFont(m_font);

	QRectF labelRect = metrics.boundingRect(m_name);
	labelRect.setWidth(labelRect.width() + 1);
	labelRect.translate(QPointF(0, 0) - labelRect.center());
	labelRect.translate(0, labelRect.height() / 2);

	painter->setRenderHint(QPainter::SmoothPixmapTransform);

	painter->rotate(-90);

	double scaleX = scene()->views().first()->viewportTransform().m11();
	double scaleY = scene()->views().first()->viewportTransform().m22();
	painter->scale(1 / scaleY, 1 / scaleX);
	painter->drawText(labelRect, Qt::AlignCenter, m_name);
}


} // namespace imtloggui


