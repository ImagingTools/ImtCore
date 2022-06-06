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

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtgui/IClientAreaProvider.h>
#include <imtgui/IPopupController.h>
#include <imtgui/IPopupEventHandler.h>
#include <imtgui/IPopupWidget.h>


namespace imtgui
{


class CPopupControllerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public IPopupController
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPopupControllerComp);
		I_REGISTER_INTERFACE(IPopupController);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_clientAreaProviderCompPtr, "ClientArea", "Desktop area for organizing popups", false, "ClientAreaProvider");
		I_ASSIGN(m_popupWidgetFactoryCompPtr, "PopupWidgetFactory", "Widget factory for popup messages and content widgets", false, "StandardPopupWidgetFactory");
		I_ASSIGN_MULTI_0(m_popupEventHandlerCompPtr, "PopupEventHandler", "Popup event handlers", false, "PopupEventHandler");
		I_ASSIGN(m_anchorAttrPtr, "AnchorCorner", "0 - TopLeft\n1 - TopRight\n2 - BottomLeft\n3 - BottomRight", true, 3);
		I_ASSIGN(m_xRatioAttrPtr, "HorizontalRatio", "Horizontal ratio", true, 0.25);
		I_ASSIGN(m_yRatioAttrPtr, "VerticalRatio", "Vertical ratio", true, 1);
		I_ASSIGN(m_spacingAttrPtr, "Spacing", "Spacing between adjacent popups", true, 5);
		I_ASSIGN(m_defaultTimeoutAttrPtr, "DefaultTimeout", "Timeout value in msecs before the popup closes, a zero or negative value requires user action to close", true, 3000);
	I_END_COMPONENT;

	CPopupControllerComp();

	// reimplemented (IPopupController)
	virtual QByteArray AddPopup(const MessagePtr& messagePtr, int timeout = -1, bool isÑlosingOnMouseClickAllowed = true, QWidget* contentWidgetPtr = nullptr) override;
	virtual bool ClosePopup(const QByteArray& popupId) override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnPrimaryScreenChanged();
	void OnScreenGeometryChanged(const QRect& geometry);
	void OnClientAreaUpdated(const istd::IChangeable::ChangeSet& changeSet, const IClientAreaProvider* objectPtr);

	void OnHoverEnter(QEvent* event);
	void OnHoverLeave(QEvent* event);
	void OnCloseRequest();
	void OnPopupTimeout();
	void OnFadeFinished();

private:
	struct PopupItem;

	void CalculatePopupAreaFromScreen();
	QByteArray CreatePopupItem(const MessagePtr& messagePtr, int timeout, bool isÑlosingOnMouseClickAllowed, QWidget* contentWidgetPtr);
	void RemovePopupItem(int index);
	int GetVisibleItemIndex(const QByteArray& id);
	int GetVisibleItemIndex(const QObject* memberPtr);

	void ValidateVisibleItems();
	void ArrangeVisibleItems();
	void ShowItems();

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
	I_REF(IClientAreaProvider, m_clientAreaProviderCompPtr);
	I_REF(IPopupWidgetFactory, m_popupWidgetFactoryCompPtr);
	I_MULTIREF(IPopupEventHandler, m_popupEventHandlerCompPtr);
	I_ATTR(int, m_anchorAttrPtr);
	I_ATTR(double, m_xRatioAttrPtr);
	I_ATTR(double, m_yRatioAttrPtr);
	I_ATTR(int, m_spacingAttrPtr);
	I_ATTR(int, m_defaultTimeoutAttrPtr);

	QPoint m_anchor;
	QSize m_size;

	imtbase::TModelUpdateBinder<IClientAreaProvider, CPopupControllerComp> m_clientAreaObserver;

	QList<PopupItemPtr> m_items;
	QList<PopupItemPtr> m_visibleItems;

	bool m_mouseOnPopup;
};


} // namespace imtgui


