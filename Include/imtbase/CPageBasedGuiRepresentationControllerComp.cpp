#include <imtbase/CPageBasedGuiRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtqml/IPageGuiElementModel.h>
#include <imtgui/IGuiElementContainer.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::CGuiElementRepresentationControllerComp)

bool CPageBasedGuiRepresentationControllerComp::SetupItemModel(
			const imtgui::IGuiElementModel& guiElementModel,
			CTreeItemModel& representation,
			int index,
			const iprm::IParamsSet* paramsPtr) const
{
	BaseClass::SetupItemModel(guiElementModel, representation, index, paramsPtr);

	const imtqml::IPageGuiElementModel* pageGuiElementPtr = dynamic_cast<const imtqml::IPageGuiElementModel*>(&guiElementModel);
	if (pageGuiElementPtr == nullptr){
		return false;
	}

	QString pageQmlItemFilePath = pageGuiElementPtr->GetPageQmlItemFilePath();
	QString startSourceItem = pageGuiElementPtr->GetStartSourceItem();

	representation.SetData("Source", pageQmlItemFilePath, index);
	representation.SetData("StartItem", startSourceItem, index);

	return true;
}


} // namespace imtbase


