// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTranslator>
#include <QtCore/QVariantList>


namespace imtrestdemo
{


/**
	Language of the interface of the demo client. The texts of the application are written in English, the other languages are
	the translation files \c <prefix>_<code>.qm (see SetTranslations()). The chosen language is kept between the runs.
	\c languageChanged() is the moment to ask the QML engine to retranslate (QQmlEngine::retranslate()) and the models of the logs to refresh
	their texts.
*/
class CLanguageController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString language READ GetLanguage WRITE SetLanguage NOTIFY languageChanged)
	Q_PROPERTY(QVariantList languages READ GetLanguages CONSTANT)
	Q_PROPERTY(int languageIndex READ GetLanguageIndex NOTIFY languageChanged)

public:
	explicit CLanguageController(QObject* parentPtr = nullptr);
	~CLanguageController();

	/**
		Set the place of the translation files, for example \c ":/Translations", and the prefix of their names, for example
		\c "imtrestdemoclient". The languages that have the file are available.
	*/
	void SetTranslations(const QString& directoryPath, const QString& filePrefix);

	/**
		Use the language that was chosen the last time; if it was not chosen, the language of the system if it is available, otherwise English.
	*/
	void LoadSavedLanguage();

	QString GetLanguage() const;

	/**
		Switch to the language ("en" or the code of a translation, for example "ru"). The choice is saved.
		An unknown code is ignored.
	*/
	Q_INVOKABLE void SetLanguage(const QString& code);

	/**
		Switch to the language without saving of the choice (the language of the command line).
	*/
	void SetLanguageTemporarily(const QString& code);

	/**
		The available languages: the list of the maps with the keys \c code and \c name (the name in its own language).
	*/
	QVariantList GetLanguages() const;
	int GetLanguageIndex() const;

	/**
		The language of the system if it is available, otherwise English.
	*/
	QString GetSystemLanguage() const;

Q_SIGNALS:
	void languageChanged();

private:
	bool IsAvailable(const QString& code) const;
	void Apply(const QString& code, bool isSaved);

private:
	QString m_directoryPath;
	QString m_filePrefix;
	QString m_language = QStringLiteral("en");
	QTranslator m_translator;
	bool m_isTranslatorInstalled = false;
};


} // namespace imtrestdemo

