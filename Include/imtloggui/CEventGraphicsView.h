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

	QRectF GetSceneVisibleRect() const;
	double GetScaleX() const;
	double GetScaleY() const;

	QRectF GetSceneRect();
	void SetSceneRect(const QRectF& rect);
	QRectF GetViewRect();
	void SetViewRect(const QRectF& rect);

	const QMargins& GetMargins();
	void SetMargins(const QMargins& margins);

Q_SIGNALS:
	void EmitViewPortChanged(bool userAction);

public Q_SLOTS:
	void OnMinimumVerticalScaleChanged(double minScale);
	void OnShowAll();

protected:
	// reimplemented (QGraphicsView)
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
	void MoveViewRect(double dX, double dY);
	void ScaleViewRect(const QPointF& center, double scaleX, double scaleY);
	void ValidateViewRect();
	void UpdateViewRect();

private:
	QPointF m_lockedScenePoint;

	double m_userAction;
	double m_minimumVerticalScale;
	QRectF m_sceneRect;
	QRectF m_viewRect;
	QMargins m_margins;
};


} // namespace imtloggui


