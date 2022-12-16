#pragma once


// Qt includes
#include <QtCore/qmetaobject.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QShowEvent>
#include <QtWidgets/QDialog>

#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
	#include <QtGui/QScreen>
#else
	#include <QtGui/QDesktopWidget>
#endif


namespace imtgui
{


template <class UI>
class TDesignerGuiDialogBase: public QDialog, public UI
{
public:
	typedef QDialog BaseClass;

	TDesignerGuiDialogBase();

	void SetDialogGeometry(double screenFactorX, double screenFactorY, const QPoint& position = QPoint());
	void SetTitlePanel(QWidget* titlePanelPtr);

protected:
	// reimplemented (QWidget)
	virtual void showEvent(QShowEvent* eventPtr) override;

	// reimplemented (QObject)
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private:
	void ApplyGeometry();

private:
	QWidget* m_titlePanelPtr;

	double m_screenFactorX;
	double m_screenFactorY;
	QPoint m_screenPosition;

	bool m_cursorLocked;
	QPoint m_pos;
};


// public methods

template<class UI>
inline TDesignerGuiDialogBase<UI>::TDesignerGuiDialogBase()
	:m_screenFactorX(0),
	m_screenFactorY(0),
	m_cursorLocked(false),
	m_titlePanelPtr(nullptr)
{
	UI::setupUi(this);
}


template<class UI>
void TDesignerGuiDialogBase<UI>::SetDialogGeometry(double screenFactorX, double screenFactorY, const QPoint& position)
{
	m_screenFactorX = screenFactorX;
	m_screenFactorY = screenFactorY;
	m_screenPosition = position;

	ApplyGeometry();
}


template<class UI>
void TDesignerGuiDialogBase<UI>::SetTitlePanel(QWidget* titlePanelPtr)
{
	if (m_titlePanelPtr != nullptr) {
		m_titlePanelPtr->removeEventFilter(this);
	}

	m_titlePanelPtr = titlePanelPtr;
	if (m_titlePanelPtr != nullptr) {
		m_titlePanelPtr->installEventFilter(this);
	}
}


// protected methods

// reimplemented (QWidget)

template<class UI>
void TDesignerGuiDialogBase<UI>::showEvent(QShowEvent* eventPtr)
{
	BaseClass::showEvent(eventPtr);

	if (!eventPtr->spontaneous()) {
		ApplyGeometry();
	}
}


// reimplemented (QObject)

template<class UI>
bool TDesignerGuiDialogBase<UI>::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == m_titlePanelPtr) {
		QMouseEvent* eventPtr = dynamic_cast<QMouseEvent*>(event);
		if (eventPtr != nullptr) {
			switch (event->type()) {
			case QEvent::MouseButtonPress:
				m_cursorLocked = true;
				m_pos = eventPtr->globalPos();
				break;
			case QEvent::MouseButtonRelease:
				m_cursorLocked = false;
				break;
			case QEvent::MouseMove:
				if (m_cursorLocked) {
					move(pos() + (eventPtr->globalPos() - m_pos));
					m_pos = eventPtr->globalPos();
				}
				break;
			case QEvent::Leave:
				m_cursorLocked = false;
				break;
			}
		}
	}

	return BaseClass::eventFilter(watched, event);
}


// private methods

template<class UI>
void TDesignerGuiDialogBase<UI>::ApplyGeometry()
{
	BaseClass::adjustSize();

	QRect screenRect;
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
	screenRect = screen()->geometry();
#else
	const QDesktopWidget* desktopPtr = QApplication::desktop();
	Q_ASSERT(desktopPtr != NULL);

	screenRect = desktopPtr->screenGeometry();
#endif

	if (m_screenFactorX > 0 && m_screenFactorY > 0) {
		double screenFactorX = qMin(0.99, m_screenFactorX);
		double screenFactorY = qMin(0.99, m_screenFactorY);

		BaseClass::resize(int(screenRect.width() * screenFactorX), int(screenRect.height() * screenFactorY));
	}
	else {
		BaseClass::resize(sizeHint());
	}

	if (m_screenPosition.isNull()) {
		QSize dialogHalfSize = size() / 2;

		BaseClass::move(screenRect.center() - QPoint(dialogHalfSize.width(), dialogHalfSize.height()));
	}
	else {
		BaseClass::move(m_screenPosition);
	}
}


} // namespace imtgui


