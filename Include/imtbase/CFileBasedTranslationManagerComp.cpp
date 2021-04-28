#include <imtbase/CFileBasedTranslationManagerComp.h>


// Qt includes
#include <QtCore/QLocale>


namespace imtbase
{


// protected methods

// reimplemented (CTranslationManagerComp)

void CFileBasedTranslationManagerComp::LoadTranslations()
{
	if (m_languagesAttrPtr.IsValid() && m_translationFilePrefixAttrPtr.IsValid()){
		int languagesCount = m_languagesAttrPtr.GetCount();

		QString translationsPath = qApp->applicationDirPath() + *m_translationFilePathAttrPtr;
		QString translationFilePrefix = *m_translationFilePrefixAttrPtr;

		for (int translatorIndex = 0; translatorIndex < languagesCount; translatorIndex++){
			QByteArray languageId = m_languagesAttrPtr[translatorIndex];

			if (*m_ignoreLocaleAttrPtr){
				int pos = languageId.indexOf("_");
				if (pos >= 0){
					languageId.chop(languageId.count() - pos);
				}
			}

			QString translatorFile = translationsPath + QString("/") + translationFilePrefix + QString("_") + languageId + QString(".qm");

			istd::TDelPtr<QTranslator> qtTranslatorPtr(new QTranslator(qApp));

			if (!qtTranslatorPtr->load(translatorFile)){
				SendVerboseMessage(QString("Cannot load translator for: %1").arg(translatorFile), "TranslationManager");
			}

			Translator translator;

			translator.translatorPtr.SetPtr(qtTranslatorPtr.PopPtr());
			translator.languageId = languageId;
			
			if (translatorIndex < m_languageNamesAttrPtr.GetCount()){
				translator.languageName = m_languageNamesAttrPtr[translatorIndex];
			}
			else{
				QLocale locale(languageId);

				translator.languageName = QLocale::languageToString(locale.language());
			}

			m_translatorsList.push_back(translator);
		}
	}
}


} // namespace imtbase


