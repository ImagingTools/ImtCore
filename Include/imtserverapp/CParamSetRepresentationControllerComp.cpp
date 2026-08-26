// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CParamSetRepresentationControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace imtserverapp
{


QByteArray CParamSetRepresentationControllerComp::GetTypeId() const
{
	return m_representationController.GetTypeId();
}


bool CParamSetRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	return m_representationController.IsModelSupported(dataModel);
}


bool CParamSetRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CParamsSet& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* paramsPtr) const
{
	return m_representationController.GetSdlRepresentationFromDataModel(sdlRepresentation, dataModel, paramsPtr);
}


bool CParamSetRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CParamsSet& sdlRepresentation) const
{
	return m_representationController.GetDataModelFromSdlRepresentation(dataModel, sdlRepresentation);
}


void CParamSetRepresentationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	RegisterSubControllers(m_paramRepresentationControllersCompPtr);

	if (m_customParamRepresentationControllerCompPtr.IsValid()){
		m_representationController.RegisterSubController(*m_customParamRepresentationControllerCompPtr);
	}
}


void CParamSetRepresentationControllerComp::RegisterSubControllers(const icomp::TMultiReferenceMember<IJsonRepresentationController>& controllers)
{
	if (!controllers.IsValid()){
		return;
	}

	for (int index = 0; index < controllers.GetCount(); ++index){
		const IJsonRepresentationController* representationControllerPtr = controllers[index];
		if (representationControllerPtr != nullptr){
			m_representationController.RegisterSubController(*representationControllerPtr);
		}
	}
}


} // namespace imtserverapp