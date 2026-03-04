// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CClientUserContextComp.h>


// Qt includes
#include <QtQml/QQmlContext>


namespace imtqml
{


// public methods

CClientUserContextComp::CClientUserContextComp()
	:m_qmlEngine(nullptr),
	m_languageParamObserver(*this),
	m_quickObjectCreatedObserver(*this)
{
}


void CClientUserContextComp::OnLanguageChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::ISelectionParam* objectPtr)
{
	if (!m_translationManagerCompPtr.IsValid()){
		return;
	}

	if (objectPtr != nullptr){
		int languageIndex = iprm::FindOptionIndexById(m_language.toUtf8(), *objectPtr->GetSelectionConstraints());
		if (languageIndex >= 0){
			m_translationManagerCompPtr->SwitchLanguage(languageIndex);

			if (m_qmlEngine != nullptr){
				m_qmlEngine->retranslate();
			}
		}
	}
}


void CClientUserContextComp::OnQuickObjectCreatedChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::IEnableableParam* objectPtr)
{
	if (!m_quickObjectCompPtr.IsValid()){
		return;
	}

	if (objectPtr != nullptr){
		bool isEnabled = objectPtr->IsEnabled();
		if (isEnabled){
			QQuickItem* quickObjectPtr = m_quickObjectCompPtr->GetQuickItem();
			if (quickObjectPtr != nullptr){
				m_qmlEngine = qmlEngine(quickObjectPtr);

				QQmlContext* roolContextPtr = m_qmlEngine->rootContext();
				if (roolContextPtr != nullptr){
					roolContextPtr->setContextProperty("context", this);
				}
			}
		}
	}
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

	if (m_languageParamPtr.IsValid()){
		m_languageParamObserver.RegisterObject(m_languageParamPtr.GetPtr(), &CClientUserContextComp::OnLanguageChanged);
	}

	if (m_quickObjectCreatedCompPtr.IsValid()){
		m_quickObjectCreatedObserver.RegisterObject(m_quickObjectCreatedCompPtr.GetPtr(), &CClientUserContextComp::OnQuickObjectCreatedChanged);
	}
}


void CClientUserContextComp::OnComponentDestroyed()
{
	m_languageParamObserver.UnregisterAllObjects();
	m_quickObjectCreatedObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


void CClientUserContextComp::SetLanguage(QString translation)
{
	if (m_language != translation){
		m_language = translation;

		Q_EMIT LanguageChanged();
	}
}


QString CClientUserContextComp::GetLanguage() const
{
	return m_language;
}


} // namespace imtqml


