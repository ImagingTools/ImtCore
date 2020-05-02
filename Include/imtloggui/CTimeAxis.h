#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


class CTimeAxis: public QGraphicsRectItem
{
public:
	enum MsecConsts
	{
		MS_SECOND = 1000,
		MS_MINUTE = 60000,
		MS_HOUR = 3600000,
		MS_DAY = 86400000
	};

	typedef QGraphicsRectItem BaseClass;

	CTimeAxis(QGraphicsItem *parent = nullptr);

	void setTimeSpan(const QDateTime& startDateTime, const QDateTime& endDateTime);
	bool setMinorTickCount(int count);
	void setColor(const QColor& color);

	// reimplemented (QGraphicsRectItem)
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

signals:
	void GeometryChanged(const QRectF& geometry);
	void TimeSpanChanged(const QDateTime& startDateTime, const QDateTime& endDateTime);

private:
	QRectF SceneVisibleRect() const;
	double convertDateTimeToPosX(const QDateTime& dateTime);

private:
	QDateTime m_startDateTime;
	QDateTime m_endDateTime;
	int m_minorTickCount;
	QColor m_color;
	QList<QGraphicsItem*> m_sceneItems;
};


} // namespace imtloggui


