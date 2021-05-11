#pragma once

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataController.h>
#include <imtbase/CTreeItemModel.h>
#include <imtauth/IAccountInfo.h>


namespace imtauthgui
{


/**
	ContactInfo Representation data controller
	\ingroup LicenseManagement
*/
class CContactInfoRepresentationDataController: public imtbase::IItemBasedRepresentationDataController
{
public:
	CContactInfoRepresentationDataController();

	// reimplemented (imtauthgui::IItemBasedRepresentationController)
	virtual void UpdateRepresentationFromBaseModel(istd::IChangeable *baseModel,
												   istd::IChangeable *representationModel,
												   const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void UpdateBaseModelFromRepresentation(istd::IChangeable *baseModel,
												   istd::IChangeable *representationModel,
												   const istd::IChangeable::ChangeSet& changeSet) override;
protected:
	virtual void FullUpdateRepresentationModel(imtauth::IContactInfo* contactPtr, imtbase::CTreeItemModel *treeItemModel) const;
	virtual void FullUpdateBaseModel(imtauth::IContactInfo* contactPtr, imtbase::CTreeItemModel *treeItemModel);
private:
	bool m_isUpdateFromRepresentation;
};


} // namespace imtauthgui


