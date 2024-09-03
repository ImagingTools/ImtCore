#include <imtsdl/CQmldirFilePersistenceComp.h>


// Qt includes
#include <QtCore/QTextStream>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/CParamsSet.h>
#include <iprm/CNameParam.h>
#include <iprm/CEnableableParam.h>


namespace imtsdl
{


// public static veriables
const QByteArray CQmldirFilePersistenceComp::s_moduleNameParamId =			QByteArrayLiteral("ModuleName");
const QByteArray CQmldirFilePersistenceComp::s_objectsParamId =				QByteArrayLiteral("Objects");
const QByteArray CQmldirFilePersistenceComp::s_objectIsSingletonParamId =	QByteArrayLiteral("IsSingleton");
const QByteArray CQmldirFilePersistenceComp::s_objectTypeNameParamId =		QByteArrayLiteral("TypeName");
const QByteArray CQmldirFilePersistenceComp::s_objectVersionNameParamId =	QByteArrayLiteral("VersionName");
const QByteArray CQmldirFilePersistenceComp::s_objectFileNameParamId =		QByteArrayLiteral("FileName");

// public methods


// reimplemented (ifile::IFilePersistence)

bool CQmldirFilePersistenceComp::IsOperationSupported(
	const istd::IChangeable* dataObjectPtr,
	const QString* filePathPtr,
	int flags,
	bool beQuiet) const
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


int CQmldirFilePersistenceComp::LoadFromFile(
	istd::IChangeable& data,
	const QString& filePath,
	ibase::IProgressManager* /*progressManagerPtr*/) const
{
	iprm::CParamsSet* paramsSetPtr = nullptr;

	try {
		iprm::CParamsSet& paramsSetRef = dynamic_cast<iprm::CParamsSet&>(data);
		paramsSetPtr = &paramsSetRef;
	}
	catch (std::bad_cast& ex){
		SendCriticalMessage(0, "Unexpected input data", __func__);
		I_CRITICAL();

		return false;
	}
	Q_ASSERT(paramsSetPtr != nullptr);

	QFile loadFile(filePath);
	if (!loadFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(loadFile.fileName()), __func__);

		return OS_FAILED;
	}

	QTextStream fileStream(&loadFile);

	istd::TDelPtr<iprm::IParamsManager> objectEntriesManagerPtr(m_paramsManagerFactComp.CreateInstance());
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
			paramsSetPtr->SetEditableParameter(s_moduleNameParamId, moduleNameParamPtr, true);
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
			qmlObjectParams.SetEditableParameter(s_objectIsSingletonParamId, &isSingletonParam);

			// get object's info
			// a) type
			iprm::CNameParam objectNameParam;
			objectNameParam.SetName(capturedTexts[0]);
			qmlObjectParams.SetEditableParameter(s_objectTypeNameParamId, &objectNameParam);
			// b) version
			iprm::CNameParam objectVerionNameParam;
			objectVerionNameParam.SetName(capturedTexts[1]);
			qmlObjectParams.SetEditableParameter(s_objectVersionNameParamId, &objectVerionNameParam);
			// c) file
			iprm::CNameParam objectFileNameParam;
			objectFileNameParam.SetName(capturedTexts[2]);
			qmlObjectParams.SetEditableParameter(s_objectFileNameParamId, &objectFileNameParam);

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
	if (objectEntriesManagerPtr->GetParamsSetsCount() > 0){
		paramsSetPtr->SetEditableParameter(s_objectsParamId, objectEntriesManagerPtr.PopPtr(), true);
	}

	return OS_OK;
}


// reimplemented (ifile::IFileTypeInfo)

int CQmldirFilePersistenceComp::SaveToFile(
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
		SendCriticalMessage(0, "Unexpected input data", __func__);
		I_CRITICAL();

		return false;
	}
	Q_ASSERT(paramsSetPtr != nullptr);

	QFile saveFile(filePath);
	if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(saveFile.fileName()), __func__);

		return OS_FAILED;
	}

	QTextStream fileStream(&saveFile);

	// module name
	iprm::TParamsPtr<iprm::INameParam> moduleNameParam(paramsSetPtr, s_moduleNameParamId);
	if (!moduleNameParam.IsValid()){
		SendErrorMessage(0, QString("Name param '%1' is missing").arg(s_moduleNameParamId));
		I_CRITICAL();

		return OS_FAILED;
	}
	fileStream << QStringLiteral("module ") << moduleNameParam->GetName();
	fileStream << Qt::endl << Qt::endl;

	// QML objects
	iprm::TParamsPtr<iprm::IParamsManager> objectEntriesManagerPtr(paramsSetPtr, s_objectsParamId, false);
	if (objectEntriesManagerPtr.IsValid() && objectEntriesManagerPtr->GetParamsSetsCount() > 0){
		int objectsCount = objectEntriesManagerPtr->GetParamsSetsCount();
		for (int objectEntryIndex = 0; objectEntryIndex > objectsCount; ++objectEntryIndex){
			iprm::IParamsSet* objectEntryParamsSetPtr = objectEntriesManagerPtr->GetParamsSet(objectEntryIndex);
			if (objectEntryParamsSetPtr == nullptr){
				SendCriticalMessage(0, "Unable to create params set for entry", __func__);
				I_CRITICAL();

				return OS_FAILED;
			}


		}
	}

	return OS_OK;
}


bool CQmldirFilePersistenceComp::GetFileExtensions(
	QStringList& result,
	const istd::IChangeable* dataObjectPtr,
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


} // namespace imtsdl

