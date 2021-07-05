#pragma once

#include "imtfile/CFileProviderComp.h"


namespace imtfile
{

bool CFileProviderComp::LoadData(QByteArray& data, const QByteArray& name) const
{
	QByteArray _name = name;
	QString homeDirPath = *this->m_homeDirPath;

	if ( m_fileTemplatePathCompPtr.IsValid() && m_fileTemplatePathCompPtr.GetPtr()->GetPath().length()){
		homeDirPath = m_fileTemplatePathCompPtr.GetPtr()->GetPath();
	}

	if (*m_pathsProblemsAutoSolve){

		int indexOfPathSeparator = -1;
		QRegularExpression regexp("(.\\/.)");

		QRegularExpressionMatch indexOfPathSeparatorMatch = regexp.match(_name);

		if (indexOfPathSeparatorMatch.hasMatch() && (indexOfPathSeparatorMatch.capturedStart() + 1) != _name.length()){
			indexOfPathSeparator = indexOfPathSeparatorMatch.capturedStart() + 1;
		}

		if (indexOfPathSeparator > 0){

			if (QFileInfo(homeDirPath + _name.mid(0,indexOfPathSeparator)).isFile()){
				_name = _name.remove(0, indexOfPathSeparator);

			}
		}

	}

	if (!homeDirPath.endsWith('/')){
		homeDirPath.append('/');
	}

	QString destinationEntryPath = homeDirPath + _name;
	QFileInfo destinationEntry(destinationEntryPath);

	if (!destinationEntry.isFile()){
		return false;
	}

	else {
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
	}

	return true;
}


} // namespace imtfile


