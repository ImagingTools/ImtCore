#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


namespace imtserverapp
{


class CParamSetRepresentationController: public imtserverapp::TJsonRepresentationControllerWrap<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0>
{
public:
	// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation) const override;

	bool RegisterSubController(const imtserverapp::IJsonRepresentationController& controller) const;

private:
	mutable QMap<QByteArray, const imtserverapp::IJsonRepresentationController*> m_representationControllersMap;
};


} // namespace imtserverapp


