// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerCompWrap.h>
#include <imtserverapp/CParamSetRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace imtserverapp
{


class CParamSetRepresentationControllerComp: public TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0>
{
public:
	typedef TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0> BaseClass;

	I_BEGIN_COMPONENT(CParamSetRepresentationControllerComp)
		I_ASSIGN_MULTI_0(m_paramRepresentationControllersCompPtr, "ParamRepresentationControllers", "Sub parameters representation controllers", false);
	I_END_COMPONENT;

protected:
	// reimplemented (TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_MULTIREF(IJsonRepresentationController, m_paramRepresentationControllersCompPtr);

private:
	CParamSetRepresentationController m_representationController;
};


} // namespace imtserverapp


