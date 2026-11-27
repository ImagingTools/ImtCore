// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CMarkdownKnowledgeBaseComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


namespace imtassist
{


// public methods

int CMarkdownKnowledgeBaseComp::LoadPath(const QString& path)
{
	const QFileInfo pathInfo(path);
	if (pathInfo.isFile()){
		return LoadFile(path, pathInfo.fileName()) ? 1 : 0;
	}

	if (!pathInfo.isDir()){
		SendWarningMessage(0, QStringLiteral("Knowledge path '%1' does not exist").arg(path), "KnowledgeBase");

		return 0;
	}

	const QDir directory(path);
	QStringList filePaths;
	QDirIterator iterator(path, QStringList() << QStringLiteral("*.md"), QDir::Files, QDirIterator::Subdirectories);
	while (iterator.hasNext()){
		filePaths.append(iterator.next());
	}

	// The same order on every platform, so that section IDs are stable.
	filePaths.sort();

	int retVal = 0;
	for (const QString& filePath: filePaths){
		if (LoadFile(filePath, directory.relativeFilePath(filePath))){
			++retVal;
		}
	}

	return retVal;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CMarkdownKnowledgeBaseComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int documentsCount = 0;
	for (int i = 0; i < m_documentPathsAttrPtr.GetCount(); ++i){
		documentsCount += LoadPath(m_documentPathsAttrPtr[i]);
	}

	SendInfoMessage(0, QStringLiteral("Assistant knowledge: %1 documents, %2 sections").arg(documentsCount).arg(GetSectionsCount()), "KnowledgeBase");
}


// private methods

bool CMarkdownKnowledgeBaseComp::LoadFile(const QString& filePath, const QString& source)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)){
		SendWarningMessage(0, QStringLiteral("Knowledge document '%1' could not be read").arg(filePath), "KnowledgeBase");

		return false;
	}

	if (file.size() > MAX_DOCUMENT_SIZE){
		SendWarningMessage(0, QStringLiteral("Knowledge document '%1' is too large").arg(filePath), "KnowledgeBase");

		return false;
	}

	AddDocument(source, QString::fromUtf8(file.readAll()));

	return true;
}


} // namespace imtassist


