#include <imtgui/CCommandGroupRepresentationControllerComp.h>


// ImtCore includes
#include <imtbase/ICommandGroup.h>


namespace imtgui
{


// protected methods

// reimplemented (IRepresentationController)

bool CCommandGroupRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::ICommandGroup* guiElementPtr = dynamic_cast<const imtbase::ICommandGroup*>(&dataModel);
	if (guiElementPtr != nullptr) {
		return true;
	}

	return false;
}


bool CCommandGroupRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	if (!BaseClass::GetRepresentationFromDataModel(dataModel, representation, paramsPtr)){
		return false;
	}

	const imtbase::ICommandGroup* commandGroupPtr = dynamic_cast<const imtbase::ICommandGroup*>(&dataModel);
	if (commandGroupPtr == nullptr) {
		return false;
	}

	representation.SetData("Alignment", commandGroupPtr->GetAlignment());

	return true;
}


bool CCommandGroupRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


} // namespace imtgui


