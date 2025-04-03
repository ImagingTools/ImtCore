#include <imtserverapp/CUrlParamRepresentationControllerComp.h>


// ImtCore includes
#include <imtbase/IUrlParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0>)

QByteArray CUrlParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::UrlParam.toUtf8();
}


bool CUrlParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::IUrlParam* urlParamPtr = dynamic_cast<const imtbase::IUrlParam*>(&dataModel);
	if (urlParamPtr != nullptr){
		return true;
	}
	
	return false;
}


bool CUrlParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtbase::IUrlParam* urlParamPtr = dynamic_cast<const imtbase::IUrlParam*>(&dataModel);
	Q_ASSERT(urlParamPtr != nullptr);
	if (urlParamPtr == nullptr){
		return false;
	}
	
	QUrl url = urlParamPtr->GetUrl();
	
	sdlRepresentation.host = url.host();
	sdlRepresentation.scheme = url.scheme();
	sdlRepresentation.port = url.port();
	sdlRepresentation.path = url.path();
	
	return true;
}


bool CUrlParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0& sdlRepresentation) const
{
	imtbase::IUrlParam* urlParamPtr = dynamic_cast<imtbase::IUrlParam*>(&dataModel);
	Q_ASSERT(urlParamPtr != nullptr);
	if (urlParamPtr == nullptr){
		return false;
	}
	
	QUrl url;
	
	if (sdlRepresentation.scheme){
		url.setScheme(*sdlRepresentation.scheme);
	}
	
	if (sdlRepresentation.host){
		url.setHost(*sdlRepresentation.host);
	}
	
	if (sdlRepresentation.port){
		url.setPort(*sdlRepresentation.port);
	}
	
	if (sdlRepresentation.path){
		url.setPath(*sdlRepresentation.path);
	}
	
	urlParamPtr->SetUrl(url);
	
	return true;
}


} // namespace imtserverapp


