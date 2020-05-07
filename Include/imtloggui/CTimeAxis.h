#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CTimeAxis : public QGraphicsRectItem, virtual public IEventScenePositionProvider
{
public:
	typedef QGraphicsRectItem BaseClass;

	CTimeAxis(QGraphicsItem *parent = nullptr);

	const QDateTime& getStartTimeSpan();
	const QDateTime& getEndTimeSpan();
	void setTimeRange(const QDateTime& firstEventTime, const QDateTime& lastEventTime);
	bool setMinorTickCount(int count);
	void setColor(const QColor& color);

	// reimplemented (QGraphicsRectItem)
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

	// reimplemented (IEventScenePositionProvider)
	virtual double GetScenePosition(const QDateTime& time) const override;

signals:
	void TimeRangeChanged();

private:
	QRectF SceneVisibleRect() const;

private:
	QDateTime m_startDateTime;
	QDateTime m_endDateTime;
	int m_minorTickCount;
	QColor m_color;
	QList<QGraphicsItem*> m_sceneItems;
};


} // namespace imtloggui


