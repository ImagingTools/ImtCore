#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtauth/IContactInfo.h>
#include <imtauthgui/IDataController.h>


namespace imtauthgui
{


/**
	Represenatation controller for contact info data model.
	\ingroup LicenseManagement
*/
class CContactInfoEditorDataController:
		public IDataController,
		public imod::TSingleModelObserverBase<imtauth::IContactInfo>
{
public:
	typedef imod::TSingleModelObserverBase<imtauth::IContactInfo> BaseClass;

	// reimplemented (imtauthgui::IDataController)
	bool GetJsonData(QByteArray& jsonData, const QString& typeData = "") const override;
	imtbase::CTreeItemModel* GetTreeItemModel(const QString& typeData) const override;

protected:
	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask);

private:
	mutable imtbase::CTreeItemModel m_treeItemModel;
};


} // namespace imtauthgui


