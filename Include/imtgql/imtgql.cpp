#include <imtgql/imtgql.h>


// ImtCore includes
#include <iqt/iqt.h>


namespace imtgql
{


// public methods

QByteArray GetLanguageIdFromRequest(const QList<CGqlParamObject>& inputParams)
{
	qsizetype count = inputParams.count();
	for (qsizetype i = 0; i < count; i++){
		if (inputParams.at(i).GetParamIds().contains("LanguageId")){
			return inputParams.at(i).GetParamArgumentValue("LanguageId").toByteArray();
		}
	}

	return QByteArray();
}


QByteArray GetDesignSchemeIdFromRequest(const QList<CGqlParamObject>& /*inputParams*/)
{
	return QByteArray();
}


QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const CGqlRequest& gqlRequest, const QByteArray& phrase, const QByteArray& context)
{
	if (translationManagerPtr == nullptr){
		return phrase;
	}

	const IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return phrase;
	}

	QByteArray languageId = gqlContextPtr->GetLanguageId();

	return iqt::GetTranslation(translationManagerPtr, phrase, languageId, context);
}


} // namespace imtqml


