// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAssistantApiCatalogComp.h>


// Qt includes
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


namespace imtassistgql
{


namespace
{


const QStringList s_forbiddenCommandWords = {
	"password", "token", "session", "login", "logout", "authoriz", "superuser", "secret", "credential", "register"
};

const QStringList s_forbiddenFieldWords = {
	"password", "token", "secret", "apikey", "privatekey", "credential", "salt", "hash"
};


} // anonymous namespace


// public methods

int CAssistantApiCatalogComp::LoadPath(const QString& path)
{
	QStringList filePaths;
	const QFileInfo pathInfo(path);
	if (pathInfo.isFile()){
		filePaths.append(path);
	}
	else if (pathInfo.isDir()){
		QDirIterator iterator(path, QStringList() << QStringLiteral("*.sdl"), QDir::Files, QDirIterator::Subdirectories);
		while (iterator.hasNext()){
			filePaths.append(iterator.next());
		}

		filePaths.sort();
	}
	else{
		SendWarningMessage(0, QStringLiteral("API schema path '%1' does not exist").arg(path), "AssistantApiCatalog");

		return 0;
	}

	int retVal = 0;
	for (const QString& filePath: filePaths){
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly)){
			SendWarningMessage(0, QStringLiteral("API schema '%1' could not be read").arg(filePath), "AssistantApiCatalog");

			continue;
		}

		QString error;
		if (!m_catalog.AddSchema(QFileInfo(filePath).fileName(), QString::fromUtf8(file.readAll()), &error)){
			SendWarningMessage(0, QStringLiteral("API schema '%1' was read partly: %2").arg(filePath, error), "AssistantApiCatalog");
		}

		++retVal;
	}

	return retVal;
}


// reimplemented (imtassistgql::IAssistantApiCatalog)

const CSdlApiCatalog& CAssistantApiCatalogComp::GetCatalog() const
{
	return m_catalog;
}


bool CAssistantApiCatalogComp::IsCommandAllowed(const QString& commandName) const
{
	const CSdlApiCatalog::Command* commandPtr = m_catalog.FindCommand(commandName);
	if (commandPtr == nullptr){
		return false;
	}

	if (Contains(m_forbiddenCommands, commandName)){
		return false;
	}

	const QString lowerName = commandName.toLower();
	for (const QString& word: s_forbiddenCommandWords){
		if (lowerName.contains(word)){
			return false;
		}
	}

	if (commandPtr->kind == CSdlApiCatalog::CK_MUTATION){
		return Contains(m_allowedMutations, commandName);
	}

	return Contains(m_allowedQueries, commandName);
}


bool CAssistantApiCatalogComp::IsFieldForbidden(const QString& fieldName) const
{
	const QString lowerName = fieldName.toLower();
	for (const QString& word: s_forbiddenFieldWords){
		if (lowerName.contains(word)){
			return true;
		}
	}

	return m_forbiddenFields.contains(lowerName);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CAssistantApiCatalogComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	for (int i = 0; i < m_allowedQueriesAttrPtr.GetCount(); ++i){
		m_allowedQueries.append(m_allowedQueriesAttrPtr[i]);
	}

	for (int i = 0; i < m_allowedMutationsAttrPtr.GetCount(); ++i){
		m_allowedMutations.append(m_allowedMutationsAttrPtr[i]);
	}

	for (int i = 0; i < m_forbiddenCommandsAttrPtr.GetCount(); ++i){
		m_forbiddenCommands.append(m_forbiddenCommandsAttrPtr[i]);
	}

	for (int i = 0; i < m_forbiddenFieldsAttrPtr.GetCount(); ++i){
		m_forbiddenFields.append(QString::fromUtf8(m_forbiddenFieldsAttrPtr[i]).toLower());
	}

	int filesCount = 0;
	for (int i = 0; i < m_schemaPathsAttrPtr.GetCount(); ++i){
		filesCount += LoadPath(m_schemaPathsAttrPtr[i]);
	}

	int allowedCount = 0;
	const QStringList commandNames = m_catalog.GetCommandNames();
	for (const QString& commandName: commandNames){
		if (IsCommandAllowed(commandName)){
			++allowedCount;
		}
	}

	SendInfoMessage(0, QStringLiteral("Assistant API: %1 schema files, %2 commands, %3 allowed").arg(filesCount).arg(commandNames.size()).arg(allowedCount), "AssistantApiCatalog");
}


// private methods

bool CAssistantApiCatalogComp::Contains(const QByteArrayList& list, const QString& value)
{
	return list.contains(value.toUtf8());
}


} // namespace imtassistgql


