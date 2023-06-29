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

// ImtCore includes
#include <imtauth/ISuperuserController.h>
#include <imtauth/ISuperuserProvider.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/ISystemStatus.h>
#include <GeneratedFiles/imtauthgui/ui_CRemoteStandardLoginGuiComp.h>


namespace imtauthgui
{


class CRemoteStandardLoginGuiComp: public iqtgui::TRestorableGuiWrap<
			iqtgui::TDesignerGuiCompBase<Ui::CRemoteStandardLoginGuiComp>>
{
	Q_OBJECT
public:
	typedef iqtgui::TRestorableGuiWrap<
				iqtgui::TDesignerGuiCompBase<Ui::CRemoteStandardLoginGuiComp>> BaseClass;

	I_BEGIN_COMPONENT(CRemoteStandardLoginGuiComp);
		I_REGISTER_SUBELEMENT(LoginLog);
		I_REGISTER_SUBELEMENT_INTERFACE(LoginLog, ilog::IMessageConsumer, ExtractLoginLog);
		I_ASSIGN(m_loginCompPtr, "Login", "Login", false, "Login");
		I_ASSIGN(m_superuserProviderCompPtr, "SuperuserProvider", "Superuser provider", false, "SuperuserProvider");
		I_ASSIGN(m_settingsProviderCompPtr, "SettingsProvider", "Application settings provider", false, "SettingsProvider");
		I_ASSIGN(m_superuserControllerCompPtr, "SuperuserController", "Superuser controller", false, "SuperuserController");
		I_ASSIGN(m_systemStatusCompPtr, "SystemStatus", "System status", true, "SystemStatus");
	I_END_COMPONENT;

	CRemoteStandardLoginGuiComp();
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
	void on_RefreshButton_clicked();
	void on_PasswordEdit_textEdited(const QString& text);
	void on_SuPasswordEdit_textEdited(const QString& text);
	void on_SuConfirmPasswordEdit_textEdited(const QString& text);
	void OnSetSuPasswordFinished();

private:
	void OnLoginUpdate(const istd::IChangeable::ChangeSet& changeSet, const iauth::ILogin* objectPtr);
	void OnSystemStatusUpdate(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISystemStatus* objectPtr);
	void UpdateLoginButtonsState();
	void CheckMatchingPassword();

private:
	class LoginLog: public ilog::IMessageConsumer
	{
	public:
		LoginLog(CRemoteStandardLoginGuiComp& parent);

		// reimplemented (ilog::IMessageConsumer)
		virtual bool IsMessageSupported(int messageCategory = -1, int messageId = -1, const istd::IInformationProvider* messagePtr = NULL) const override;
		virtual void AddMessage(const MessagePtr& messagePtr) override;

	private:
		CRemoteStandardLoginGuiComp& m_parent;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractLoginLog(CRemoteStandardLoginGuiComp& parent)
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

		SetSuPasswordThread(CRemoteStandardLoginGuiComp& parent);

		void Start(const QByteArray& suPassword);
		ThreadState GetState();

	protected:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CRemoteStandardLoginGuiComp& m_parent;
		QByteArray m_suPassword;
		ThreadState m_state;
	};

private:
	I_REF(imtauth::ISuperuserProvider, m_superuserProviderCompPtr);
	I_REF(imtauth::ISuperuserController, m_superuserControllerCompPtr);
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(iqt::ISettingsProvider, m_settingsProviderCompPtr);
	I_REF(imtbase::ISystemStatus, m_systemStatusCompPtr);

	imtbase::TModelUpdateBinder<iauth::ILogin, CRemoteStandardLoginGuiComp> m_loginObserver;
	imtbase::TModelUpdateBinder<imtbase::ISystemStatus, CRemoteStandardLoginGuiComp> m_systemStatusObserver;

	LoginLog m_loginLog;
	SetSuPasswordThread m_setSuPasswordThread;
};


} // namespace imtauthgui


