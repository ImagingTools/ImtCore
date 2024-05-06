#include <imtqml/CClientUserContextComp.h>


namespace imtqml
{


// public methods

CClientUserContextComp::CClientUserContextComp()
{
}


// reimplemented (icomp::CComponentBase)

void CClientUserContextComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_translationManagerCompPtr.IsValid()){
		int languageIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();
		if (languageIndex >= 0){
			const iprm::IOptionsList& optionsList = m_translationManagerCompPtr->GetLanguagesInfo();
			m_language = optionsList.GetOptionId(languageIndex);
		}
	}
}


void CClientUserContextComp::SetLanguage(QString translation)
{
	if (m_translationManagerCompPtr.IsValid()){
		if (!translation.isEmpty()){
			int languageIndex = iprm::FindOptionIndexById(translation.toUtf8(), m_translationManagerCompPtr->GetLanguagesInfo());
			if (languageIndex >= 0){
				m_translationManagerCompPtr->SwitchLanguage(languageIndex);

				if (m_enginePtr != nullptr){
					m_enginePtr->retranslate();
				}

				m_language = translation;

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


