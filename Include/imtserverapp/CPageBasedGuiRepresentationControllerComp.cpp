#include <imtserverapp/CPageBasedGuiRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtqml/IPageGuiElementModel.h>
#include <imtserverapp/IGuiElementContainer.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtbase::CGuiElementRepresentationControllerComp)

bool CPageBasedGuiRepresentationControllerComp::SetupItemModel(
			const imtserverapp::IGuiElementModel& guiElementModel,
			imtbase::CTreeItemModel& representation,
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


} // namespace imtserverapp


