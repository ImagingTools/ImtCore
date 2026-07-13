// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CFileSystemControllerComp.h>

// Qt includes
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtservergql
{


// protected methods

// reimplemented (CGqlRequestHandlerCompBase)

QJsonObject CFileSystemControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_fileSystemProviderCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to get file system entries. Error: Attribute 'FileSystemProvider' was not set");
		SendErrorMessage(0, errorMessage, "CFileSystemControllerComp");

		return QJsonObject();
	}

	QString path;

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		path = inputParamPtr->GetParamArgumentValue("path").toString();
	}

	imtfile::IFileSystemStructureProvider::FileSystemEntryList entries;
	QString resolvedPath;
	QString parentPath;
	if (!m_fileSystemProviderCompPtr->GetFileSystemEntries(path, entries, resolvedPath, parentPath, errorMessage)){
		return QJsonObject();
	}

	QJsonArray entriesArray;
	for (const imtfile::IFileSystemStructureProvider::FileSystemEntry& entry : entries){
		QJsonObject entryObj;
		entryObj.insert("name", entry.name);
		entryObj.insert("path", entry.path);
		entryObj.insert("isDir", entry.isDir);
		entryObj.insert("size", entry.size);
		entryObj.insert("lastModified", entry.lastModified.toString(Qt::ISODate));

		entriesArray.append(entryObj);
	}

	QJsonObject dataObj;
	dataObj.insert("path", resolvedPath);
	dataObj.insert("parentPath", parentPath);
	dataObj.insert("entries", entriesArray);

	QJsonObject rootObj;
	rootObj.insert("data", dataObj);

	return rootObj;
}


} // namespace imtservergql
