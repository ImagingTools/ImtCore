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
#include <imtauthgui/IDataController.h>
#include <imtauthgui/CTreeItemModel.h>


namespace imtauthgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CContactInfoEditorDataController :
		public IDataController,
		public imod::TSingleModelObserverBase<imtauth::IContactInfo>
{
public:
	typedef imod::TSingleModelObserverBase<imtauth::IContactInfo> BaseClass;

//	I_BEGIN_COMPONENT(CContactInfoEditorDataControllerComp);
//		I_REGISTER_INTERFACE(imod::IObserver);
//		I_ASSIGN(m_guiCompPtr, "GuiCompPtr", "reference to GuiQuickWrap", true, "GuiQuickWrap");
//	I_END_COMPONENT;

	// reimplemented (imtauthgui::IDataController)
	bool GetJsonData(QByteArray& jsonData, const QString& typeData = "") override;
	CTreeItemModel* GetTreeItemModel(const QString& typeData) override;

//	bool GetJsonContactInfo(QByteArray& jsonContactInfo);

protected:
	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask);

private:
	CTreeItemModel m_treeItemModel;

//	I_REF(iqtgui::IGuiObject, m_guiCompPtr);
};


} // namespace imtauthgui


