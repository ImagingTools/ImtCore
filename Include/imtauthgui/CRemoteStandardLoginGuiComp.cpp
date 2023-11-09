#include <imtauthgui/CRemoteStandardLoginGuiComp.h>


// Qt includes
#include <QtGui/QKeyEvent>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QInputDialog>
#else
#include <QtGui/QInputDialog>
#endif
#include <QtWidgets/QMessageBox>
#include <QtGui/QMovie>


namespace imtauthgui
{


// public methods

CRemoteStandardLoginGuiComp::CRemoteStandardLoginGuiComp()
	:m_loginObserver(*this),
	m_systemStatusObserver(*this),
	m_loginLog(*this),
	m_setSuPasswordThread(*this)
{
}


bool CRemoteStandardLoginGuiComp::eventFilter(QObject* watched, QEvent* event)
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


// reimplemented (icomp::CComponentBase)

void CRemoteStandardLoginGuiComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	connect(&m_setSuPasswordThread, &QThread::finished, this, &CRemoteStandardLoginGuiComp::OnSetSuPasswordFinished, Qt::QueuedConnection);
}


void CRemoteStandardLoginGuiComp::OnComponentDestroyed()
{
	if (m_setSuPasswordThread.isRunning()){
		m_setSuPasswordThread.requestInterruption();
		m_setSuPasswordThread.wait();
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CRemoteStandardLoginGuiComp::OnGuiShown()
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
	RefreshButton->setVisible(false);

	on_SuPasswordEdit_textEdited("");

	QMovie* movie = new QMovie(":/Animation/Loading");
	movie->setScaledSize(QSize(50, 50));

	LoadingLabel->setMovie(movie);

	LoadingLabel->movie()->start();
	LoadingLabel->show();
}


void CRemoteStandardLoginGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (m_loginCompPtr.IsValid()){
		m_loginObserver.RegisterObject(m_loginCompPtr.GetPtr(), &CRemoteStandardLoginGuiComp::OnLoginUpdate);
	}

	if (m_systemStatusCompPtr.IsValid()){
		m_systemStatusObserver.RegisterObject(m_systemStatusCompPtr.GetPtr(), &CRemoteStandardLoginGuiComp::OnSystemStatusUpdate);

		StackedWidget->setCurrentIndex(3);
	}
	else{
		StackedWidget->setCurrentIndex(0);
	}

}


void CRemoteStandardLoginGuiComp::OnGuiDestroyed()
{
	m_loginObserver.UnregisterAllObjects();
	m_systemStatusObserver.UnregisterAllObjects();

	BaseClass::OnGuiDestroyed();
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CRemoteStandardLoginGuiComp::OnRestoreSettings(const QSettings& settings)
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


void CRemoteStandardLoginGuiComp::OnSaveSettings(QSettings& settings) const
{
	bool isRememberMe = RememberMe->isChecked();
	QString lastUser = UserEdit->text();

	settings.setValue("RememberMe", isRememberMe);
	settings.setValue("LastUser", lastUser);
}


// private slots

void CRemoteStandardLoginGuiComp::on_LoginButton_clicked()
{
	PasswordEdit->setFocus();

	QString userName = UserEdit->text();
	QString password = PasswordEdit->text();

	if (m_loginCompPtr.IsValid()){
		StackedWidget->setCurrentIndex(3);

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

			m_systemStatusCompPtr->UpdateSystemStatus();
		}

		StackedWidget->setCurrentIndex(0);
	}
}


void CRemoteStandardLoginGuiComp::on_SetPasswordButton_clicked()
{
	if (m_superuserControllerCompPtr.IsValid()){
		StackedWidget->setCurrentIndex(3);

		QString password = SuPasswordEdit->text();

		m_setSuPasswordThread.Start(password.toUtf8());
	}
}


void CRemoteStandardLoginGuiComp::on_RefreshButton_clicked()
{
	if (m_systemStatusCompPtr.IsValid()){
		StackedWidget->setCurrentIndex(3);
	}
}


void CRemoteStandardLoginGuiComp::on_PasswordEdit_textEdited(const QString&/*text*/)
{
	PasswordEdit->setStyleSheet("");
	PasswordLabel->setStyleSheet("");
	PasswordMessage->setText("");
}


void CRemoteStandardLoginGuiComp::on_SuPasswordEdit_textEdited(const QString& text)
{
	SuPasswordEdit->setStyleSheet("");
	SuPasswordMessage->setText("");

	CheckMatchingPassword();

	if (text.isEmpty()){
		SuPasswordMessage->setStyleSheet("color: red");
		SuPasswordMessage->setText(tr("Please enter a non-empty password"));
	}

	SetPasswordButton->setEnabled(!text.isEmpty());
}


