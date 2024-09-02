#include <imtbase/CGuiElementRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/IUserInfo.h>
#include <imtgui/IGuiElementContainer.h>


namespace imtbase
{


// public methods

bool CGuiElementRepresentationControllerComp::SetupItemModel(
			const imtgui::IGuiElementModel& guiElementModel,
			CTreeItemModel& representation,
			int index,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!m_commandRepresentationControllerPtr.IsValid()){
		return false;
	}

	imtbase::CTreeItemModel commandRepresentation;
	if (!m_commandRepresentationControllerPtr->GetRepresentationFromDataModel(guiElementModel, commandRepresentation, paramsPtr)){
		return false;
	}

	if (!representation.CopyItemDataFromModel(index, &commandRepresentation)){
		return false;
	}

	return true;
}


// reimplemented (imtbase::CObjectRepresentationControllerCompBase)

bool CGuiElementRepresentationControllerComp::GetRepresentationFromValue(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	const imtgui::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	Q_ASSERT(guiElementContainerPtr != nullptr);

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	imtauth::IUserInfo::FeatureIds userPermissions;
	bool isAdmin = true;
	if (userInfoParamPtr.IsValid()){
		userPermissions = userInfoParamPtr->GetPermissions();
		isAdmin = userInfoParamPtr->IsAdmin();
	}

	representation.Clear();

	QByteArrayList elementIds = guiElementContainerPtr->GetElementIds();

	for (int i = 0; i < elementIds.count(); i++){
		const QByteArray elementId = elementIds[i];
		const imtgui::IGuiElementModel* guiElementPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
		if (guiElementPtr != nullptr){
			if (!isAdmin){
				if (m_commandPermissionsProviderCompPtr.IsValid() && !elementId.isEmpty()){
					QByteArrayList elementPermissions = m_commandPermissionsProviderCompPtr->GetCommandPermissions(elementId);

					if (m_checkPermissionCompPtr.IsValid()){
						bool result = m_checkPermissionCompPtr->CheckPermission(userPermissions, elementPermissions);
						if (!result){
							continue;
						}
					}
				}
			}

			int index = representation.InsertNewItem();
			SetupItemModel(*guiElementPtr, representation, index, paramsPtr);
		}
	}

	return true;
}


// reimplemented (IRepresentationController)

bool CGuiElementRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtgui::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementPtr != nullptr){
		return true;
	}

	return false;
}


bool CGuiElementRepresentationControllerComp::GetDataModelFromRepresentation(
			const CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return true;
}


// reimplemented (icomp::CComponentBase)

void CGuiElementRepresentationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_commandRepresentationControllerPtr.SetPtr(new imtbase::CCommandRepresentationController(m_translationManagerCompPtr.GetPtr()));
}


} // namespace imtbase


