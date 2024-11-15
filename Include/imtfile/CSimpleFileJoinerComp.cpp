#include <imtfile/CSimpleFileJoinerComp.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QLockFile>
#include <QtCore/QTemporaryDir>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IOptionsList.h>
#include <iprm/IEnableableParam.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CTempDir.h>


namespace imtfile
{


// public static variables
const QByteArray CSimpleFileJoinerComp:: s_sourceDirPathParamId = QByteArrayLiteral("SourceDirPath");
const QByteArray CSimpleFileJoinerComp:: s_targetFilePathParamId = QByteArrayLiteral("TargetFilePath");
const QByteArray CSimpleFileJoinerComp:: s_appendModeParamId = QByteArrayLiteral("Append");


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

	const iprm::IOptionsList* filesToJoinListPtr = dynamic_cast<const iprm::IOptionsList*>(inputPtr);
	if (filesToJoinListPtr == nullptr){
		SendErrorMessage(0, QString("Files to join is not set"));

		return TS_INVALID;
	}

	QFileInfo targetFileInfo(targetFilePath);
	const QString targetFileDirPath = targetFileInfo.absoluteDir().absolutePath();
	if (!istd::CSystem::EnsurePathExists(targetFileDirPath)){
		SendErrorMessage(0, QString("Unable to create directory: '%1'").arg(targetFileDirPath));

		return TS_INVALID;
	}

	// check if append is enabled
	bool appendDataToFile = false;
	iprm::TParamsPtr<iprm::IEnableableParam> appendDataToFileParamPtr(paramsPtr, s_appendModeParamId, false);
	if (appendDataToFileParamPtr.IsValid()){
		appendDataToFile = appendDataToFileParamPtr->IsEnabled();
	}

	// define temp workspace
	imtbase::CTempDir tempDir;
	QFile tempJoinedFile(tempDir.Path() + '/' + targetFileInfo.fileName());

	// save original file if append is enabled
	if (appendDataToFile){
		if (!QFile::rename(targetFilePath, tempJoinedFile.fileName())){
			SendErrorMessage(0, QString("Unable to move target file: '%1' to '%2'").arg(targetFilePath, tempJoinedFile.fileName()));

			return TS_INVALID;
		}
	}

	// remove existing file
	if (targetFileInfo.isFile() && targetFileInfo.exists()){
		if (!QFile::remove(targetFilePath)){
			SendErrorMessage(0, QString("Unable to remove target file: '%1'").arg(targetFilePath));

			return TS_INVALID;
		}
	}

	// lock target file to ensure it will be created
	QLockFile targetLockFile(targetFilePath);
	if (!targetLockFile.tryLock(3000)){
		SendErrorMessage(0, QString("Unable to lock target file: '%1'").arg(targetFilePath));

		return TS_INVALID;
	}

	// init temp workspace
	QIODevice::OpenMode openFlags = QIODevice::ReadWrite;
	if (appendDataToFile){
		openFlags.setFlag(QIODevice::Append);
	}
	if (!tempJoinedFile.open(openFlags)){
		SendErrorMessage(0, QString("Unable to open temp file: '%1'").arg(tempJoinedFile.fileName()));

		return TS_INVALID;
	}

	QTextStream tempJoinedFileStream(&tempJoinedFile);

	// get header text
	QString headText;
	if (m_filePartSeparatorTextCompPtr.IsValid()){
		headText = m_filePartSeparatorTextCompPtr->GetText();
	}

	// process files
	SendVerboseMessage(QString("Joining files. Output: '%1'").arg(tempJoinedFile.fileName()));
	int filesCount = filesToJoinListPtr->GetOptionsCount();
	for (int fileNameIndex = 0; fileNameIndex < filesCount; ++fileNameIndex){
		const QString currentRelativePath = filesToJoinListPtr->GetOptionName(fileNameIndex);
		const QString currentFilePath = sourceDir.absoluteFilePath(currentRelativePath);
		SendVerboseMessage(QString("Joining files. [%1]'%2'").arg(QString::number(fileNameIndex), currentFilePath));
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


