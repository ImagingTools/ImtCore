#include <imtqml/CClientUserContextComp.h>


// Qt includes
#include <QtQml/QQmlEngine>


namespace imtqml
{


CClientUserContextComp::CClientUserContextComp()
{
}


void CClientUserContextComp::SetLanguage(QString translation)
{
	if (m_translationManagerCompPtr.IsValid()){
		if (!translation.isEmpty()){
			int languageIndex = languageIndex = iprm::FindOptionIndexById(translation.toUtf8(), m_translationManagerCompPtr->GetLanguagesInfo());
			if (languageIndex >= 0){
				m_translationManagerCompPtr->SwitchLanguage(languageIndex);

				if (m_enginePtr != nullptr){
					m_enginePtr->retranslate();
				}

				Q_EMIT LanguageChanged();
			}
		}
	}
}


QString CClientUserContextComp::GetLanguage() const
{
	return m_language;
}


void CClientUserContextComp::SetQmlEngine(QQmlEngine *engine)
{
	if (engine != nullptr){
		m_enginePtr = engine;
	}
}


} // namespace imtqml


