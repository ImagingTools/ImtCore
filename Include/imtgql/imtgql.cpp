#include <imtgql/imtgql.h>


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtgql
{


// public methods

QByteArray GetLanguageIdFromRequest(const QList<imtgql::CGqlObject>& inputParams)
{
	int count = inputParams.count();
	for (int i = 0; i < count; i++){
		if (inputParams.at(i).GetFieldIds().contains("LanguageId")){
			return inputParams.at(i).GetFieldArgumentValue("LanguageId").toByteArray();
		}
	}

	return QByteArray();
}


QByteArray GetDesignSchemeIdFromRequest(const QList<imtgql::CGqlObject>& /*inputParams*/)
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

	return imtbase::GetTranslation(translationManagerPtr, phrase, languageId, context);
}


} // namespace imtqml


