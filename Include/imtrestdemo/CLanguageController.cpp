// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CLanguageController.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QVariantMap>


namespace imtrestdemo
{


namespace
{


const QString s_languageKey = QStringLiteral("language");
const QString s_englishCode = QStringLiteral("en");


// The languages that the application knows: the names are written in their own language, so a person can find his language
// whatever language is on now
struct SLanguageInfo
{
	const char* code;
	const char* name;
};


const SLanguageInfo s_languages[] = {
	{"en", "English"},
	{"ru", "\xD0\xA0\xD1\x83\xD1\x81\xD1\x81\xD0\xBA\xD0\xB8\xD0\xB9"}};


} // namespace


CLanguageController::CLanguageController(QObject* parentPtr)
	:QObject(parentPtr)
{
}


CLanguageController::~CLanguageController()
{
	if (m_isTranslatorInstalled){
		QCoreApplication::removeTranslator(&m_translator);
	}
}


void CLanguageController::SetTranslations(const QString& directoryPath, const QString& filePrefix)
{
	m_directoryPath = directoryPath;
	m_filePrefix = filePrefix;
}


void CLanguageController::LoadSavedLanguage()
{
	const QString saved = QSettings().value(s_languageKey).toString();

	Apply(IsAvailable(saved) ? saved : GetSystemLanguage(), false);
}


QString CLanguageController::GetLanguage() const
{
	return m_language;
}


void CLanguageController::SetLanguage(const QString& code)
{
	if (IsAvailable(code)){
		Apply(code, true);
	}
}


void CLanguageController::SetLanguageTemporarily(const QString& code)
{
	if (IsAvailable(code)){
		Apply(code, false);
	}
}


QVariantList CLanguageController::GetLanguages() const
{
	QVariantList retVal;

	for (const SLanguageInfo& info : s_languages){
		const QString code = QString::fromLatin1(info.code);
		if (IsAvailable(code)){
			QVariantMap language;
			language.insert("code", code);
			language.insert("name", QString::fromUtf8(info.name));
			retVal.append(language);
		}
	}

	return retVal;
}


int CLanguageController::GetLanguageIndex() const
{
	const QVariantList languages = GetLanguages();

	for (int i = 0; i < languages.size(); ++i){
		if (languages.at(i).toMap().value("code").toString() == m_language){
			return i;
		}
	}

	return 0;
}


QString CLanguageController::GetSystemLanguage() const
{
	const QString code = QLocale::system().name().section('_', 0, 0);

	return IsAvailable(code) ? code : s_englishCode;
}


// private methods

bool CLanguageController::IsAvailable(const QString& code) const
{
	if (code == s_englishCode){
		return true;
	}

	return !code.isEmpty() && !m_directoryPath.isEmpty() && QFile::exists(QStringLiteral("%1/%2_%3.qm").arg(m_directoryPath, m_filePrefix, code));
}


void CLanguageController::Apply(const QString& code, bool isSaved)
{
	if (isSaved){
		QSettings().setValue(s_languageKey, code);
	}

	if (code == m_language && (m_isTranslatorInstalled || code == s_englishCode)){
		return;
	}

	if (m_isTranslatorInstalled){
		QCoreApplication::removeTranslator(&m_translator);
		m_isTranslatorInstalled = false;
	}

	if (code != s_englishCode){
		if (m_translator.load(QStringLiteral("%1_%2.qm").arg(m_filePrefix, code), m_directoryPath)){
			QCoreApplication::installTranslator(&m_translator);
			m_isTranslatorInstalled = true;
		}
	}

	m_language = code;

	Q_EMIT languageChanged();
}


} // namespace imtrestdemo

