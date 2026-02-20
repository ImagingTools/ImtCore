// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSimpleFileJoinerComp.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QThread>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IOptionsList.h>
#include <iprm/IEnableableParam.h>
#include <ifile/IFileNameParam.h>


namespace imtsdl
{


// public static variables
const QByteArray CSimpleFileJoinerComp:: s_sourceDirPathParamId = QByteArrayLiteral("SourceDirPath");
const QByteArray CSimpleFileJoinerComp:: s_targetFilePathParamId = QByteArrayLiteral("TargetFilePath");
const QByteArray CSimpleFileJoinerComp:: s_appendModeParamId = QByteArrayLiteral("Append");


// private static variables
const QString CSimpleFileJoinerComp::s_filePathVariable = QStringLiteral("$(FilePath)");


// public methods

// reimplemented (iproc::IProcessor)

iproc::IProcessor::TaskState CSimpleFileJoinerComp::DoProcessing(
		const iprm::IParamsSet* paramsPtr,
		const istd::IPolymorphic* inputPtr,
		istd::IChangeable* /*outputPtr*/,
		ibase::IProgressManager* /*progressManagerPtr*/)
{
	if (paramsPtr == nullptr){
		SendErrorMessage(0, QString("Input params is required"));

		return TS_INVALID;
	}

	// ensure all required params is set
	iprm::TParamsPtr<ifile::IFileNameParam> sourceDirPathParamPtr(paramsPtr, s_sourceDirPathParamId, true);
	if (!sourceDirPathParamPtr.IsValid()){
		SendErrorMessage(0, QString("Source directory is not set"));

		return TS_INVALID;
	}
	QString sourceDirPath = sourceDirPathParamPtr->GetPath();

	iprm::TParamsPtr<ifile::IFileNameParam> targetFilePathParamPtr(paramsPtr, s_targetFilePathParamId, true);
	if (!targetFilePathParamPtr.IsValid()){
		SendErrorMessage(0, QString("Target file path is not set"));

		return TS_INVALID;
	}
	QString targetFilePath = targetFilePathParamPtr->GetPath();

	QDir sourceDir(sourceDirPath);
	if (!sourceDir.exists()){
		SendErrorMessage(0, QStringLiteral("Source directory does not exist: '%1'").arg(sourceDirPath));

		return TS_INVALID;
	}

	auto filesToJoinListPtr = dynamic_cast<const iprm::IOptionsList*>(inputPtr);
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

	// init temp workspace
	QIODevice::OpenMode openFlags = QIODevice::WriteOnly;
	if (appendDataToFile){
		openFlags.setFlag(QIODevice::Append);
	}

	QFile targetFile(targetFilePath);
	int attempts = 3;

	// A potential (in Windows OS) error may occur if the file system processes the file deletion/closing operation at an inopportune time. Therefore, it is required to wait until the operating system has finished processing all relevant events before continuing.
	bool isOpen = false;
	do {
		isOpen = targetFile.open(openFlags);
		if (!isOpen){
			--attempts;
			SendWarningMessage(0, QString("Unable to open target file: '%1'. Error: %2. Retrying... Remained: %3").arg(targetFilePath, targetFile.errorString(), QString::number(attempts)));
			QThread::sleep(1);
		}
	} while (attempts > 0 && !isOpen);

	if (!targetFile.isOpen()){
		SendErrorMessage(0, QString("Unable to open target file: '%1'. Error: %2.").arg(targetFilePath, targetFile.errorString()));

		return TS_INVALID;
	}

	QTextStream fileStream(&targetFile);
	// get header text
	QString headText;
	if (m_filePartSeparatorTextCompPtr.IsValid()){
		headText = m_filePartSeparatorTextCompPtr->GetText();
	}

	// process files
	SendVerboseMessage(QString("Joining files. Output: '%1'").arg(targetFile.fileName()));
	int filesCount = filesToJoinListPtr->GetOptionsCount();
	for (int fileNameIndex = 0; fileNameIndex < filesCount; ++fileNameIndex){
		const QString currentRelativePath = filesToJoinListPtr->GetOptionName(fileNameIndex);
		const QString currentFilePath = sourceDir.absoluteFilePath(currentRelativePath);
		SendVerboseMessage(QString("Joining files. [%1]'%2'").arg(QString::number(fileNameIndex), currentFilePath));
		QFile currentFile(currentFilePath);
		if (!currentFile.open(QIODevice::ReadOnly)){
			SendErrorMessage(0, QString("Unable to open processing file: '%1'. Error: %2").arg(currentFilePath, currentFile.errorString()));

			return TS_INVALID;
		}

		if (!headText.isEmpty()){
			QString currentFileHeadText(headText);
			currentFileHeadText.replace(s_filePathVariable, currentRelativePath);
			fileStream << currentFileHeadText;
			fileStream << Qt::endl << Qt::endl;
		}

		const QByteArray readData = currentFile.readAll();
		fileStream << readData;

		for (int i = 0; i < *m_emptyStringsAtEndAttrPtr; ++i){
			fileStream << Qt::endl;
		}

		// write data and clear buffer
		fileStream.flush();
	}

	// close file
	targetFile.close();

	return TS_OK;
}


} // namespace imtsdl


