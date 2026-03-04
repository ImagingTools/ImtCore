// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgenqml/CQmldirFilePersistenceComp.h>


// Qt includes
#include <QtCore/QTextStream>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/CParamsSet.h>
#include <iprm/CNameParam.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtsdlgenqml/imtsdlgenqml.h>


namespace imtsdlgenqml
{


// public methods

// reimplemented (ifile::IFilePersistence)

bool CQmldirFilePersistenceComp::IsOperationSupported(
	const istd::IChangeable* dataObjectPtr,
	const QString* filePathPtr,
	int flags,
	bool /*beQuiet*/) const
{
	if (filePathPtr != nullptr && !filePathPtr->isEmpty()){
		if (flags & QF_LOAD && !QFile(*filePathPtr).isReadable()){
			return false;
		}
	}

	if (!(flags & (QF_LOAD | QF_SAVE))){
		return false;
	}

	const iprm::CParamsSet* paramsSetPtr =  dynamic_cast<const iprm::CParamsSet*>(dataObjectPtr);
	if (paramsSetPtr == nullptr){
		return false;
	}

	return true;
}


ifile::IFilePersistence::OperationState CQmldirFilePersistenceComp::LoadFromFile(
	istd::IChangeable& data,
	const QString& filePath,
	ibase::IProgressManager* /*progressManagerPtr*/) const
{
	iprm::CParamsSet* paramsSetPtr = nullptr;

	try {
		iprm::CParamsSet& paramsSetRef = dynamic_cast<iprm::CParamsSet&>(data);
		paramsSetPtr = &paramsSetRef;
	}
	catch (std::bad_cast&){
		SendCriticalMessage(0, "Unexpected input data", __func__);
		I_CRITICAL();

		return OS_FAILED;
	}
	Q_ASSERT(paramsSetPtr != nullptr);

	QFile loadFile(filePath);
	if (!loadFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(loadFile.fileName()), __func__);

		return OS_FAILED;
	}

	QTextStream fileStream(&loadFile);

	iprm::IParamsManagerUniquePtr objectEntriesManagerPtr(m_paramsManagerFactComp.CreateInstance());
	quint64 lastReadLine = 0;
	while(!fileStream.atEnd()){
		QString readLine = fileStream.readLine();
		++lastReadLine;

		// module name
		static QRegularExpression moduleNameRegExp("^\\s*module (\\S+)");
		QRegularExpressionMatch moduleNameMatch = moduleNameRegExp.match(readLine);
		if (moduleNameMatch.hasMatch()){
			const QString moduleName = moduleNameMatch.capturedTexts().constLast();
			iprm::CNameParam* moduleNameParamPtr = new iprm::CNameParam;
			moduleNameParamPtr->SetName(moduleName);
			paramsSetPtr->SetEditableParameter(QmldirModelParamIds::ModuleName, moduleNameParamPtr, true);
		}

		// QML Object
		static QRegularExpression qmlObjectRegExp("^\\s*(?:singleton\\s+){0,1}(\\S+)\\s+(\\S+)\\s+(\\S+)");
		QRegularExpressionMatch qmlObjectRegExpMatch = qmlObjectRegExp.match(readLine);
		if (qmlObjectRegExpMatch.hasMatch()){
			// collect all object info
			QStringList capturedTexts = qmlObjectRegExpMatch.capturedTexts();

			// remove first, because the list includes the implicit capturing group number 0, capturing the substring matched by the entire pattern.
			capturedTexts.removeFirst();

			// ensure that the entry is valid schema of entry: [singleton] <TypeName> <VersionName> <FileName>(relative path)
			if (capturedTexts.size() < 3){
				SendWarningMessage(0, QString("Unexpected string! '%1' at %2. It will be ignored.").arg(readLine, QString::number(lastReadLine)));

				continue;
			}

			// create paramsset to store object
			iprm::CParamsSet qmlObjectParams;

			// determine whether an object is a singleton
			static QRegularExpression singletonRegExp("^\\s*(singleton)");
			bool isSingleton = singletonRegExp.match(readLine).hasMatch();
			iprm::CEnableableParam isSingletonParam(isSingleton);
			qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectIsSingleton, &isSingletonParam);

			// get object's info
			// a) type
			iprm::CNameParam objectNameParam;
			objectNameParam.SetName(capturedTexts[0]);
			qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectTypeName, &objectNameParam);
			// b) version
			iprm::CNameParam objectVerionNameParam;
			objectVerionNameParam.SetName(capturedTexts[1]);
			qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectVersionName, &objectVerionNameParam);
			// c) file
			iprm::CNameParam objectFileNameParam;
			objectFileNameParam.SetName(capturedTexts[2]);
			qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectFileName, &objectFileNameParam);

			// insert params set to object entries manager
			int indexOfInsertedSet = objectEntriesManagerPtr->InsertParamsSet();
			iprm::IParamsSet* objectEntryParamsSetPtr = objectEntriesManagerPtr->GetParamsSet(indexOfInsertedSet);
			if (objectEntryParamsSetPtr == nullptr){
				SendCriticalMessage(0, "Unable to create params set for entry", __func__);
				I_CRITICAL();

				return OS_FAILED;
			}
			const bool isCopied = objectEntryParamsSetPtr->CopyFrom(qmlObjectParams);
			if (!isCopied){
				SendCriticalMessage(0, "Unable to set params set for entry", __func__);
				I_CRITICAL();

				return OS_FAILED;
			}
		}
	}

	iser::ISerializableUniquePtr objectParamPtr;
	objectParamPtr.MoveCastedPtr<iprm::IParamsManager>(objectEntriesManagerPtr);

	paramsSetPtr->SetEditableParameter(QmldirModelParamIds::Objects, objectParamPtr);

	return OS_OK;
}


