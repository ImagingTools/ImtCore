#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtWidgets/QGraphicsOpacityEffect>
#include <QtWidgets/QWidget>
#include <QtCore/QParallelAnimationGroup>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QVariantAnimation>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/CMessageContainer.h>
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtgui/IClientAreaProvider.h>
#include <imtgui/IPopupController.h>
#include <imtgui/IPopupEventHandler.h>
#include <imtgui/IPopupWidget.h>


namespace imtgui
{


class CPopupController:
			public QObject,
			virtual public IPopupController
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	CPopupController();

	void SetAnchorCorner(Qt::Corner corner);
	void SetParentWidget(QWidget* parentPtr);
	void SetScreen(QScreen* screenPtr);
	void SetRatioX(double ratio);
	void SetRatioY(double ratio);
	void SetSpacing(double spacing);
	void SetDefaultTimeOut(int msec);
	void SetPopupWidgetFactory(IPopupWidgetFactory* factoryPtr);

	void SetEnabled(bool enable);

	// reimplemented (IPopupController)
	virtual QByteArray AddPopup(const MessagePtr& messagePtr, int timeout = -1, bool isClosingOnMouseClickAllowed = true, QWidget* contentWidgetPtr = nullptr) override;
	virtual void ClosePopup(const QByteArray& popupId) override;
	virtual void RegisterEventHandler(IPopupEventHandler* handlerPtr) override;
	virtual void UnRegisterEventHandler(IPopupEventHandler* handlerPtr) override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

Q_SIGNALS:
	void EmitAddMessage(const QByteArray& id, const MessagePtr& messagePtr);
	void EmitAddPopup(
		const QByteArray& id,
		const MessagePtr& messagePtr,
		int timeout,
		bool isClosingOnMouseClickAllowed,
		QWidget* contentWidgetPtr);
	void EmitClosePopup(const QByteArray& popupId);

private Q_SLOTS:
	void OnAddMessage(const QByteArray& id, const MessagePtr& messagePtr);
	void OnAddPopup(
		const QByteArray& id,
		const MessagePtr& messagePtr,
		int timeout,
		bool isClosingOnMouseClickAllowed,
		QWidget* contentWidgetPtr);
	void OnClosePopup(const QByteArray& popupId);

	void OnScreenGeometryChanged(const QRect& rect);

	void OnHoverEnter(QEvent* event);
	void OnHoverLeave(QEvent* event);
	void OnCloseRequest();
	void OnPopupTimeout();
	void OnFadeFinished();

private:
	struct PopupItem;

	void CreatePopupItem(
		const QByteArray& id,
		const MessagePtr& messagePtr,
		int timeout,
		bool isClosingOnMouseClickAllowed,
		QWidget* contentWidgetPtr);
	void RemovePopupItem(int index);
	int GetVisibleItemIndex(const QByteArray& id);
	int GetVisibleItemIndex(const QObject* memberPtr);
	int GetInvisibleItemIndex(const QByteArray& id);

	void ValidateVisibleItems();
	void ArrangeVisibleItems();
	void ShowItems();
	void HideItems();

	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private:
	struct PopupItem
	{
		PopupItem()
			:widgetPtr(nullptr),
			timeout(-1)
		{
			fadeAnimation.setTargetObject(&effect);
			fadeAnimation.setPropertyName("opacity");
			fadeAnimation.setDuration(500);
			fadeAnimation.setStartValue(0);
			fadeAnimation.setEndValue(1);
		}

		~PopupItem()
		{
			timeoutTimer.stop();
			fadeAnimation.stop();
			widgetPtr->hide();
			widgetPtr->deleteLater();
		}

		QByteArray id;
		QWidget* widgetPtr;
		int timeout;
		QTimer timeoutTimer;
		QGraphicsOpacityEffect effect;
		QPropertyAnimation fadeAnimation;
	};
	typedef istd::TSmartPtr<PopupItem> PopupItemPtr;

private:
	bool m_enabled;
	QRect m_rect;
	Qt::Corner m_corner;
	QWidget* m_parentPtr;
	QScreen* m_screenPtr;
	double m_ratioX;
	double m_ratioY;
	double m_spacing;
	int m_timeout;
	IPopupWidgetFactory* m_widgetFactoryPtr;

	QList<IPopupEventHandler*> m_eventHandlers;
	QMutex m_eventHandlersMutex;

	QList<PopupItemPtr> m_items;
	QList<PopupItemPtr> m_visibleItems;

	bool m_mouseOnPopup;
};


} // namespace imtgui


