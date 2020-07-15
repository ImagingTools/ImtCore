#pragma once


// Qt includes
#include <QtGui/QkeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QGraphicsView>

// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtBase includes
#include <imtloggui/IViewRectProvider.h>


namespace imtloggui
{


class CGraphicsView: public QGraphicsView, public imod::TSingleModelObserverBase<imtloggui::IViewRectProvider>
{
	Q_OBJECT
public:
	typedef imod::TSingleModelObserverBase<imtloggui::IViewRectProvider> BaseClass;

	CGraphicsView(QWidget *parent = nullptr);

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (QGraphicsView)
	virtual void resizeEvent(QResizeEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void keyPressEvent(QKeyEvent *event) override;
	virtual void keyReleaseEvent(QKeyEvent *event) override;

Q_SIGNALS:
	void EmitResizeEvent(QResizeEvent *event);
	void EmitMouseMoveEvent(QMouseEvent *event);
	void EmitMousePressEvent(QMouseEvent *event);
	void EmitMouseReleaseEvent(QMouseEvent *event);
	void EmitWheelEvent(QWheelEvent* event);
	void EmitKeyPressEvent(QKeyEvent *event);
	void EmitKeyReleaseEvent(QKeyEvent *event);
};


} // namespace imtloggui


