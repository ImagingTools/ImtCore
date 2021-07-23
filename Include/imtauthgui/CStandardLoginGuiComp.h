#pragma once


// ACF includes
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>
#include <iqt/ISettingsProvider.h>
#include <iqtgui/IDialog.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <ilog/IMessageConsumer.h>

// ImtCore includes
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
		I_ASSIGN(m_settingsProviderCompPtr, "SettingsProvider", "Application settings provider", false, "SettingsProvider");
		I_ASSIGN(m_rightsCompPtr, "Rights", "User rights for the application", false, "Rights");
	I_END_COMPONENT;

	CStandardLoginGuiComp();
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiShown() override;
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (ibase::TDesignSchemaHandlerWrap)
	virtual void OnDesignSchemaChanged() override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

private Q_SLOTS:
	void on_LoginButton_clicked();
	void on_PasswordEdit_textEdited(const QString& text);

private:
	void OnLoginUpdate(const istd::IChangeable::ChangeSet& changeSet, const iauth::ILogin* objectPtr);
	void UpdateLoginButtonsState();

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


private:
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(iqt::ISettingsProvider, m_settingsProviderCompPtr);
	I_REF(iauth::IRightsProvider, m_rightsCompPtr);

	imtbase::TModelUpdateBinder<iauth::ILogin, CStandardLoginGuiComp> m_loginObserver;
	LoginLog m_loginLog;
};


} // namespace imtauthgui


