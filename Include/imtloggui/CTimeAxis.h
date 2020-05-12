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
	bool SetMinorTickCount(int count);
	void SetColor(const QColor& color);
	void EnsureTimeRange(const QDateTime& time);
	void AdaptTickPitch();

	// reimplemented (QGraphicsRectItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	// reimplemented (IEventScenePositionProvider)
	virtual double GetScenePositionFromTime(const QDateTime& time) const override;
	virtual QDateTime GetTimeFromScenePosition(double position) const override;

Q_SIGNALS:
	void AxisChanged();

private:
	QRectF SceneVisibleRect() const;

private:
	QDateTime m_baseTime;
	QDateTime m_firstEvent;
	QDateTime m_lastEvent;
	QDateTime m_startTime;
	QDateTime m_endTime;
	int m_minorTickCount;
	QColor m_color;

	double m_majorTickPitch;
	double m_minorTickPitch;
};


} // namespace imtloggui


