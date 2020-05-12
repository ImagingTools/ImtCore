#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsView>

// ImtCore includes
#include <imtloggui/CTimeAxis.h>


namespace imtloggui
{


class CEventGraphicsView: public QGraphicsView
{
	Q_OBJECT

public:
	typedef QGraphicsView BaseClass;

	CEventGraphicsView(QWidget* parent = nullptr);

	void setTimeAxis(CTimeAxis* timeAxisPtr);

Q_SIGNALS:
	void AxisPositionChanged();

public Q_SLOTS:
	void OnAxisPositionChanged();

protected:
	// reimplemented (QGraphicsView)
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

protected Q_SLOTS:
	void rangeChanged(int min, int max);
	void valueChanged(int value);

private:
	QRectF SceneVisibleRect() const;

private:
	QTransform m_lastTransform;
	CTimeAxis* m_timeAxisPtr;
};


} // namespace imtloggui


