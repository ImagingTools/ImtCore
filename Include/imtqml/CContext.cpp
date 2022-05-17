#include "CContext.h"

// Qt includes
#include <QtQml/QQmlEngine>
#include <QApplication>


namespace imtqml
{

//QTranslator m_translatorTest;

CContext::CContext(QQmlEngine* engine, QObject *parent) : QObject(parent), m_engine(engine)
{
}


void CContext::SetLanguage(QString translation)
{
	if (m_translator.load("Lisa_" + translation + ".qm", "C:/Sybnavigation/ITDevelopment/ImtCore/Include/imtgui/CMake/")){
		qApp->installTranslator(&m_translator);
		if (m_engine != nullptr){
			m_engine->retranslate();
		}
		m_language = translation;
		emit LanguageChanged();
	}
}


QString CContext::GetLanguage() const
{
	return m_language;
}


} // namespace imtqml

