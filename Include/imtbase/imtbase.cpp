#include <imtbase/imtbase.h>
#include <QtCore/QTranslator>


namespace imtbase
{


QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const QByteArray& phrase, const QByteArray& languageId, const QByteArray& context)
{
	int currentIndex = -1;

	if (languageId.isEmpty()){
		currentIndex = translationManagerPtr->GetCurrentLanguageIndex();
	}
	else{
		currentIndex = iprm::FindOptionIndexById(languageId, translationManagerPtr->GetLanguagesInfo());
	}

	if (currentIndex >= 0){
		QString phraseTr;
		const QTranslator* translatorPtr = translationManagerPtr->GetLanguageTranslator(currentIndex);
		if (translatorPtr != nullptr){
			phraseTr = translatorPtr->translate(context, phrase);
			if(phraseTr.isEmpty()){
				const iqt::ITranslationManager* slaveTranslatorManager = translationManagerPtr->GetSlaveTranslationManager();
				if(slaveTranslatorManager != nullptr){
					GetTranslation(slaveTranslatorManager, phrase, languageId, context);
				}
				else{
					return phrase;
				}
			}
			else
			{
				return phraseTr.toUtf8();
			}
		}
	}
	return phrase;
}


} // namespace imtbase


