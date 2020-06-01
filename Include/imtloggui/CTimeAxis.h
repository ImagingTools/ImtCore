#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QStyleOptionGraphicsItem>

// ImtCore includes
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CTimeAxis : public QObject, public QGraphicsRectItem, virtual public IEventScenePositionProvider
{
	Q_OBJECT

public:
	typedef QGraphicsRectItem BaseClass;

	// Quantization thresholds in secs
	enum QuantizationThresholds
	{
		QUA_DAY = 86400 * 3,
		QUA_MONTH = 86400 * 365,
	};

	CTimeAxis(QGraphicsItem* parent = nullptr);

	void SetColor(const QColor& color);
	void EnsureTimeRange(const QDateTime& time);
	int GetMargin();
	void CreateTimeItemTable();

	// reimplemented (QGraphicsRectItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	// reimplemented (QGraphicsItem)
	void setPos(const QPointF& origin);
	void setPos(double x, double y);

	// reimplemented (IEventScenePositionProvider)
	virtual double GetScenePositionFromTime(const QDateTime& time) const override;
	virtual QDateTime GetTimeFromScenePosition(double position) const override;
	virtual QDateTime GetBeginTime() const;
	virtual QDateTime GetEndTime() const;
	virtual QDateTime GetVisibleBeginTime() const;
	virtual QDateTime GetVisibleEndTime() const;

Q_SIGNALS:
	void EmitAxisChanged();

protected:
	enum TickType
	{
		TT_MAJOR = 0,
		TT_MINOR
	};

	enum TimeInterval
	{
		TI_1MS = 0,
		TI_10MS,
		TI_100MS,

		TI_1S,
		TI_5S,
		TI_15S,
		TI_30S,

		TI_1M,
		TI_5M,
		TI_15M,
		TI_30M,

		TI_1H,
		TI_3H,
		TI_6H,
		TI_12H,

		TI_DAY,
		TI_WEEK,
		TI_MONTH,
		TI_QUARTER,
		TI_YEAR,

		TI_COUNT,
		TI_NONE
	};

	struct IntervalsInfo
	{
		TimeInterval majorInterval;
		TimeInterval minorInterval;
		QString timeFormat;
	};

	struct TickInfo
	{
		TickType type;
		QString timeFormat;
	};

	typedef QMap<QDateTime, TickInfo> Ticks;

	struct	MinorIntervalItem
	{
		double scaleMin;
		double distance;
		TimeInterval interval;
	};

	struct MajorIntervalItem
	{
		double scaleMin;
		double scaleMax;
		double distance;
		TimeInterval interval;
		QString timeFormat;

		QVector<MinorIntervalItem> minorIntervals;
	};

	QVector<MajorIntervalItem> m_intervals;

protected:
	IntervalsInfo CalculateIntervals(double scale) const;
	Ticks CalculateTicks(const IntervalsInfo& intervalsInfo) const;

	double GetCurrentScaleX() const;
	double GetCurrentScaleY() const;
	QRectF GetSceneVisibleRect() const;
	QRectF GetAxisVisibleRect() const;

	double GetRectPositionFromTime(const QDateTime& time) const;
	QDateTime GetTimeFromRectPosition(double position) const;

private:
	QDateTime m_baseTime;
	QDateTime m_firstEvent;
	QDateTime m_lastEvent;
	QDateTime m_startTime;
	QDateTime m_endTime;
	QColor m_color;
	QFontMetrics m_fontMetrics;

	double m_minMinorTickStep;
	double m_labelWidthFactor;
};


} // namespace imtloggui


