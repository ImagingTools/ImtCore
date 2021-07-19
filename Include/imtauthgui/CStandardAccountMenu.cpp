#include <imtauthgui/CStandardAccountMenu.h>


namespace imtauthgui
{


// public methods

CStandardAccountMenu::CStandardAccountMenu()
	:m_loginObserver(*this)
{
}


bool CStandardAccountMenu::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress){
		if (watched == LogoutFrame){
			GetQtWidget()->hide();
			if (m_loginCompPtr.IsValid()){
				m_loginCompPtr->Logout();
			}

			return true;
		}

		if (watched == PreferencesFrame){
			GetQtWidget()->hide();
			if (m_preferencesDialogCompPtr.IsValid()){
				m_preferencesDialogCompPtr->ExecuteDialog(nullptr);
			}

			return true;
		}
	}

	return false;
}


// protected methods

void CStandardAccountMenu::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	GetQtWidget()->setWindowFlags(Qt::Popup);

	QIcon accountIcon(*m_accountPreviewAttrPtr);
	QIcon preferencesIcon(*m_preferencesIconAttrPtr);

	AccountPreview->setPixmap(accountIcon.pixmap(*m_iconSizeAttrPtr));
	PreferencesIcon->setPixmap(preferencesIcon.pixmap(*m_iconSizeAttrPtr));

	LogoutFrame->installEventFilter(this);
	PreferencesFrame->installEventFilter(this);

	if (m_loginCompPtr.IsValid()){
		m_loginObserver.RegisterObject(m_loginCompPtr.GetPtr(), &CStandardAccountMenu::OnLoginUpdate);
	}
}


void CStandardAccountMenu::OnGuiDestroyed()
{
	m_loginObserver.UnregisterAllObjects();

	BaseClass::OnGuiDestroyed();
}


// private methods

void CStandardAccountMenu::OnLoginUpdate(const istd::IChangeable::ChangeSet& changeSet, const iauth::ILogin* objectPtr)
{
	if (m_loginCompPtr.IsValid()){
		iauth::CUser* userPtr = m_loginCompPtr->GetLoggedUser();
		if (userPtr != nullptr){
			QString userName = userPtr->GetUserName();
			UserName->setText(userPtr->GetUserName());
			userName.replace(" ", "");
			Email->setText(userName + "@yahoo.com");
		}
	}
}


} // namespace imtauthgui


