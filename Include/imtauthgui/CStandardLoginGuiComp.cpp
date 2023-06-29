#include <imtauthgui/CStandardLoginGuiComp.h>


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

CStandardLoginGuiComp::CStandardLoginGuiComp()
	:m_loginObserver(*this),
	m_connectionObserver(*this),
	m_loginLog(*this),
	m_setSuPasswordThread(*this)
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


// reimplemented (icomp::CComponentBase)

void CStandardLoginGuiComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	connect(&m_setSuPasswordThread, &QThread::finished, this, &CStandardLoginGuiComp::OnSetSuPasswordFinished, Qt::QueuedConnection);
}


void CStandardLoginGuiComp::OnComponentDestroyed()
{
	if (m_setSuPasswordThread.isRunning()){
		m_setSuPasswordThread.requestInterruption();
		m_setSuPasswordThread.wait();
	}

	BaseClass::OnComponentDestroyed();
}


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

	QMovie* movie = new QMovie(":/Animation/Loading");
	movie->setScaledSize(QSize(50, 50));

	LoadingLabel->setMovie(movie);

	LoadingLabel->movie()->start();
	LoadingLabel->show();
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

	StackedWidget->setCurrentIndex(3);
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
		StackedWidget->setCurrentIndex(3);

		QString password = SuPasswordEdit->text();

		m_setSuPasswordThread.Start(password.toUtf8());
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

	CheckMatchingPassword();

	if (text.isEmpty()){
		SuPasswordMessage->setStyleSheet("color: red");
		SuPasswordMessage->setText(tr("Please enter a non-empty password"));
	}

	SetPasswordButton->setEnabled(!text.isEmpty());
}


void CStandardLoginGuiComp::on_SuConfirmPasswordEdit_textEdited(const QString& text)
{
	CheckMatchingPassword();
}


void CStandardLoginGuiComp::OnSetSuPasswordFinished()
{
	CStandardLoginGuiComp::SetSuPasswordThread::ThreadState state = m_setSuPasswordThread.GetState();
	if (state == CStandardLoginGuiComp::SetSuPasswordThread::ThreadState::TS_OK){
		StackedWidget->setCurrentIndex(0);
	}
	else if (state == CStandardLoginGuiComp::SetSuPasswordThread::ThreadState::TS_FAILED){
		StackedWidget->setCurrentIndex(1);

		QMessageBox::critical(GetWidget(), tr("User Management"), tr("Password for the super user could not be set"), QMessageBox::Close);
	}
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
				if (superuserExists){
					StackedWidget->setCurrentIndex(0);
				}
				else{
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


void CStandardLoginGuiComp::CheckMatchingPassword()
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


void CStandardLoginGuiComp::ShowLoadingPage()
{
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


// public methods of the embedded class SetSuPasswordThread

CStandardLoginGuiComp::SetSuPasswordThread::SetSuPasswordThread(CStandardLoginGuiComp& parent)
	:m_parent(parent),
	  m_state(TS_UNKNOWN)
{
}


void CStandardLoginGuiComp::SetSuPasswordThread::Start(const QByteArray& suPassword)
{
	m_suPassword = suPassword;

	QThread::start();
}


CStandardLoginGuiComp::SetSuPasswordThread::ThreadState CStandardLoginGuiComp::SetSuPasswordThread::GetState()
{
	return m_state;
}


// protected methods of the embedded class SetSuPasswordThread

// reimplemented (QThread)

void CStandardLoginGuiComp::SetSuPasswordThread::run()
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


