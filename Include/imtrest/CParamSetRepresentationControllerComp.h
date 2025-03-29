#pragma once


// ImtCore includes
#include <imtrest/TJsonRepresentationControllerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


namespace imtrest
{


class CParamSetRepresentationControllerComp: public imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0>
{
public:
	typedef imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0> BaseClass;

	I_BEGIN_COMPONENT(CParamSetRepresentationControllerComp)
		I_ASSIGN_MULTI_0(m_paramRepresentationControllersCompPtr, "ParamRepresentationControllers", "Sub parameters representation controllers", false);
	I_END_COMPONENT;

protected:
	const imtrest::IJsonRepresentationController* FindSubController(const QByteArray& modelId) const;
	
	// reimplemented (imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation) const override;

protected:
	I_MULTIREF(imtrest::IJsonRepresentationController, m_paramRepresentationControllersCompPtr);
};


} // namespace imtrest


