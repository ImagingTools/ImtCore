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
	void EmitViewPortChanged(bool userAction);

public Q_SLOTS:
	void OnAxisPositionChanged();
	void OnMinimumVerticalScaleChanged(double minScale);

protected:
	// reimplemented (QGraphicsView)
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;

	// reimplemented (QObject)
	virtual bool eventFilter(QObject *, QEvent *) override;

private Q_SLOTS:
	void OnRangeChanged(int min, int max);
	void OnValueChanged(int value);

private:
	QRectF SceneVisibleRect() const;

private:
	CTimeAxis* m_timeAxisPtr;

	double m_userAction;
	double m_minimumVerticalScale;
};


} // namespace imtloggui


