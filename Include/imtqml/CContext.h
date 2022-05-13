#pragma once


//Qt includes
#include <QObject>
#include <QTranslator>


namespace imtqml
{


class CContext: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString language READ GetLanguage WRITE SetLanguage NOTIFY LanguageChanged)

public:
	explicit CContext(QObject *parent = 0);

signals:
	void LanguageChanged();

public:
	void SetLanguage(QString translation);
	QString GetLanguage() const;

private:
	QTranslator m_translator;
};


} // namespace imtqml
