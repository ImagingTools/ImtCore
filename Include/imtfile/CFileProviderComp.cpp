// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CFileProviderComp.h>

// Qt includes
#include <QtCore/QRegularExpressionMatch>


namespace imtfile
{


// reimplemented (IFileProvider)

bool CFileProviderComp::LoadData(QByteArray& data, const QByteArray& name) const
{
	QByteArray workingFileName = name;
	QString homeDirPath = *m_homeDirPathAttrPtr;

	if (m_fileTemplatePathCompPtr.IsValid() && !m_fileTemplatePathCompPtr->GetPath().isEmpty()){
		homeDirPath = m_fileTemplatePathCompPtr->GetPath();
	}

	if (*m_pathsProblemsAutoSolveAttrPtr){
		qsizetype indexOfPathSeparator = -1;
		QRegularExpression regexp("(.\\/.)");

		QRegularExpressionMatch indexOfPathSeparatorMatch = regexp.match(workingFileName);

		if (indexOfPathSeparatorMatch.hasMatch() && (indexOfPathSeparatorMatch.capturedStart() + 1) != workingFileName.length()){
			indexOfPathSeparator = indexOfPathSeparatorMatch.capturedStart() + 1;
		}

		if (indexOfPathSeparator > 0){

			if (QFileInfo(homeDirPath + workingFileName.mid(0,indexOfPathSeparator)).isFile()){
				workingFileName = workingFileName.remove(0, indexOfPathSeparator);
			}
		}
	}

	if (!homeDirPath.endsWith('/')){
		homeDirPath.append('/');
	}

	QString destinationEntryPath = homeDirPath + workingFileName;
	QFileInfo destinationEntry(destinationEntryPath);

	if (!destinationEntry.isFile()){
		return false;
	}


	QString destinationFileAbsoluteFilePath = destinationEntry.absoluteFilePath();
	if (homeDirPath == ":"){
		destinationFileAbsoluteFilePath = destinationEntryPath;
	}

	if (destinationFileAbsoluteFilePath.endsWith('/')){
		destinationFileAbsoluteFilePath.chop(1);
	}

	QFile destinationFile(destinationFileAbsoluteFilePath);
	if (!destinationFile.open(QFile::ReadOnly)){
		return false;
	}

	data = destinationFile.readAll();
	destinationFile.close();

	return true;
}


} // namespace imtfile


