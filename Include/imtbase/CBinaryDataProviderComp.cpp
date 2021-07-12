#include <imtbase/CBinaryDataProviderComp.h>

// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>


namespace imtbase
{


// reimplemented (IBinaryDataProvider)

bool CBinaryDataProviderComp::GetData(QByteArray& data, const QByteArray& dataId) const
{
	QByteArray workingFileName = dataId;
	QString homeDirPath = *m_homeDirPathAttrPtr;

	if (m_fileTemplatePathCompPtr.IsValid() && m_fileTemplatePathCompPtr->GetPath().length()){
		homeDirPath = m_fileTemplatePathCompPtr->GetPath();
	}

	if (*m_pathsProblemsAutoSolveAttrPtr){
		int indexOfPathSeparator = -1;
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


} // namespace imtbase


