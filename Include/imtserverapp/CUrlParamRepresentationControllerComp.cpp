// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CUrlParamRepresentationControllerComp.h>


// ImtCore includes
#include <imtbase/IUrlParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CUrlParam>)

QByteArray CUrlParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::UrlParam.toUtf8();
}


bool CUrlParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::IUrlParam* urlParamPtr = dynamic_cast<const imtbase::IUrlParam*>(&dataModel);
	return urlParamPtr != nullptr;
}


bool CUrlParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CUrlParam& sdlRepresentation,
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
			const sdl::V1_0::imtbase::CUrlParam& sdlRepresentation) const
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


