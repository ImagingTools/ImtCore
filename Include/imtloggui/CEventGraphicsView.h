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
public:
	typedef QGraphicsView BaseClass;

	CEventGraphicsView(QWidget *parent = nullptr);

	void setTimeAxis(CTimeAxis* timeAxisPtr);

protected:
	// reimplemented (QGraphicsView)
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

	// reimplemented (QAbstractScrollArea)
	bool viewportEvent(QEvent *event) override;

private slots:
	void ScrollRangeChanged(int min, int max);
	void ScrollValueChanged(int value);

private:
	CTimeAxis* m_timeAxisPtr;
};


} // namespace imtloggui


