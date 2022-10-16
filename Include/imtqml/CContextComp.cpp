#include "CContextComp.h"

// Qt includes
#include <QtQml/QQmlEngine>
#include <QApplication>


namespace imtqml
{


CContextComp::CContextComp(QQmlEngine* engine, QObject *parent) :
	QObject(parent),
	m_engine(engine)
{
}


void CContextComp::SetLanguage(QString translation)
{
	if (m_translationManagerCompPtr.IsValid()){
		int languageIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();
		if (languageIndex >= 0){
			const QTranslator* translator =  m_translationManagerCompPtr->GetLanguageTranslator(languageIndex);
			if (translator != nullptr){
				qApp->installTranslator(const_cast<QTranslator*>(translator));

				if (m_engine != nullptr){
					m_engine->retranslate();
				}

				emit LanguageChanged();
			}
		}
	}
}


QString CContextComp::GetLanguage() const
{
	return m_language;
}


void CContextComp::SetQmlEngine(QQmlEngine *engine)
{
	if (m_engine != nullptr){
		m_engine = engine;
	}
}


} // namespace imtqml

