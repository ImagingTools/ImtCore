#pragma once


// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
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
		I_ASSIGN(m_dataProviderCompPtr, "DataControllerCompPtr", "reference to DataController", true, "DataController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtqml::CQuickObjectCompBase)
	virtual void OnItemCreated() override;

private:
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_dataProviderCompPtr);
};


} // namespace imtauthgui


