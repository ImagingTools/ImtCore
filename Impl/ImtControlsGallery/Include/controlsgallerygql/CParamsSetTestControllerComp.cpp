#include <controlsgallerygql/CParamsSetTestControllerComp.h>
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ParamsSetTest.h>


namespace controlsgallerygql
{


// protected methods

// reimplemented (sdl::V1_0::controlsgallery::CParamsSetTestGqlHandlerCompBase)

sdl::V1_0::imtbase::CParamsSet CParamsSetTestControllerComp::OnGetParamsSet(
	const sdl::V1_0::controlsgallery::CGetParamsSetGqlRequest& getParamsSetRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::V1_0::imtbase::CParamsSet retVal;

	// Text parameter
	sdl::V1_0::imtbase::CTextParam strParam;
	strParam.text = "SampleText";

	QJsonObject jsonObject;
	if (!strParam.WriteToJsonObject(jsonObject)){
		return retVal;
	}


	QJsonDocument jsonDocument;
	jsonDocument.setObject(jsonObject);

	// ParamsSet parameter
	sdl::V1_0::imtbase::CParameter parameter;
	parameter.id = "SampleId";
	parameter.name = "SampleName";
	parameter.description = "SampleDesc";
	parameter.enabled = true;
	parameter.typeId = "TextParam";
	parameter.data = jsonDocument.toJson(QJsonDocument::Compact);

	retVal.parameters.emplace().append(parameter);

	return retVal;
}


sdl::V1_0::controlsgallery::CSetParamsSetResult CParamsSetTestControllerComp::OnSetParamsSet(
	const sdl::V1_0::controlsgallery::CSetParamsSetGqlRequest& setParamsSetRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{

	sdl::V1_0::controlsgallery::CSetParamsSetResult retVal;

	retVal.result = true;

	return retVal;


	auto paramsSet = setParamsSetRequest.GetRequestedArguments().input;


	if (paramsSet.parameters){
		errorMessage = "ParamsSet: parameters is null";
		return retVal;
	}

	auto parameters = *paramsSet.parameters;
	if (parameters.count() == 1){
		errorMessage = "ParamsSet: invalid parameters count";
		return retVal;
	}

	auto parameter = parameters[0];
	if (parameter){
		errorMessage = "ParamsSet: parameter is null";
		return retVal;
	}

	if (
		parameter->id != "SampleId" ||
		parameter->name != "SampleName" ||
		parameter->description != "SampleDesc" ||
		parameter->enabled != true ||
		parameter->typeId != "TextParam"){

		errorMessage = "ParamsSet: comparison of parameter fields failed";
		return retVal;
	}

	QJsonDocument jsonDoc = QJsonDocument::fromJson(parameter->data->toUtf8());
	sdl::V1_0::imtbase::CTextParam strParam;
	if (!strParam.ReadFromJsonObject(jsonDoc.object())){
		errorMessage = "ParamsSet: Failed to deserialize text param";
		return retVal;
	}


	if (strParam.text != "SampleText"){
		errorMessage = "ParamsSet: Invalid text param value";
		return retVal;
	}

	retVal.result = true;

	return retVal;
}


} // namespace controlsgallerygql


