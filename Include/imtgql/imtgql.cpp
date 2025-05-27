#include <imtgql/imtgql.h>


// ImtCore includes
#include <iqt/iqt.h>


namespace imtgql
{


// public methods

QByteArray GetLanguageIdFromRequest(const QList<imtgql::CGqlParamObject>& inputParams)
{
	int count = inputParams.count();
	for (int i = 0; i < count; i++){
		if (inputParams.at(i).GetParamIds().contains("LanguageId")){
			return inputParams.at(i).GetParamArgumentValue("LanguageId").toByteArray();
		}
	}

	return QByteArray();
}


QByteArray GetDesignSchemeIdFromRequest(const QList<imtgql::CGqlParamObject>& /*inputParams*/)
{
	return QByteArray();
}


QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const imtgql::CGqlRequest& gqlRequest, const QByteArray& phrase, const QByteArray& context)
{
	if (translationManagerPtr == nullptr){
		return phrase;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return phrase;
	}

	QByteArray languageId = gqlContextPtr->GetLanguageId();

	return iqt::GetTranslation(translationManagerPtr, phrase, languageId, context);
}


} // namespace imtqml


