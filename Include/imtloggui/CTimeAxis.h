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

	void setPos(const QPointF& origin);
	void setPos(double x, double y);

	const QDateTime& GetStartOfRange() const;
	const QDateTime& GetEndOfRange() const;
	void SetColor(const QColor& color);
	void EnsureTimeRange(const QDateTime& time);
	int GetMargin();
	void CreateTimeItemTable();


	// reimplemented (QGraphicsRectItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	// reimplemented (IEventScenePositionProvider)
	virtual double GetScenePositionFromTime(const QDateTime& time) const override;
	virtual QDateTime GetTimeFromScenePosition(double position) const override;

Q_SIGNALS:
	void AxisChanged();

protected:
	enum TickType
	{
		TT_MAJOR = 0,
		TT_MINOR
	};

	enum TickInterval
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

	struct TimeItemInfo
	{
		TickInterval majorTimeInterval;
		TickInterval minorTimeInterval;
		QString majorTimeFormat;
	};

	struct TickInfo
	{
		QDateTime time;
		TickType type;
		QString majorTimeFormat;
	};

	typedef QVector<TickInfo> Ticks;

	struct	MinorItem
	{
		double scaleMin;
		double distance;
		TickInterval interval;
	};

	struct MajorItem
	{
		double scaleMin;
		double scaleMax;
		double distance;
		TickInterval interval;
		QString timeFormat;

		QVector<MinorItem> minorItemTable;
	};

	QVector<MajorItem> m_majorItemTable;

protected:
	TimeItemInfo CalculateTimeItems(double scale) const;
	Ticks GenerateTicks(const TimeItemInfo& timeItemInfo) const;

private:
	QRectF SceneVisibleRect() const;
	virtual QDateTime GetTimeFromRectPosition(double position) const;

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


