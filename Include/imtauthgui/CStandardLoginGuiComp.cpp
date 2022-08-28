#include <imtauthgui/CStandardLoginGuiComp.h>


// Qt includes
#include <QtGui/QKeyEvent>


namespace imtauthgui
{


// public methods

CStandardLoginGuiComp::CStandardLoginGuiComp()
	:m_loginObserver(*this),
	m_loginLog(*this)
{
}


bool CStandardLoginGuiComp::eventFilter(QObject* watched, QEvent* event)
{
	if (m_loginCompPtr.IsValid()){
		bool isLogged = m_loginCompPtr->GetLoggedUser() != NULL;

		QKeyEvent* keyEventPtr = dynamic_cast<QKeyEvent*>(event);
		if (keyEventPtr != nullptr){
			int pressedKey = keyEventPtr->key();

			if ((pressedKey == Qt::Key_Return || pressedKey == Qt::Key_Enter)){
				if (!isLogged){
					on_LoginButton_clicked();
					return true;
				}
			}
		}
	}

	return BaseClass::eventFilter(watched, event);
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CStandardLoginGuiComp::OnGuiShown()
{
	BaseClass::OnGuiShown();

	if (!UserEdit->text().isEmpty()){
		PasswordEdit->setFocus();
		PasswordEdit->setCursorPosition(0);
	}

	PasswordEdit->setText("");
	PasswordEdit->setStyleSheet("");
	PasswordLabel->setStyleSheet("");
	PasswordMessage->setText("");
}


void CStandardLoginGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	LoginLogText->setVisible(false);

	if (m_loginCompPtr.IsValid()){
		m_loginObserver.RegisterObject(m_loginCompPtr.GetPtr(), &CStandardLoginGuiComp::OnLoginUpdate);
	}
}


void CStandardLoginGuiComp::OnGuiDestroyed()
{
	m_loginObserver.UnregisterAllObjects();

	BaseClass::OnGuiDestroyed();
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CStandardLoginGuiComp::OnRestoreSettings(const QSettings& settings)
{
	QString lastUser = settings.value("LastUser").toString();
	bool isRememberMe = settings.value("RememberMe", false).toBool();

	if (isRememberMe){
		UserEdit->setText(lastUser);
	}

	RememberMe->setChecked(isRememberMe);

	if (!lastUser.isEmpty()){
		PasswordEdit->setFocus();
		PasswordEdit->setCursorPosition(0);
	}
}


void CStandardLoginGuiComp::OnSaveSettings(QSettings& settings) const
{
	bool isRememberMe = RememberMe->isChecked();
	QString lastUser = UserEdit->text();

	settings.setValue("RememberMe", isRememberMe);
	settings.setValue("LastUser", lastUser);
}


// private slots

void CStandardLoginGuiComp::on_LoginButton_clicked()
{
	PasswordEdit->setFocus();

	LoginLogText->clear();

	LoginLogText->setVisible(false);

	if (m_loginCompPtr.IsValid()){
		QString userName = UserEdit->text();
		QString password = PasswordEdit->text();
		if (m_loginCompPtr->Login(userName, password)){
			if (RememberMe->isChecked() == false){
				UserEdit->setText("");
			}
		}
		else{
			PasswordEdit->setStyleSheet("border-color: red; color: red");
			PasswordLabel->setStyleSheet("color: red");
			PasswordMessage->setStyleSheet("color: red");
			PasswordMessage->setText(tr("Login failed"));
		}
	}
}


void CStandardLoginGuiComp::on_PasswordEdit_textEdited(const QString&/*text*/)
{
	PasswordEdit->setStyleSheet("");
	PasswordLabel->setStyleSheet("");
	PasswordMessage->setText("");
	LoginLogText->clear();
	LoginLogText->setVisible(false);
}


// private methods

void CStandardLoginGuiComp::OnLoginUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/, const iauth::ILogin* /*objectPtr*/)
{
	UpdateLoginButtonsState();
}


void CStandardLoginGuiComp::UpdateLoginButtonsState()
{
	bool isLogged = true;
	if (m_loginCompPtr.IsValid()){
		isLogged = m_loginCompPtr->GetLoggedUser() != NULL;
	}

	UserEdit->setEnabled(!isLogged);
	PasswordEdit->setEnabled(!isLogged);
	UserEdit->setEnabled(!isLogged);
	LoginButton->setEnabled(!isLogged);
}


// public methods of embedded class LoginLog

CStandardLoginGuiComp::LoginLog::LoginLog(CStandardLoginGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (ilog::IMessageConsumer)

bool CStandardLoginGuiComp::LoginLog::IsMessageSupported(int /*messageCategory*/, int /*messageId*/, const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CStandardLoginGuiComp::LoginLog::AddMessage(const MessagePtr& messagePtr)
{
	Q_ASSERT(messagePtr.IsValid());

	if (messagePtr.IsValid()){
		QString loginMessage = messagePtr->GetInformationDescription();

		m_parent.LoginLogText->setVisible(!loginMessage.isEmpty());

		m_parent.LoginLogText->setPlainText(m_parent.LoginLogText->toPlainText() + "\n" + loginMessage);
	}
}


} // namespace imtauthgui


