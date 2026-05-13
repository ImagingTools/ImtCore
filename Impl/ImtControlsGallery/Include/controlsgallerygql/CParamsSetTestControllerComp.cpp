#include <controlsgallerygql/CParamsSetTestControllerComp.h>


namespace controlsgallerygql
{


// protected methods

// reimplemented (sdl::controlsgallery::ParamsSetTest::CGraphQlHandlerCompBase)

sdl::imtbase::ImtBaseTypes::CParamsSet CParamsSetTestControllerComp::OnGetParamsSet(
	const sdl::controlsgallery::ParamsSetTest::CGetParamsSetGqlRequest& getParamsSetRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::imtbase::ImtBaseTypes::CParamsSet retVal;

	// Text parameter
	sdl::imtbase::ImtBaseTypes::CTextParam strParam;
	strParam.Version_1_0.emplace().text = "SampleText";

	QJsonObject jsonObject;
	if (!strParam.WriteToJsonObject(jsonObject, strParam.PV_1_0)){
		return retVal;
	}

	retVal.Version_1_0.emplace();

	QJsonDocument jsonDocument;
	jsonDocument.setObject(jsonObject);

	// ParamsSet parameter
	sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameter;
	parameter.id = "SampleId";
	parameter.name = "SampleName";
	parameter.description = "SampleDesc";
	parameter.enabled = true;
	parameter.typeId = "TextParam";
	parameter.data = jsonDocument.toJson(QJsonDocument::Compact);

	retVal.Version_1_0->parameters.emplace().append(parameter);

	return retVal;
}


sdl::controlsgallery::ParamsSetTest::CSetParamsSetResult CParamsSetTestControllerComp::OnSetParamsSet(
	const sdl::controlsgallery::ParamsSetTest::CSetParamsSetGqlRequest& setParamsSetRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{

	sdl::controlsgallery::ParamsSetTest::CSetParamsSetResult retVal;

	retVal.Version_1_0.emplace().result = true;

	return retVal;


	auto paramsSet = setParamsSetRequest.GetRequestedArguments().input;

	if (!paramsSet.Version_1_0){
		errorMessage = "ParamsSet: V1_0 is null";
		return retVal;
	}

	if (paramsSet.Version_1_0->parameters){
		errorMessage = "ParamsSet: parameters is null";
		return retVal;
	}

	auto parameters = *paramsSet.Version_1_0->parameters;
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
	sdl::imtbase::ImtBaseTypes::CTextParam strParam;
	if (!strParam.ReadFromJsonObject(jsonDoc.object())){
		errorMessage = "ParamsSet: Failed to deserialize text param";
		return retVal;
	}

	if (!strParam.Version_1_0){
		errorMessage = "ParamsSet: Text param V1_0 is null";
		return retVal;
	}

	if (strParam.Version_1_0->text != "SampleText"){
		errorMessage = "ParamsSet: Invalid text param value";
		return retVal;
	}

	retVal.Version_1_0.emplace().result = true;

	return retVal;
}


} // namespace controlsgallerygql


