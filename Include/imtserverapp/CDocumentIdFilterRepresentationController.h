#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentIdFilter.h>


namespace imtserverapp
{


class CDocumentIdFilterRepresentationController:
			public imtserverapp::TJsonRepresentationControllerWrap<sdl::imtbase::DocumentIdFilter::CDocumentIdFilter::V1_0>
{
public:
	CDocumentIdFilterRepresentationController();

	// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<Filter::CDocumentIdFilter::V1_0>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::DocumentIdFilter::CDocumentIdFilter::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::DocumentIdFilter::CDocumentIdFilter::V1_0& sdlRepresentation) const override;
};


} // namespace imtserverapp


