#pragma once


// ACF includes
#include <iauth/ILogin.h>
#include <iqtgui/IDialog.h>
#include <iqtgui/TDesignerGuiCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <GeneratedFiles/imtauthgui/ui_CStandardAccountMenuComp.h>


namespace imtauthgui
{


class CStandardAccountMenuComp: public iqtgui::TDesignerGuiCompBase<Ui::CStandardAccountMenuComp>
{
public:
	typedef iqtgui::TDesignerGuiCompBase<Ui::CStandardAccountMenuComp> BaseClass;

	I_BEGIN_COMPONENT(CStandardAccountMenuComp);
		I_ASSIGN(m_accountPreviewAttrPtr, "AccountIcon", "Account icon", true, ":/Icons/Account");
		I_ASSIGN(m_preferencesIconAttrPtr, "PreferencesIcon", "Preferences icon", true, ":/Icons/Settings");
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Size of icons", true, 32);
		I_ASSIGN(m_loginCompPtr, "Login", "Login", false, "Login");
		I_ASSIGN(m_preferencesDialogCompPtr, "PreferencesDialog", "Preferences dialog", false, "Preferences");
	I_END_COMPONENT;

	CStandardAccountMenuComp();
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (ibase::TDesignSchemaHandlerWrap)
	virtual void OnDesignSchemaChanged();

private:
	void OnLoginUpdate(const istd::IChangeable::ChangeSet& changeSet, const iauth::ILogin* objectPtr);

private:
	I_ATTR(QByteArray, m_accountPreviewAttrPtr);
	I_ATTR(QByteArray, m_preferencesIconAttrPtr);
	I_ATTR(int, m_iconSizeAttrPtr);
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(iqtgui::IDialog, m_preferencesDialogCompPtr);

	imtbase::TModelUpdateBinder<iauth::ILogin, CStandardAccountMenuComp> m_loginObserver;
};


} // namespace imtauthgui


