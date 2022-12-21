#pragma once


//ACF includes
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

//Qt includes
#include <QtCore/QObject>
#include <QtCore/QTranslator>
#include <QtQml/QQmlEngine>


namespace imtqml
{


class CClientUserContextComp: public QObject, public icomp::CComponentBase
{
	Q_OBJECT
	Q_PROPERTY(QString language READ GetLanguage WRITE SetLanguage NOTIFY LanguageChanged)

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CClientUserContextComp);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

	CClientUserContextComp();

Q_SIGNALS:
	void LanguageChanged();

public:
	void SetLanguage(QString translation);
	void SetQmlEngine(QQmlEngine* m_enginePtr);
	QString GetLanguage() const;

private:
	QString m_language;
	QQmlEngine* m_enginePtr;
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtqml


