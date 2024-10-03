#include <imtsdl/CFileCleanupProcessorComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDir>
#include <QtCore/QLockFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IOptionsList.h>
#include <ifile/IFileNameParam.h>


namespace imtsdl
{


// public static variables
const QByteArray CFileCleanupProcessorComp:: s_sourceFilePathParamId = QByteArrayLiteral("SourceFilePath");
const QByteArray CFileCleanupProcessorComp:: s_targetFilePathParamId = QByteArrayLiteral("TargetFilePath");


// private static variables
const QString CFileCleanupProcessorComp::s_filePathVariable = QStringLiteral("$(FilePath)");


// public methods

// reimplemented (iproc::IProcessor)

int CFileCleanupProcessorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	if (paramsPtr == nullptr){
		SendErrorMessage(0, QString("Input params is required"));

		return TS_INVALID;
	}

	QString sourceFilePath;
	QString targetFilePath;

	// ensure all required params is set
	iprm::TParamsPtr<ifile::IFileNameParam> sourceDirPathParamPtr(paramsPtr, s_sourceFilePathParamId, true);
	if (!sourceDirPathParamPtr.IsValid()){
		SendErrorMessage(0, QString("Source directory is not set"));

		return TS_INVALID;
	}
	sourceFilePath = sourceDirPathParamPtr->GetPath();

	iprm::TParamsPtr<ifile::IFileNameParam> targetFilePathParamPtr(paramsPtr, s_targetFilePathParamId, false);
	if (targetFilePathParamPtr.IsValid()){
		targetFilePath = targetFilePathParamPtr->GetPath();
	}
	else {
		targetFilePath = sourceFilePath;
	}

	const iprm::IOptionsList* regExpListParamPtr = dynamic_cast<const iprm::IOptionsList*>(inputPtr);
	if (regExpListParamPtr == nullptr){
		SendErrorMessage(0, QString("Unexpected input param type"));

		return TS_INVALID;
	}
	const int regExpCount = regExpListParamPtr->GetOptionsCount();
	if (regExpCount <= 0){
		SendWarningMessage(0, "Regular expressnions is missing. Nothing todo");

		return TS_OK;
	}
	QList<QRegularExpression> regExpList;
	for (int regExpIndex = 0; regExpIndex < regExpCount; ++regExpIndex){
		const QString regExpPattern = regExpListParamPtr->GetOptionName(regExpIndex);
		Q_ASSERT_X(!regExpPattern.isEmpty(), "processing input ptr", "RegExp pattern is empty!");

		QRegularExpression regExp(regExpPattern);
		regExpList << regExp;
	}

	// create target file
	QScopedPointer<QFile> targetFilePtr;
	if (targetFilePath != sourceFilePath){
		targetFilePtr.reset(new QFile(targetFilePath));
	}
	else {
		targetFilePtr.reset(new QTemporaryFile);
	}
	Q_ASSERT(!targetFilePtr.isNull());


	// initalize
	QFileInfo targetFileInfo(targetFilePath);
	const QString targetFileDirPath = targetFileInfo.absoluteDir().absolutePath();
	if (!istd::CSystem::EnsurePathExists(targetFileDirPath)){
		SendErrorMessage(0, QString("Unable to create directory: '%1'").arg(targetFileDirPath));

		return TS_INVALID;
	}

	if (!targetFilePtr->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
		SendErrorMessage(0, QString("Unable to open file: '%1'. Error: %2").arg(targetFilePath, targetFilePtr->errorString()));

		return TS_INVALID;
	}

	QFile sourceFile(sourceFilePath);
	if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("Unable to open file: '%1'. Error: %2").arg(sourceFilePath, sourceFile.errorString()));

		return TS_INVALID;
	}

	QTextStream inputFileStream(&sourceFile);
	QTextStream outputFileStream(targetFilePtr.data());

	QSet<QString> foundLines;
	// process file
	while (!inputFileStream.atEnd()){
		const QString line = inputFileStream.readLine();
		// we already have a line. Do not write duplicate
		if (foundLines.contains(line)){
			continue;
		}

		// Save the line that matches the expressions in order to avoid writing it again
		for (const QRegularExpression& regExp: regExpList){
			if (regExp.match(line).hasMatch()){
				foundLines << line;

				break;
			}
		}

		outputFileStream << line << Qt::endl;
	}

	// finalize
	sourceFile.close();
	targetFilePtr->close();
	sourceFile.remove();
	if (targetFilePath == sourceFilePath && sourceFile.fileName() != targetFilePtr->fileName()){
		SendVerboseMessage(QString("move file: '%1' to '%2'").arg(targetFilePtr->fileName(), sourceFilePath));
		if (!QFile::rename(targetFilePtr->fileName(), sourceFilePath)){
			SendErrorMessage(0, QString("Unable to move file: '%1' to '%2'").arg(targetFilePtr->fileName(), sourceFilePath));

			return TS_INVALID;
		}
	}

	return TS_OK;
}


} // namespace imtsdl


