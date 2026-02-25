// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	m_loginStatusProviderObserver(*this),
	m_pumaLoginStatusProviderObserver(*this),
	m_loginLog(*this),
	m_setSuPasswordThread(*this)
{
}


bool CRemoteStandardLoginGuiComp::eventFilter(QObject* watched, QEvent* event)
{
	if (m_loginCompPtr.IsValid()){
		bool isLogged = m_loginCompPtr->GetLoggedUser() != NULL;

		auto keyEventPtr = dynamic_cast<QKeyEvent*>(event);
		if (keyEventPtr != nullptr){
			int pressedKey = keyEventPtr->key();

			if (pressedKey == Qt::Key_Return || pressedKey == Qt::Key_Enter){
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

	if (LoadingLabel->movie() == nullptr) {
		constexpr int scaledSize = 50;
		auto *movie = new QMovie(":/Animation/Loading", "", LoadingLabel);
		movie->setScaledSize(QSize(scaledSize, scaledSize));
		LoadingLabel->setMovie(movie);
	}

	LoadingLabel->movie()->start();
	LoadingLabel->show();
}


void CRemoteStandardLoginGuiComp::OnGuiHidden()
{
	if (LoadingLabel->movie() != nullptr){
		LoadingLabel->movie()->stop();
	}

	BaseClass::OnGuiHidden();
}


void CRemoteStandardLoginGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (m_loginCompPtr.IsValid()){
		m_loginObserver.RegisterObject(m_loginCompPtr.GetPtr(), &CRemoteStandardLoginGuiComp::OnLoginUpdate);
	}

	if (m_pumaLoginStatusProviderCompPtr.IsValid()){
		m_pumaLoginStatusProviderObserver.RegisterObject(m_pumaLoginStatusProviderCompPtr.GetPtr(), &CRemoteStandardLoginGuiComp::OnPumaConnectionStatusUpdate);
	}

	if (m_loginStatusProviderCompPtr.IsValid()){
		m_loginStatusProviderObserver.RegisterObject(m_loginStatusProviderCompPtr.GetPtr(), &CRemoteStandardLoginGuiComp::OnConnectionStatusUpdate);

		StackedWidget->setCurrentIndex(US_WAIT_INDICATOR);
	}
	else{
		OnConnectionStatusUpdate(istd::IChangeable::GetAnyChange(), nullptr);
	}
}


void CRemoteStandardLoginGuiComp::OnGuiDestroyed()
{
	if (LoadingLabel->movie() != nullptr) {
		LoadingLabel->movie()->stop();
	}

	m_loginObserver.UnregisterAllObjects();
	m_loginStatusProviderObserver.UnregisterAllObjects();
	m_pumaLoginStatusProviderObserver.UnregisterAllObjects();

	BaseClass::OnGuiDestroyed();
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CRemoteStandardLoginGuiComp::OnRestoreSettings(const QSettings& settings)
{
	QString lastUser = settings.value("LastUser").toString();
	bool isRememberMe = settings.value("RememberMe", false).toBool();
	QByteArray refreshToken = settings.value("RefreshToken").toByteArray();

	if (isRememberMe){
		UserEdit->setText(lastUser);
		
		// Try to login with refresh token if available
		if (TryRestoreSessionWithRefreshToken(lastUser, refreshToken)){
			// Successfully logged in with refresh token
			return;
		}
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
	
	// Save refresh token only if "Remember me" is checked
	SaveRefreshTokenIfRememberMe(settings);
	
	// Clear refresh token if "Remember me" is not checked
	ClearRefreshTokenIfNeeded(settings);
}


// private slots

void CRemoteStandardLoginGuiComp::on_LoginButton_clicked()
{
	PasswordEdit->setFocus();

	QString userName = UserEdit->text();
	QString password = PasswordEdit->text();

	if (m_loginCompPtr.IsValid()){
		StackedWidget->setCurrentIndex(US_WAIT_INDICATOR);

		if (m_loginCompPtr->Login(userName, password)){
			if (!RememberMe->isChecked()){
				UserEdit->setText("");
			}
		}
		else{
			PasswordEdit->setStyleSheet("border-color: red; color: red");
			PasswordLabel->setStyleSheet("color: red");
			PasswordMessage->setStyleSheet("color: red");
			PasswordMessage->setText(tr("Login failed"));
		}

		StackedWidget->setCurrentIndex(US_USER_PASSWORD_LOGIN);
	}
}


void CRemoteStandardLoginGuiComp::on_SetPasswordButton_clicked()
{
	if (m_superuserControllerCompPtr.IsValid()){
		StackedWidget->setCurrentIndex(US_WAIT_INDICATOR);

		QString password = SuPasswordEdit->text();

		m_setSuPasswordThread.Start(password.toUtf8());
	}
}


void CRemoteStandardLoginGuiComp::on_RefreshButton_clicked()
{
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
	SetSuPasswordThread::ThreadState state = m_setSuPasswordThread.GetState();
	if (state != SetSuPasswordThread::ThreadState::TS_OK){
		StackedWidget->setCurrentIndex(US_USER_PASSWORD_LOGIN);
	}
	else if (state == SetSuPasswordThread::ThreadState::TS_FAILED){
		StackedWidget->setCurrentIndex(US_ENTER_SU_PASSWORD);

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


void CRemoteStandardLoginGuiComp::OnConnectionStatusUpdate(
			const istd::IChangeable::ChangeSet& changeSet,
			const imtcom::IConnectionStatusProvider* objectPtr)
{
	imtcom::IConnectionStatusProvider::ConnectionStatus loginStatus =
		(objectPtr != nullptr) ? objectPtr->GetConnectionStatus() : imtcom::IConnectionStatusProvider::CS_CONNECTED;

	switch (loginStatus){
	case imtcom::IConnectionStatusProvider::CS_UNKNOWN:
	case imtcom::IConnectionStatusProvider::CS_DISCONNECTED:
		NoConnection->setText(tr("No connection to the server"));
		StackedWidget->setCurrentIndex(US_NO_CONNECTION_TO_SERVER);
		break;
	case imtcom::IConnectionStatusProvider::CS_CONNECTED:
		if (m_pumaLoginStatusProviderCompPtr.IsValid()){
			int pumaLoginStatus = m_pumaLoginStatusProviderCompPtr->GetConnectionStatus();
			if (pumaLoginStatus != imtcom::IConnectionStatusProvider::CS_CONNECTED){
				OnPumaConnectionStatusUpdate(changeSet, m_pumaLoginStatusProviderCompPtr.GetPtr());
				return;
			}
		}

		if (m_superuserProviderCompPtr.IsValid()){
			QString errorMessage;
			imtauth::ISuperuserProvider::ExistsStatus status = m_superuserProviderCompPtr->SuperuserExists(errorMessage);
			if (status == imtauth::ISuperuserProvider::ES_EXISTS){
				StackedWidget->setCurrentIndex(US_USER_PASSWORD_LOGIN);
			}
			else if (status == imtauth::ISuperuserProvider::ES_NOT_EXISTS){
				StackedWidget->setCurrentIndex(US_ENTER_SU_PASSWORD);
			}
			else{
				NoConnection->setText(errorMessage);
				StackedWidget->setCurrentIndex(US_NO_CONNECTION_TO_SERVER);
			}
		}
		break;
	}
}


void CRemoteStandardLoginGuiComp::OnPumaConnectionStatusUpdate(
			const istd::IChangeable::ChangeSet& changeSet,
			const imtcom::IConnectionStatusProvider* objectPtr)
{
	if (objectPtr != nullptr){
		switch (objectPtr->GetConnectionStatus()){
		case imtcom::IConnectionStatusProvider::CS_CONNECTED:
			OnConnectionStatusUpdate(changeSet, m_loginStatusProviderCompPtr.GetPtr());
			break;
		case imtcom::IConnectionStatusProvider::CS_UNKNOWN:
		case imtcom::IConnectionStatusProvider::CS_DISCONNECTED:
			NoConnection->setText(tr("No connection to the authorization server"));
			StackedWidget->setCurrentIndex(US_NO_CONNECTION_TO_SERVER);
			break;
		}
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


bool CRemoteStandardLoginGuiComp::TryRestoreSessionWithRefreshToken(const QString& /*userName*/, const QByteArray& /*refreshToken*/)
{
	return false;
}


void CRemoteStandardLoginGuiComp::SaveRefreshTokenIfRememberMe(QSettings& settings) const
{
}


void CRemoteStandardLoginGuiComp::ClearRefreshTokenIfNeeded(QSettings& settings) const
{
	bool isRememberMe = RememberMe->isChecked();
	
	if (!isRememberMe){
		settings.remove("RefreshToken");
	}
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


