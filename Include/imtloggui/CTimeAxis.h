#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


class CTimeAxis: public QGraphicsObject
{
public:
	enum MsecConsts
	{
		MS_SECOND = 1000,
		MS_MINUTE = 60000,
		MS_HOUR = 3600000,
		MS_DAY = 86400000
	};

	typedef QGraphicsItem BaseClass;

	CTimeAxis(QGraphicsItem *parent = nullptr);

	void setTimeSpan(const QDateTime& startDateTime, const QDateTime& endDateTime);
	bool setMinorTickCount(int count);
	void setColor(const QColor& color);
	void setGeometry(const QRectF &geometry);
	void update();

	// reimplemented (QGraphicsItem)
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

signals:
	void GeometryChanged(const QRectF& geometry);
	void TimeSpanChanged(const QDateTime& startDateTime, const QDateTime& endDateTime);

private:
	double convertDateTimeToPosX(const QDateTime& dateTime);

private:
	QRectF m_geometryRect;
	QRectF m_boundingRect;

	QDateTime m_startDateTime;
	QDateTime m_endDateTime;
	int m_minorTickCount;
	QColor m_color;
	QList<QGraphicsItem*> m_sceneItems;
};


} // namespace imtloggui


