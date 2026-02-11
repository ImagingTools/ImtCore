// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CParamSetRepresentationControllerComp.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet>)

QByteArray CParamSetRepresentationControllerComp::GetTypeId() const
{
	return m_representationController.GetTypeId();
}


bool CParamSetRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	return m_representationController.IsModelSupported(dataModel);
}


bool CParamSetRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* paramsPtr) const
{
	return m_representationController.GetSdlRepresentationFromDataModel(sdlRepresentation, dataModel, paramsPtr);
}


bool CParamSetRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation) const
{
	return m_representationController.GetDataModelFromSdlRepresentation(dataModel, sdlRepresentation);
}


// reimplemented (icomp::CComponentBase)

void CParamSetRepresentationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_paramRepresentationControllersCompPtr.IsValid()){
		for (int i = 0; i < m_paramRepresentationControllersCompPtr.GetCount(); ++i){
			const imtserverapp::IJsonRepresentationController* representationControllerPtr = m_paramRepresentationControllersCompPtr[i];
			if (representationControllerPtr != nullptr){
				m_representationController.RegisterSubController(*representationControllerPtr);
			}
		}
	}
}


} // namespace imtserverapp


