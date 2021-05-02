#pragma once


// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtauthgui/IDataController.h>
#include <imtqml/CQuickObjectCompBase.h>


namespace imtauthgui
{


/**
	QML-based editor for the contact/account info.
	\ingroup LicenseManagement
*/
class CContactInfoEditorQmlComp: public imtqml::CQuickObjectCompBase
{
public:
	typedef imtqml::CQuickObjectCompBase BaseClass;

	I_BEGIN_COMPONENT(CContactInfoEditorQmlComp);
		I_ASSIGN(m_dataControllerCompPtr, "DataControllerCompPtr", "reference to DataController", true, "DataController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtqml::CQuickObjectCompBase)
	virtual void OnItemCreated(QQuickItem& item) override;

private:
	I_REF(imtauthgui::IDataController, m_dataControllerCompPtr);
};


} // namespace imtauthgui


