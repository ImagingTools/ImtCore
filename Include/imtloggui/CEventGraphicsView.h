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

	void SetTimeAxis(CTimeAxis* timeAxisPtr);

Q_SIGNALS:
	void EmitAxisPositionChanged();
	void EmitViewPortChanged();

public Q_SLOTS:
	void OnAxisPositionChanged();
	void OnMinimumVerticalScaleChanged(double minScale);

protected:
	// reimplemented (QGraphicsView)
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private Q_SLOTS:
	void OnRangeChanged(int min, int max);
	void OnValueChanged(int value);

private:
	QRectF SceneVisibleRect() const;

private:
	CTimeAxis* m_timeAxisPtr;

	double m_minimumVerticalScale;
};


} // namespace imtloggui


