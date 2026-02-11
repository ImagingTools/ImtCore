// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlGuiCompBase.h>


// Qt includes
#include <QtQml/QQmlEngine>


namespace imtqml
{


CQmlGuiCompBase::CQmlGuiCompBase()
	:m_quickWidget(nullptr)
{
	EnableDesignHandler(true);
}


bool CQmlGuiCompBase::IsGuiShown() const
{
	return m_isGuiShown;
}


// reimplemented (iqtgui::IGuiObject)

bool CQmlGuiCompBase::IsGuiCreated() const
{
	return  m_quickWidget != nullptr;
}


bool CQmlGuiCompBase::CreateGui(QWidget* parentPtr)
{
	if (m_quickWidget == nullptr){
		m_quickWidget = new QQuickWidget(parentPtr);
		m_quickWidget->setContentsMargins(0, 0, 0, 0);
		
		m_quickWidget->engine()->addImportPath("qrc:/qml");

		m_quickWidget->setSource(QUrl(*m_pathToQmlAttrPtr));

		m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

		if (parentPtr != nullptr){
			QLayout* layoutPtr = parentPtr->layout();
			if (layoutPtr != nullptr){
				layoutPtr->addWidget(m_quickWidget);
			}
		}

		m_quickWidget->installEventFilter(this);

		EnableLocalization(true);

		OnGuiCreated();
		OnGuiRetranslate();
		OnGuiDesignChanged();

		QQuickItem* quickItemPtr = m_quickWidget->rootObject();
		if (quickItemPtr != nullptr){
			connect(quickItemPtr, &QQuickItem::destroyed, this, &CQmlGuiCompBase::OnQuickItemDestroyed);
		}
	}
	else{
		Q_ASSERT(false);
	}

	return m_quickWidget != nullptr;
}


bool CQmlGuiCompBase::DestroyGui()
{
	QQuickItem* quickItemPtr = m_quickWidget->rootObject();
	if (quickItemPtr != nullptr){
		disconnect(quickItemPtr, &QQuickItem::destroyed, this, &CQmlGuiCompBase::OnQuickItemDestroyed);
	}

	EnableLocalization(false);

	if (m_quickWidget != nullptr){
		OnGuiDestroyed();

		m_quickWidget->deleteLater();

		m_quickWidget = nullptr;
	}

	return true;
}


QWidget* CQmlGuiCompBase::GetWidget() const
{
	return m_quickWidget;
}


void CQmlGuiCompBase::OnTryClose(bool* ignoredPtr)
{
	if (m_quickWidget != nullptr){
		m_quickWidget->close();

		if (ignoredPtr != nullptr){
			*ignoredPtr = false;
		}
	}
}


// protected methods

void CQmlGuiCompBase::OnGuiDesignChanged()
{
}


void CQmlGuiCompBase::OnGuiShown()
{
	if (m_hasPendingDesignChanges){
		OnGuiDesignChanged();

		m_hasPendingDesignChanges = false;
	}
}


void CQmlGuiCompBase::OnGuiHidden()
{
}


void CQmlGuiCompBase::OnRetranslate()
{
}


void CQmlGuiCompBase::OnGuiRetranslate()
{
}


void CQmlGuiCompBase::OnGuiCreated()
{
}


void CQmlGuiCompBase::OnGuiDestroyed()
{
}


// reimplemented (ibase::TLocalizableWrap)

void CQmlGuiCompBase::OnLanguageChanged()
{
	OnRetranslate();

	if (IsGuiCreated()){
		OnGuiRetranslate();
	}
}


// reimplemented (ibase::TDesignSchemaHandlerWrap)

void CQmlGuiCompBase::OnDesignSchemaChanged(const QByteArray & themeId)
{
	BaseClass::OnDesignSchemaChanged(themeId);

	if (m_defaultStatusIconPathAttrPtr.IsValid()){
		istd::CChangeNotifier changeNotifier(&m_visualStatus);

		m_visualStatus.m_statusIcon = GetIcon(*m_defaultStatusIconPathAttrPtr);
	}

	if (IsGuiShown()){
		OnGuiDesignChanged();
	}
	else{
		m_hasPendingDesignChanges = true;
	}
}


// reimplemented (icomp::CComponentBase)

void CQmlGuiCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_defaultStatusIconPathAttrPtr.IsValid()){
		m_visualStatus.m_statusIcon = GetIcon(*m_defaultStatusIconPathAttrPtr);
	}

	m_visualStatus.m_statusText = *m_defaultStatusTextAttrPtr;
}


// reimplemented (QObject)

bool CQmlGuiCompBase::eventFilter(QObject* senderPtr, QEvent* eventPtr)
{
	QWidget* widgetPtr = GetWidget();
	if ((senderPtr != widgetPtr) || (widgetPtr == NULL)){
		return false;
	}

	switch (eventPtr->type()){
	case QEvent::Close:
	{
		bool ignoreClose = false;
		OnTryClose(&ignoreClose);
		if (ignoreClose){
			eventPtr->ignore();

			return true;
		}
	}
	break;

	case QEvent::Show:
		if (!m_isGuiShown){
			m_isGuiShown = true;
			OnGuiShown();
		}
		break;

	case QEvent::Hide:
		if (m_isGuiShown){
			m_isGuiShown = false;
			OnGuiHidden();
		}
		break;

	default:
		break;
	}

	return false;
}


// protected slots

void CQmlGuiCompBase::OnQuickItemDestroyed()
{
}


// public methods of embedded class VisualStatus

// reimplemented (iqtgui::IVisualStatus)

QIcon CQmlGuiCompBase::VisualStatus::GetStatusIcon() const
{
	return m_statusIcon;
}


QString CQmlGuiCompBase::VisualStatus::GetStatusText() const
{
	return m_statusText;
}


} // namespace imtqml



