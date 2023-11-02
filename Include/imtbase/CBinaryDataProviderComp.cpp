#include <imtbase/CBinaryDataProviderComp.h>


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>


namespace imtbase
{


// reimplemented (IBinaryDataProvider)

bool CBinaryDataProviderComp::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 readFromPosition,
			qint64 readMaxLength) const
{
	Q_ASSERT(readMaxLength != 0);
	if (readFromPosition < 0){
		I_CRITICAL();
		return false;
	}

	QByteArray workingFileName = dataId;
	QString homeDirPath;

	if (m_homeDirPathAttrPtr.IsValid()){
		homeDirPath = m_homeDirPathAttrPtr->GetPath();
	}

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
	if (destinationEntry.isDir()){
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
		while (!destinationFile.open(QFile::ReadOnly)){
			QString fileName = destinationEntry.fileName();
			QString filePath = destinationEntry.filePath();
			QStringList listPath = filePath.split("/");
			if (listPath.last() == fileName){
				listPath.removeLast();
			}
			if(listPath.size() > 1){
				listPath.removeLast();
				fileName = listPath.join("/") + "/" + fileName;
				destinationFile.setFileName(fileName);
				destinationEntry.setFile(fileName);
			}
			else{
				return false;
			}
		}

		if (!destinationFile.seek(readFromPosition)){
			SendErrorMessage(0, QString("Unable to seek in file. File: '%1', pos: %2").arg(destinationFileAbsoluteFilePath, QString::number(readFromPosition)));
			destinationFile.errorString();

			return false;
		}

		qint64 readLen = readMaxLength;
		if (readLen < 0){
			readLen = std::numeric_limits<qint64>().max();
		}

		data = destinationFile.read(readLen);
		destinationFile.close();
	}

	return true;
}


} // namespace imtbase