// reimplemented (ifile::IFileTypeInfo)

ifile::IFilePersistence::OperationState CQmldirFilePersistenceComp::SaveToFile(
	const istd::IChangeable& data,
	const QString& filePath,
	ibase::IProgressManager* /*progressManagerPtr*/) const
{

	const iprm::CParamsSet* paramsSetPtr = nullptr;

	try {
		const iprm::CParamsSet& paramsSetRef = dynamic_cast<const iprm::CParamsSet&>(data);
		paramsSetPtr = &paramsSetRef;
	}
	catch (std::bad_cast& ex){
		SendCriticalMessage(0, QString("Unexpected input data. Error: %1").arg(ex.what()), __func__);
		I_CRITICAL();

		return OS_FAILED;
	}

	Q_ASSERT(paramsSetPtr != nullptr);

	QFile saveFile(filePath);
	if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(saveFile.fileName()), __func__);

		return OS_FAILED;
	}

	QTextStream fileStream(&saveFile);

	// module name
	iprm::TParamsPtr<iprm::INameParam> moduleNameParam(paramsSetPtr, QmldirModelParamIds::ModuleName);
	if (!moduleNameParam.IsValid()){
		SendErrorMessage(0, QString("Name param '%1' is missing").arg(qPrintable(QmldirModelParamIds::ModuleName)));
		I_CRITICAL();

		return OS_FAILED;
	}
	fileStream << QStringLiteral("module ") << moduleNameParam->GetName();
	fileStream << Qt::endl << Qt::endl;

	// QML objects
	iprm::TParamsPtr<iprm::IParamsManager> objectEntriesManagerPtr(paramsSetPtr, QmldirModelParamIds::Objects, false);
	if (objectEntriesManagerPtr.IsValid() && objectEntriesManagerPtr->GetParamsSetsCount() > 0){
		int objectsCount = objectEntriesManagerPtr->GetParamsSetsCount();
		for (int objectEntryIndex = 0; objectEntryIndex < objectsCount; ++objectEntryIndex){
			iprm::IParamsSet* objectEntryParamsSetPtr = objectEntriesManagerPtr->GetParamsSet(objectEntryIndex);
			if (objectEntryParamsSetPtr == nullptr){
				SendCriticalMessage(0, "Unable to get params set for entry", __func__);
				I_CRITICAL();

				return OS_FAILED;
			}

			// determine whether an object is a singleton
			iprm::TParamsPtr<iprm::IEnableableParam> singletonParamPtr(objectEntryParamsSetPtr, QmldirModelParamIds::ObjectIsSingleton, false);
			if (singletonParamPtr.IsValid()){
				bool isSingleton = singletonParamPtr->IsEnabled();
				// write singleton directive for entry
				if (isSingleton){
					fileStream << QStringLiteral("singleton ");
				}
			}

			// write object's info
			// a) type
			iprm::TParamsPtr<iprm::CNameParam> objectNameParamPtr(objectEntryParamsSetPtr, QmldirModelParamIds::ObjectTypeName, true);
			if (!objectNameParamPtr.IsValid()){
				SendErrorMessage(0, "Object name param is missing!");

				return OS_FAILED;
			}
			fileStream << objectNameParamPtr->GetName() << ' ';

			// b) version
			iprm::TParamsPtr<iprm::CNameParam> objectVerionNameParamPtr(objectEntryParamsSetPtr, QmldirModelParamIds::ObjectVersionName, true);
			if (!objectVerionNameParamPtr.IsValid()){
				SendErrorMessage(0, "Object version name param is missing!");

				return OS_FAILED;
			}
			fileStream << objectVerionNameParamPtr->GetName() << ' ';

			// c) file
			iprm::TParamsPtr<iprm::CNameParam> objectFileNameParamPtr(objectEntryParamsSetPtr, QmldirModelParamIds::ObjectFileName, true);
			if (!objectFileNameParamPtr.IsValid()){
				SendErrorMessage(0, "Object file name param is missing!");

				return OS_FAILED;
			}
			fileStream << objectFileNameParamPtr->GetName() << ' ';

			// end of object
			fileStream << Qt::endl;
		}
	}

	return OS_OK;
}


bool CQmldirFilePersistenceComp::GetFileExtensions(
	QStringList& result,
	const istd::IChangeable* /*dataObjectPtr*/,
	int /*flags*/,
	bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result << QStringLiteral("");

	return true;
}


QString CQmldirFilePersistenceComp::GetTypeDescription(const QString* extensionPtr) const
{
	if (extensionPtr != nullptr && !extensionPtr->isNull() && extensionPtr->isEmpty()){
		return QStringLiteral("Qt qmldir file");
	}

	return QString();
}


// reimplemented (ifile::IFilePersistenceInfo)

QString CQmldirFilePersistenceComp::GetLastFilePath(OperationType operationType, PathType pathType) const
{
	switch (operationType){
	case OT_SAVE:
		return GetPathForType(m_lastSaveInfo, pathType);

	default:
		return GetPathForType(m_lastOpenInfo, pathType);
	}
}


// protected methods

QString CQmldirFilePersistenceComp::GetPathForType(const QFileInfo& fileInfo, PathType pathType) const
{
	switch (pathType){
	case OT_FILENAME:
		return fileInfo.fileName();

	case OT_DIR:
		return fileInfo.absolutePath();

	default:
		return fileInfo.absoluteFilePath();
	}
}


} // namespace imtsdlgenqml

