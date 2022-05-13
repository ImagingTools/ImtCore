#include "CContext.h"

// Qt includes
#include <QtQml/QQmlEngine>
#include <QApplication>


namespace imtqml
{

//QTranslator m_translatorTest;

CContext::CContext(QObject *parent) : QObject(parent)
{
}


void CContext::SetLanguage(QString translation)
{
	if (m_translator.load(translation + ".qm", "c:/Sybnavigation/ITDevelopment/Lisa/Include/lisaqml/Resources/qml/Translations/")){
		qApp->installTranslator(&m_translator);
		emit LanguageChanged();
	}
}


QString CContext::GetLanguage() const
{
	return m_translator.language();
}


} // namespace imtqml

