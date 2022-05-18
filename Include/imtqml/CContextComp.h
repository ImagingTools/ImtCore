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


class CContextComp: public QObject, public icomp::CComponentBase
{
	Q_OBJECT
	Q_PROPERTY(QString language READ GetLanguage WRITE SetLanguage NOTIFY LanguageChanged)

public:
	typedef icomp::CComponentBase BaseClass;
	explicit CContextComp(QQmlEngine* engine = nullptr, QObject *parent = 0);

	I_BEGIN_COMPONENT(CContextComp);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

signals:
	void LanguageChanged();

public:
	void SetLanguage(QString translation);
	void SetQmlEngine(QQmlEngine* m_engine);
	QString GetLanguage() const;

private:
	QString m_language;
	QQmlEngine* m_engine;
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtqml
