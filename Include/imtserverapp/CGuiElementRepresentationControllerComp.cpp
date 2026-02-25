// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CGuiElementRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtserverapp/IGuiElementContainer.h>


namespace imtserverapp
{


// public methods

bool CGuiElementRepresentationControllerComp::SetupItemModel(
			const imtserverapp::IGuiElementModel& guiElementModel,
			imtbase::CTreeItemModel& representation,
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


// reimplemented (imtserverapp::CObjectRepresentationControllerCompBase)

bool CGuiElementRepresentationControllerComp::GetRepresentationFromValue(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	const imtserverapp::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtserverapp::IGuiElementContainer*>(&dataModel);
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
		const imtserverapp::IGuiElementModel* guiElementPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
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
	const imtserverapp::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtserverapp::IGuiElementContainer*>(&dataModel);

	return guiElementPtr != nullptr;
}


bool CGuiElementRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return true;
}


// reimplemented (icomp::CComponentBase)

void CGuiElementRepresentationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_commandRepresentationControllerPtr.SetPtr(new imtserverapp::CCommandRepresentationController(m_translationManagerCompPtr.GetPtr()));
}


} // namespace imtserverapp


