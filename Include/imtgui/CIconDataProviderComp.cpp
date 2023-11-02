#include <imtgui/CIconDataProviderComp.h>


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtDebug>


namespace imtgui
{


// public methods

// reimplemented (IBinaryDataProvider)

bool CIconDataProviderComp::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 /*readFromPosition*/,
			qint64 /*readMaxLength*/) const
{
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

	if (!destinationEntry.suffix().isEmpty() && destinationEntry.suffix() != "svg"){
		return false;
	}

	if (destinationEntry.isDir()){
		return false;
	}
	else{
		QString destinationFileAbsoluteFilePath = destinationEntry.absoluteFilePath();

		if (homeDirPath == ":"){
			destinationFileAbsoluteFilePath = destinationEntryPath;
		}

		if (destinationFileAbsoluteFilePath.endsWith('/')){
			destinationFileAbsoluteFilePath.chop(1);
		}

		QFile destinationFile(destinationFileAbsoluteFilePath);
		if (!destinationFile.open(QFile::ReadOnly)){
			if (destinationEntry.suffix().isEmpty()){
				destinationFileAbsoluteFilePath += ".svg";
			}

			if (destinationEntry.suffix() == "svg"){
				destinationFileAbsoluteFilePath = destinationFileAbsoluteFilePath.replace(".svg", "");
			}

			destinationFile.setFileName(destinationFileAbsoluteFilePath);
			if (!destinationFile.open(QFile::ReadOnly)){
				QString baseName = destinationEntry.baseName();
				if (baseName.contains('_')){
					QStringList data = baseName.split('_');
					if (data.size() >= 3){
						QString newName = data[0];
						QStringList fileData = destinationFileAbsoluteFilePath.split('/');

						fileData.removeLast();
						fileData << newName;

						destinationFileAbsoluteFilePath = fileData.join('/');

						destinationFile.setFileName(destinationFileAbsoluteFilePath);
						if (!destinationFile.open(QFile::ReadOnly)){
							return false;
						}
					}
				}
			}
		}

		data = destinationFile.readAll();
		destinationFile.close();
	}

	return true;
}


} // namespace imtgui


