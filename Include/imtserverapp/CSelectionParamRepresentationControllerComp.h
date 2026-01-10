#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


namespace imtserverapp
{


class CSelectionParamRepresentationControllerComp: public TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0>
{
public:
	typedef TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0> BaseClass;

	I_BEGIN_COMPONENT(CSelectionParamRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0& sdlRepresentation) const override;
};


} // namespace imtbase


