#pragma once


// Qt includes
#include <QtWidgets/QWidget>

// ImtCore includes
#include <imtgui/IPopupWidget.h>


namespace imtgui
{


class CPopupWidgetBase: public QWidget, virtual public IPopupWidget
{
	Q_OBJECT
public:
	typedef QWidget BaseClass;

	CPopupWidgetBase();

	// reimplemented (IPopupWidget)
	virtual const ilog::IMessageConsumer::MessagePtr& GetMessagePtr() const override;
	virtual void AllowClosingOnMouseClick(bool isAllowed = true) override;

Q_SIGNALS:
	void EmitHoverEnter(QEvent* event);
	void EmitHoverLeave(QEvent* event);
	void EmitCloseRequest();

protected:
	// reimplemented (QWidget)
#if QT_VERSION < 0x060000
	virtual void enterEvent(QEvent* event) override;
#else
	virtual void enterEvent(QEnterEvent* event) override;
#endif
	virtual void leaveEvent(QEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;

protected:
	ilog::IMessageConsumer::MessagePtr m_messagePtr;
	bool m_isClosingOnMouseClickAllowed;
};


} // namespace imtgui


