#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsLayoutItem>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


class CTimeAxis: public QGraphicsLayoutItem, public QGraphicsItem
{
public:
	CTimeAxis(QGraphicsItem *parent = nullptr);

	void setTimeInterval(const QDateTime & startDateTime, const QDateTime & endDateTime);
	bool setMinorTickCount(int count);

	// reimplemented (QGraphicsLayoutItem)
	void setGeometry(const QRectF &geometry) override;

	// reimplemented (QGraphicsItem)
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

protected:
	// reimplemented (QGraphicsLayoutItem)
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;

private:
	int m_minorTickCount;
	QDateTime m_startDateTime;
	QDateTime m_endDateTime;
};


} // namespace imtloggui


