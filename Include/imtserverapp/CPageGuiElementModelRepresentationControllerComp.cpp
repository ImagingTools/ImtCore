// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CPageGuiElementModelRepresentationControllerComp.h>


// ImtCore includes
#include <imtqml/IPageGuiElementModel.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::IRepresentationController)

bool CPageGuiElementModelRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtqml::IPageGuiElementModel* guiElementPtr = dynamic_cast<const imtqml::IPageGuiElementModel*>(&dataModel);

	return guiElementPtr != nullptr;
}


bool CPageGuiElementModelRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtqml::IPageGuiElementModel* guiElementPtr = dynamic_cast<const imtqml::IPageGuiElementModel*>(&dataModel);
	if (guiElementPtr == nullptr){
		return false;
	}

	bool result = BaseClass::GetRepresentationFromDataModel(dataModel, representation, paramsPtr);
	if (!result){
		return false;
	}

	QString pageQmlItemFilePath = guiElementPtr->GetPageQmlItemFilePath();
	QString startSourceItem = guiElementPtr->GetStartSourceItem();

	representation.SetData("source", pageQmlItemFilePath);
	representation.SetData("startItem", startSourceItem);

	return true;
}


bool CPageGuiElementModelRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


} // namespace imtgui