void CRemoteStandardLoginGuiComp::on_SuConfirmPasswordEdit_textEdited(const QString& /*text*/)
{
	CheckMatchingPassword();
}


void CRemoteStandardLoginGuiComp::OnSetSuPasswordFinished()
{
	CRemoteStandardLoginGuiComp::SetSuPasswordThread::ThreadState state = m_setSuPasswordThread.GetState();
	if (state == CRemoteStandardLoginGuiComp::SetSuPasswordThread::ThreadState::TS_OK){
		StackedWidget->setCurrentIndex(0);
	}
	else if (state == CRemoteStandardLoginGuiComp::SetSuPasswordThread::ThreadState::TS_FAILED){
		StackedWidget->setCurrentIndex(1);

		QMessageBox::critical(GetWidget(), tr("User Management"), tr("Password for the super user could not be set"), QMessageBox::Close);
	}
}


// private methods

void CRemoteStandardLoginGuiComp::OnLoginUpdate(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iauth::ILogin* /*objectPtr*/)
{
	UpdateLoginButtonsState();
}


void CRemoteStandardLoginGuiComp::OnSystemStatusUpdate(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::ISystemStatus* objectPtr)
{
	QString error;
	imtbase::ISystemStatus::SystemStatus status = objectPtr->GetSystemStatus(error);
	if (status == imtbase::ISystemStatus::SystemStatus::SS_NO_ERROR){
		if (m_superuserProviderCompPtr.IsValid()){
			QString errorMessage;
			bool superuserExists = m_superuserProviderCompPtr->SuperuserExists(errorMessage);
			if (superuserExists){
				StackedWidget->setCurrentIndex(0);
			}
			else{
				StackedWidget->setCurrentIndex(1);
			}
		}
	}
	else{
		NoConnection->setText(error);

		StackedWidget->setCurrentIndex(2);
	}
}


void CRemoteStandardLoginGuiComp::UpdateLoginButtonsState()
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


void CRemoteStandardLoginGuiComp::CheckMatchingPassword()
{
	QString password = SuPasswordEdit->text();
	QString confirmPassword = SuConfirmPasswordEdit->text();

	bool isEqual = password == confirmPassword;
	if (!isEqual){
		SuPasswordMessage->setStyleSheet("color: red");
		SuPasswordMessage->setText(tr("Password doesn't match"));
	}
	else{
		SuPasswordMessage->setStyleSheet("");
		SuPasswordMessage->setText("");
	}

	SetPasswordButton->setEnabled(isEqual);
}


// public methods of embedded class LoginLog

CRemoteStandardLoginGuiComp::LoginLog::LoginLog(CRemoteStandardLoginGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (ilog::IMessageConsumer)

bool CRemoteStandardLoginGuiComp::LoginLog::IsMessageSupported(int /*messageCategory*/, int /*messageId*/, const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CRemoteStandardLoginGuiComp::LoginLog::AddMessage(const MessagePtr& messagePtr)
{
	Q_ASSERT(messagePtr.IsValid());

	if (messagePtr.IsValid()){
		QString loginMessage = messagePtr->GetInformationDescription();
	}
}


// public methods of the embedded class SetSuPasswordThread

CRemoteStandardLoginGuiComp::SetSuPasswordThread::SetSuPasswordThread(CRemoteStandardLoginGuiComp& parent)
	:m_parent(parent),
	m_state(TS_UNKNOWN)
{
}


void CRemoteStandardLoginGuiComp::SetSuPasswordThread::Start(const QByteArray& suPassword)
{
	m_suPassword = suPassword;

	QThread::start();
}


CRemoteStandardLoginGuiComp::SetSuPasswordThread::ThreadState CRemoteStandardLoginGuiComp::SetSuPasswordThread::GetState()
{
	return m_state;
}


// protected methods of the embedded class SetSuPasswordThread

// reimplemented (QThread)

void CRemoteStandardLoginGuiComp::SetSuPasswordThread::run()
{
	if (m_parent.m_superuserControllerCompPtr.IsValid()){
		if (m_parent.m_superuserControllerCompPtr->SetSuperuserPassword(m_suPassword)){
			m_state = TS_OK;
		}
		else{
			m_state = TS_FAILED;
		}
	}
}


} // namespace imtauthgui


