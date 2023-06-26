#include <imtauthgui/CStandardLoginGuiComp.h>


// Qt includes
#include <QtGui/QKeyEvent>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QInputDialog>
#else
#include <QtGui/QInputDialog>
#endif
#include <QtWidgets/QMessageBox>



namespace imtauthgui
{


// public methods

CStandardLoginGuiComp::CStandardLoginGuiComp()
	:m_loginObserver(*this),
	m_connectionObserver(*this),
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

	LoginMessage->setStyleSheet("font-weight: bold");
	LoginMessage_2->setStyleSheet("font-weight: bold");
	NoConnection->setStyleSheet("font-weight: bold; color: red");

	SetPasswordButton->setEnabled(false);

	on_SuPasswordEdit_textEdited("");
}


void CStandardLoginGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (m_loginCompPtr.IsValid()){
		m_loginObserver.RegisterObject(m_loginCompPtr.GetPtr(), &CStandardLoginGuiComp::OnLoginUpdate);
	}

	if (m_connectionStatusProviderCompPtr.IsValid()){
		m_connectionObserver.RegisterObject(m_connectionStatusProviderCompPtr.GetPtr(), &CStandardLoginGuiComp::OnConnectionStatusChanged);
	}

	StackedWidget->setCurrentIndex(0);
}


void CStandardLoginGuiComp::OnGuiDestroyed()
{
	m_loginObserver.UnregisterAllObjects();
	m_connectionObserver.UnregisterAllObjects();

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


void CStandardLoginGuiComp::on_SetPasswordButton_clicked()
{
	if (m_superuserControllerCompPtr.IsValid()){
		QString password = SuPasswordEdit->text();
		if (password.isEmpty()){
			QMessageBox::critical(GetWidget(), tr("User Management"), tr("Password for the super user could not be empty"), QMessageBox::Close);

			return;
		}

		if (m_superuserControllerCompPtr->SetSuperuserPassword(password.toUtf8())){
			StackedWidget->setCurrentIndex(0);
		}
		else{
			QMessageBox::critical(GetWidget(), tr("User Management"), tr("Password for the super user could not be set"), QMessageBox::Close);
		}
	}
}


void CStandardLoginGuiComp::on_PasswordEdit_textEdited(const QString&/*text*/)
{
	PasswordEdit->setStyleSheet("");
	PasswordLabel->setStyleSheet("");
	PasswordMessage->setText("");
}


void CStandardLoginGuiComp::on_SuPasswordEdit_textEdited(const QString& text)
{
	SuPasswordEdit->setStyleSheet("");
	SuPasswordMessage->setText("");

	if (text.isEmpty()){
		SuPasswordMessage->setStyleSheet("color: red");
		SuPasswordMessage->setText(tr("Please enter a non-empty password"));
	}

	SetPasswordButton->setEnabled(!text.isEmpty());
}


// private methods

void CStandardLoginGuiComp::OnLoginUpdate(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iauth::ILogin* /*objectPtr*/)
{
	UpdateLoginButtonsState();
}


void CStandardLoginGuiComp::OnConnectionStatusChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtcom::IConnectionStatusProvider* objectPtr)
{
	if (objectPtr != nullptr){
		imtcom::IConnectionStatusProvider::ConnectionStatus connectionStatus = objectPtr->GetConnectionStatus();
		if (connectionStatus == imtcom::IConnectionStatusProvider::CS_CONNECTED){
			if (m_superuserProviderCompPtr.IsValid()){
				bool superuserExists = m_superuserProviderCompPtr->SuperuserExists();
				if (!superuserExists){
					StackedWidget->setCurrentIndex(1);
				}
			}
		}
		else if (connectionStatus == imtcom::IConnectionStatusProvider::CS_DISCONNECTED){
			StackedWidget->setCurrentIndex(2);
		}
	}
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
	}
}


} // namespace imtauthgui


