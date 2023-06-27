#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>
#include <iqt/ISettingsProvider.h>
#include <iqtgui/IDialog.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <ilog/IMessageConsumer.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <imtcom/IConnectionStatusProvider.h>

// ImtCore includes
#include <imtauth/ISuperuserController.h>
#include <imtauth/ISuperuserProvider.h>
#include <imtbase/TModelUpdateBinder.h>
#include <GeneratedFiles/imtauthgui/ui_CStandardLoginGuiComp.h>


namespace imtauthgui
{


class CStandardLoginGuiComp: public iqtgui::TRestorableGuiWrap<
			iqtgui::TDesignerGuiCompBase<Ui::CStandardLoginGuiComp>>
{
	Q_OBJECT
public:
	typedef iqtgui::TRestorableGuiWrap<
				iqtgui::TDesignerGuiCompBase<Ui::CStandardLoginGuiComp>> BaseClass;

	I_BEGIN_COMPONENT(CStandardLoginGuiComp);
		I_REGISTER_SUBELEMENT(LoginLog);
		I_REGISTER_SUBELEMENT_INTERFACE(LoginLog, ilog::IMessageConsumer, ExtractLoginLog);
		I_ASSIGN(m_loginCompPtr, "Login", "Login", false, "Login");
		I_ASSIGN(m_superuserProviderCompPtr, "SuperuserProvider", "Superuser provider", false, "SuperuserProvider");
		I_ASSIGN(m_settingsProviderCompPtr, "SettingsProvider", "Application settings provider", false, "SettingsProvider");
		I_ASSIGN(m_superuserControllerCompPtr, "SuperuserController", "Superuser controller", false, "SuperuserController");
		I_ASSIGN(m_connectionStatusProviderCompPtr, "ConnectionStatusProvider", "Connection status provider", false, "ConnectionStatusProvider");
	I_END_COMPONENT;

	CStandardLoginGuiComp();
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiShown() override;
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

private Q_SLOTS:
	void on_LoginButton_clicked();
	void on_SetPasswordButton_clicked();
	void on_PasswordEdit_textEdited(const QString& text);
	void on_SuPasswordEdit_textEdited(const QString& text);
	void on_SuConfirmPasswordEdit_textEdited(const QString& text);
	void OnSetSuPasswordFinished();

private:
	void OnLoginUpdate(const istd::IChangeable::ChangeSet& changeSet, const iauth::ILogin* objectPtr);
	void OnConnectionStatusChanged(const istd::IChangeable::ChangeSet& changeSet, const imtcom::IConnectionStatusProvider* objectPtr);
	void UpdateLoginButtonsState();
	void CheckMatchingPassword();
	void ShowLoadingPage();

private:
	class LoginLog: public ilog::IMessageConsumer
	{
	public:
		LoginLog(CStandardLoginGuiComp& parent);

		// reimplemented (ilog::IMessageConsumer)
		virtual bool IsMessageSupported(int messageCategory = -1, int messageId = -1, const istd::IInformationProvider* messagePtr = NULL) const override;
		virtual void AddMessage(const MessagePtr& messagePtr) override;

	private:
		CStandardLoginGuiComp& m_parent;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractLoginLog(CStandardLoginGuiComp& parent)
	{
		return &parent.m_loginLog;
	}

	class SetSuPasswordThread: public QThread
	{
	public:
		enum ThreadState
		{
			TS_UNKNOWN = 0,
			TS_OK,
			TS_FAILED
		};

		SetSuPasswordThread(CStandardLoginGuiComp& parent);

		void Start(const QByteArray& suPassword);
		ThreadState GetState();

	protected:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CStandardLoginGuiComp& m_parent;
		QByteArray m_suPassword;
		ThreadState m_state;
	};

private:
	I_REF(imtauth::ISuperuserProvider, m_superuserProviderCompPtr);
	I_REF(imtauth::ISuperuserController, m_superuserControllerCompPtr);
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(iqt::ISettingsProvider, m_settingsProviderCompPtr);
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusProviderCompPtr);

	imtbase::TModelUpdateBinder<iauth::ILogin, CStandardLoginGuiComp> m_loginObserver;
	imtbase::TModelUpdateBinder<imtcom::IConnectionStatusProvider, CStandardLoginGuiComp> m_connectionObserver;

	LoginLog m_loginLog;
	SetSuPasswordThread m_setSuPasswordThread;
};


} // namespace imtauthgui


