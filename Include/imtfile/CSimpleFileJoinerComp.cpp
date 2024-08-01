#include <imtfile/CSimpleFileJoinerComp.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QLockFile>

// Acf includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CTempDir.h>


namespace imtfile
{


// public static variables
const QByteArray CSimpleFileJoinerComp:: s_sourceDirPathParamId = QByteArrayLiteral("SourceDirPath");
const QByteArray CSimpleFileJoinerComp:: s_targetFilePathParamId = QByteArrayLiteral("TargetFilePath");


// private static variables
const QString CSimpleFileJoinerComp::s_filePathVariable = QStringLiteral("$(FilePath)");


// public methods

// reimplemented (iproc::IProcessor)

int CSimpleFileJoinerComp::DoProcessing(
		const iprm::IParamsSet* paramsPtr,
		const istd::IPolymorphic* inputPtr,
		istd::IChangeable* /*outputPtr*/,
		ibase::IProgressManager* /*progressManagerPtr*/)
{
	if (paramsPtr == nullptr){
		SendErrorMessage(0, QString("Input params is required"));

		return TS_INVALID;
	}

	QString sourceDirPath;
	QString targetFilePath;

	// ensure all required params is set
	iprm::TParamsPtr<ifile::IFileNameParam> sourceDirPathParamPtr(paramsPtr, s_sourceDirPathParamId, true);
	if (!sourceDirPathParamPtr.IsValid()){
		SendErrorMessage(0, QString("Source directory is not set"));

		return TS_INVALID;
	}
	sourceDirPath = sourceDirPathParamPtr->GetPath();

	iprm::TParamsPtr<ifile::IFileNameParam> targetFilePathParamPtr(paramsPtr, s_targetFilePathParamId, true);
	if (!targetFilePathParamPtr.IsValid()){
		SendErrorMessage(0, QString("Target file path is not set"));

		return TS_INVALID;
	}
	targetFilePath = targetFilePathParamPtr->GetPath();

	QDir sourceDir(sourceDirPath);
	if (!sourceDir.exists()){
		SendErrorMessage(0, QString("Source directory is not exists: '%1'").arg(sourceDirPath));

		return TS_INVALID;
	}

	QFileInfo targetFileInfo(targetFilePath);
	const QString targetFileDirPath = targetFileInfo.absoluteDir().absolutePath();
	if (!istd::CSystem::EnsurePathExists(targetFileDirPath)){
		SendErrorMessage(0, QString("Unable to create directory: '%1'").arg(targetFileDirPath));

		return TS_INVALID;
	}

	if (targetFileInfo.isFile() && targetFileInfo.exists()){
		if (!QFile::remove(targetFilePath)){
			SendErrorMessage(0, QString("Unable to remove target file: '%1'").arg(targetFilePath));

			return TS_INVALID;
		}
	}

	// lock target file to ensure it will be created
	QLockFile targetLockFile(targetFilePath);
	if (!targetLockFile.tryLock(std::chrono::seconds(3))){
		SendErrorMessage(0, QString("Unable to lock target file: '%1'").arg(targetFilePath));

		return TS_INVALID;
	}

	// init temp-working dir
	imtbase::CTempDir tempDir;
	QFile tempJoinedFile(tempDir.Path() + targetFileInfo.fileName());
	if (!tempJoinedFile.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open temp file: '%1'").arg(tempJoinedFile.fileName()));

		return TS_INVALID;
	}

	QTextStream tempJoinedFileStream(&tempJoinedFile);

	// get header text
	QString headText;
	if (m_filePartSeparatorTextCompPtr.IsValid()){
		headText = m_filePartSeparatorTextCompPtr->GetText();
	}

	// setup filters
	QStringList fileFilters = {"*"};
	const iprm::ITextParam* singleFilterParamPtr = dynamic_cast<const iprm::ITextParam*>(inputPtr);
	if (singleFilterParamPtr != nullptr){
		fileFilters = {singleFilterParamPtr->GetText()};
	}

	const iprm::IParamsSet* fileFiltersParamsPtr = dynamic_cast<const iprm::IParamsSet*>(inputPtr);
	if (fileFiltersParamsPtr != nullptr && !fileFiltersParamsPtr->GetParamIds().isEmpty()){
		fileFilters.clear();
		const iprm::IParamsSet::Ids paramIdList = fileFiltersParamsPtr->GetParamIds();
		for (const QByteArray& paramId: paramIdList){
			iprm::TParamsPtr<iprm::ITextParam> filterParamPtr(fileFiltersParamsPtr, paramId, true);
			if (!filterParamPtr.IsValid()){
				SendErrorMessage(0, "Unexpected filter param");

				return TS_INVALID;
			}
			fileFilters << filterParamPtr->GetText();
		}
	}

	// process files
	QDirIterator sourceDirIterator(sourceDir.absolutePath(), fileFilters, QDir::Files | QDir::Readable, QDirIterator::Subdirectories);
	while(sourceDirIterator.hasNext()){
		const QString currentFilePath = sourceDirIterator.next();

		// skip target file
		QFileInfo currentFileInfo(currentFilePath);
		QString targetFileInfoCanonicalFilePath = targetFileInfo.canonicalFilePath();
		QString currentFileInfoCanonicalFilePath = currentFileInfo.canonicalFilePath();
		if (targetFileInfoCanonicalFilePath == currentFileInfoCanonicalFilePath){
			SendVerboseMessage(QString("Skipping '%1'. It is target: '%2'").arg(currentFilePath, targetFilePath));

			continue;
		}

		SendVerboseMessage(QString("processing '%1'. Target: '%2'").arg(currentFileInfoCanonicalFilePath, targetFileInfoCanonicalFilePath));

		const QString currentRelativePath = sourceDir.relativeFilePath(currentFilePath);
		QFile currentFile(currentFilePath);
		if (!currentFile.open(QIODevice::ReadOnly)){
			SendErrorMessage(0, QString("Unable to open processing file: '%1'").arg(currentFilePath));

			return TS_INVALID;
		}

		if (!headText.isEmpty()){
			QString currentFileHeadText(headText);
			currentFileHeadText.replace(s_filePathVariable, currentRelativePath);
			tempJoinedFileStream << currentFileHeadText;
			tempJoinedFileStream << Qt::endl << Qt::endl;
		}

		const QByteArray readData = currentFile.readAll();
		tempJoinedFileStream << readData;

		for (int i = 0; i < *m_emptyStringsAtEndAttrPtr; ++i){
			tempJoinedFileStream << Qt::endl;
		}

		// write data and cleat buffer
		tempJoinedFileStream.flush();
	}

	// close file and unlock target file
	tempJoinedFile.close();
	targetLockFile.unlock();

	// move temp file to target path
	if (!tempJoinedFile.rename(targetFilePath)){
		SendErrorMessage(0, QString("Unable to move target file: from '%1' to '%2'").arg(tempJoinedFile.fileName(), targetFilePath));

		return TS_INVALID;
	}

	return TS_OK;
}


} // namespace imtfile


