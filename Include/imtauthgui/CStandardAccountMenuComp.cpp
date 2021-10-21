#include <imtauthgui/CStandardAccountMenuComp.h>


namespace imtauthgui
{


// public methods

CStandardAccountMenuComp::CStandardAccountMenuComp()
	:m_loginObserver(*this)
{
}


bool CStandardAccountMenuComp::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress){
		if (watched == LogoutFrame){
			QWidget* widgetPtr =  GetQtWidget();
			if (widgetPtr != nullptr){
				widgetPtr->hide();
			}

			if (m_loginCompPtr.IsValid()){
				m_loginCompPtr->Logout();
			}
		}

		if (watched == PreferencesFrame){
			QWidget* widgetPtr = GetQtWidget();
			if (widgetPtr != nullptr){
				widgetPtr->hide();
			}
			
			if (m_preferencesDialogCompPtr.IsValid()){
				m_preferencesDialogCompPtr->ExecuteDialog(this);
			}
		}
	}

	return BaseClass::eventFilter(watched, event);
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CStandardAccountMenuComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	GetQtWidget()->setWindowFlags(Qt::Popup);

	QIcon accountIcon = GetIcon(*m_accountPreviewAttrPtr);
	QIcon preferencesIcon = GetIcon(*m_preferencesIconAttrPtr);
	QIcon logoutIcon = GetIcon(*m_logoutIconAttrPtr);
	AccountPreview->setPixmap(accountIcon.pixmap(*m_iconSizeAttrPtr));
	PreferencesIcon->setPixmap(preferencesIcon.pixmap(*m_iconSizeAttrPtr));
	LogoutIcon->setPixmap(logoutIcon.pixmap(*m_iconSizeAttrPtr));

	LogoutFrame->installEventFilter(this);
	PreferencesFrame->installEventFilter(this);

	if (m_loginCompPtr.IsValid()){
		m_loginObserver.RegisterObject(m_loginCompPtr.GetPtr(), &CStandardAccountMenuComp::OnLoginUpdate);
	}
}


void CStandardAccountMenuComp::OnGuiDestroyed()
{
	m_loginObserver.UnregisterAllObjects();

	BaseClass::OnGuiDestroyed();
}


// reimplemented (ibase::TDesignSchemaHandlerWrap)

void CStandardAccountMenuComp::OnDesignSchemaChanged()
{
	BaseClass::OnDesignSchemaChanged();

	QIcon accountIcon = GetIcon(*m_accountPreviewAttrPtr);
	QIcon preferencesIcon = GetIcon(*m_preferencesIconAttrPtr);
	AccountPreview->setPixmap(accountIcon.pixmap(*m_iconSizeAttrPtr));
	PreferencesIcon->setPixmap(preferencesIcon.pixmap(*m_iconSizeAttrPtr));
}


// private methods

void CStandardAccountMenuComp::OnLoginUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/, const iauth::ILogin* objectPtr)
{
	iauth::CUser* userPtr = objectPtr->GetLoggedUser();
	if (userPtr != nullptr){
		QString userName = userPtr->GetUserName();
		UserName->setText(userPtr->GetUserName());
		userName.replace(" ", "");

		//Email->setText(userName + "@yahoo.com");
		Email->setVisible(false);
	}
}


} // namespace imtauthgui


