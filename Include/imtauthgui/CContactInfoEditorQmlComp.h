#pragma once


// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtauthgui/IDataController.h>
#include <imtqml/CQuickObjectComp.h>


namespace imtauthgui
{


/**
	QML-based editor for the contact/account info.
	\ingroup LicenseManagement
*/
class CContactInfoEditorQmlComp: public imtqml::CQuickObjectComp
{
public:
	typedef imtqml::CQuickObjectComp BaseClass;

	I_BEGIN_COMPONENT(CContactInfoEditorQmlComp);
		I_ASSIGN(m_dataControllerCompPtr, "DataControllerCompPtr", "reference to DataController", true, "DataController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtqml::CQuickObjectComp)
	virtual bool CreateItem(QQmlEngine* enginePtr) override;

private:
	I_REF(imtauthgui::IDataController, m_dataControllerCompPtr);
};


} // namespace imtauthgui


