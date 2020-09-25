#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsView>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtloggui/CTimeAxis.h>
#include <imtloggui/IViewPropertyProvider.h>
#include <imtloggui/IViewPropertyManager.h>


namespace imtloggui
{


class CEventGraphicsView:
			public QGraphicsView,
			virtual public istd::IChangeable,
			virtual public IViewPropertyProvider,
			virtual public IViewPropertyManager
{
	Q_OBJECT

public:
	typedef QGraphicsView BaseClass;

	CEventGraphicsView(QWidget* parent = nullptr);

	// reimplemented (imtloggui::IViewPropertyProvider)
	virtual QRectF GetSceneRect() const override;
	virtual QRectF GetViewRect() const override;
	virtual QMargins GetMargins() const override;
	virtual istd::CRange GetScaleRangeX() const override;
	virtual istd::CRange GetScaleRangeY() const override;
	virtual double GetScaleX() const override;
	virtual double GetScaleY() const override;

	// reimplemented (imtloggui::IViewPropertyManager)
	virtual bool SetSceneRect(const QRectF& rect) override;
	virtual bool SetViewRect(const QRectF& rect) override;
	virtual bool SetMargins(const QMargins& margins) override;
	virtual bool SetScaleRangeX(const istd::CRange& range) override;
	virtual bool SetScaleRangeY(const istd::CRange& range) override;

Q_SIGNALS:
	void EmitViewPortChanged(bool userAction);

public Q_SLOTS:
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
	istd::CRange m_scaleRangeX;
	istd::CRange m_scaleRangeY;
};


} // namespace imtloggui


