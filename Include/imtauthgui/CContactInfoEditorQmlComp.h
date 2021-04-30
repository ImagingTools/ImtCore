#pragma once


// Qt includes
#include <QtWidgets/QAction>
#include <QtCore/QJsonDocument>


// ACF includes
//#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/IGuiObject.h>
#include <imod/TSingleModelObserverBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
//#include <GeneratedFiles/imtauthgui/ui_CContactInfoEditorComp.h>
#include <imtauth/IContactInfo.h>
#include <imtauthgui/CTreeItemModel.h>
#include <imtauthgui/IDataController.h>
#include <imtqml/CQuickObjectComp.h>


namespace imtauthgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CContactInfoEditorQmlComp:
		public imtqml::CQuickObjectComp
//		public imtqml::CQuickObjectComp, public iqtgui::IGuiObject, public imod::TSingleModelObserverBase<imtauth::IContactInfo>
//			public iqtgui::TDesignerGuiObserverCompBase<
//						Ui::CContactInfoEditorComp, imtauth::IContactInfo>
{
//	Q_OBJECT
public:
//	typedef imod::TSingleModelObserverBase<imtauth::IContactInfo> BaseClass;
	typedef imtqml::CQuickObjectComp BaseClass;

	I_BEGIN_COMPONENT(CContactInfoEditorQmlComp);
//		I_REGISTER_INTERFACE(imod::IObserver);
//		I_ASSIGN(m_guiCompPtr, "GuiCompPtr", "reference to GuiQuickWrap", true, "GuiQuickWrap");
		I_ASSIGN(m_dataControllerCompPtr, "DataControllerCompPtr", "reference to DataController", true, "DataController");
	I_END_COMPONENT;

protected:

	// reimplemented (imtqml::CQuickObjectComp)
	virtual bool CreateItem(QQmlEngine* enginePtr) override;

private:

//	I_REF(iqtgui::IGuiObject, m_guiCompPtr);
	I_REF(imtauthgui::IDataController, m_dataControllerCompPtr);
};


} // namespace imtauthgui


