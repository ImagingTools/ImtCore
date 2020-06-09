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
	void SetContainer(QGraphicsItem* containerPtr);

	QRectF GetSceneVisibleRect() const;
	double GetScaleX() const;
	double GetScaleY() const;

	QRectF GetSceneRect();
	void SetSceneRect(const QRectF& rect);
	void SetViewRect(const QRectF& rect);
	void MoveViewRect(double dX, double dY);
	void ScaleViewRect(const QPointF& center, double scaleX, double scaleY);

Q_SIGNALS:
	void EmitViewPortChanged(bool userAction);

public Q_SLOTS:
	void OnMinimumVerticalScaleChanged(double minScale);

protected:
	// reimplemented (QGraphicsView)
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;

private Q_SLOTS:
	void OnRangeChanged(int min, int max);
	void OnValueChanged(int value);

private:
	void ValidateViewRect();
	void UpdateViewRect();

private:
	CTimeAxis* m_timeAxisPtr;

	QPointF m_lockedScenePoint;

	double m_userAction;
	double m_minimumVerticalScale;
	QRectF m_sceneRect;
	QRectF m_viewRect;

	//QGraphicsItem* m_containerPtr;
};


} // namespace imtloggui


