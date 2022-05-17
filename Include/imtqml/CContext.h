#pragma once


//Qt includes
#include <QObject>
#include <QTranslator>
#include <QQmlEngine>


namespace imtqml
{


class CContext: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString language READ GetLanguage WRITE SetLanguage NOTIFY LanguageChanged)

public:
	explicit CContext(QQmlEngine* engine, QObject *parent = 0);

signals:
	void LanguageChanged();

public:
	void SetLanguage(QString translation);
	QString GetLanguage() const;

private:
	QTranslator m_translator;
	QString m_language;
	QQmlEngine* m_engine;
};


} // namespace imtqml
