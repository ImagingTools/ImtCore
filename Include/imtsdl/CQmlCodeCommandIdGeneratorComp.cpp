#include "CQmlCodeCommandIdGeneratorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QXmlStreamWriter>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IParamsManager.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CNameParam.h>


// imtsdl includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlRequest.h>
#include <imtsdl/CQmldirFilePersistenceComp.h>


namespace imtsdl
{


int CQmlCodeCommandIdGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_qmlCommandIdsFileSuffixAttrPtr.IsValid());
	Q_ASSERT(m_qmldirFilePersistanceCompPtr.IsValid());
	Q_ASSERT(m_qrcFilePersistanceCompPtr.IsValid());
	Q_ASSERT(m_paramsFactComp.IsValid());

	if (!m_argumentParserCompPtr->IsQmlEnabled()){
		return TS_OK;
	}

	if (m_argumentParserCompPtr->IsDependenciesMode()){
		// nothing todo

		return TS_OK;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return TS_INVALID;
	}

	if (!istd::CSystem::EnsurePathExists(outputDirectoryPath)){
		SendErrorMessage(0, QString("Unable to create path '%1'").arg(outputDirectoryPath));
		I_CRITICAL();

		return TS_INVALID;
	}

	m_qmlCommandsFilePtr.SetPtr(new QFile(GetQmlCommandIdsFilePath()));

	if (!ProcessQmlFile()){
		SendErrorMessage(0, QString("Unable to begin files"));
		I_CRITICAL();

		return TS_INVALID;
	}

	// then update a qmldir file
	iprm::CParamsSet qmldirDataParams;
	int loadStatus = m_qmldirFilePersistanceCompPtr->LoadFromFile(qmldirDataParams, outputDirectoryPath + "/qmldir");
	if (loadStatus != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QString("Unable to load qmldir file data from '%1'").arg(outputDirectoryPath + "/qmldir"));

		return TS_INVALID;
	}

	const QString currentNamespace = m_argumentParserCompPtr->GetNamespace();
	iprm::IParamsManager* objectsParamsManagerPtr = dynamic_cast<iprm::IParamsManager*>(qmldirDataParams.GetEditableParameter(QmldirModelParamIds::Objects));
	if (objectsParamsManagerPtr == nullptr){
		SendCriticalMessage(0, "Invalid params created");
		I_CRITICAL();

		return TS_INVALID;
	}

	// create paramsset to store object
	iprm::CParamsSet qmlObjectParams;

	// set singleton
	iprm::CEnableableParam isSingletonParam(true);
	qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectIsSingleton, &isSingletonParam);

	// get object's info
	// a) type
	iprm::CNameParam objectNameParam;
	objectNameParam.SetName(GetQmlCommandIdsFileName());
	qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectTypeName, &objectNameParam);
	// b) version
	iprm::CNameParam objectVerionNameParam;
	objectVerionNameParam.SetName(QStringLiteral(" 1.0 "));
	qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectVersionName, &objectVerionNameParam);
	// c) file
	iprm::CNameParam objectFileNameParam;
	objectFileNameParam.SetName(GetQmlCommandIdsFileName() + QStringLiteral(".qml"));
	qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectFileName, &objectFileNameParam);

	int indexOfInsertedSet = objectsParamsManagerPtr->InsertParamsSet();
	iprm::IParamsSet* objectEntryParamsSetPtr = objectsParamsManagerPtr->GetParamsSet(indexOfInsertedSet);
	if (objectEntryParamsSetPtr == nullptr){
		SendCriticalMessage(0, "Unable to create params set for entry", __func__);
		I_CRITICAL();

		return TS_INVALID;
	}
	const bool isCopied = objectEntryParamsSetPtr->CopyFrom(qmlObjectParams);
	if (!isCopied){
		SendCriticalMessage(0, "Unable to set params set for entry", __func__);
		I_CRITICAL();

		return TS_INVALID;
	}

	int saveStatus = m_qmldirFilePersistanceCompPtr->SaveToFile(qmldirDataParams, outputDirectoryPath + "/qmldir");
	if (saveStatus != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QString("Unable to save qmldir file data from '%1'").arg(outputDirectoryPath + "/qmldir"));

		return TS_INVALID;
	}

	// and finally update a QRC file
	istd::TDelPtr<iprm::IParamsManager> qrcParamsPtr(m_paramsFactComp.CreateInstance());
	Q_ASSERT(qrcParamsPtr != nullptr);

	const int qrcLoadStatus = m_qrcFilePersistanceCompPtr->LoadFromFile(*qrcParamsPtr, outputDirectoryPath + '/' + currentNamespace + QStringLiteral(".qrc"));
	if (qrcLoadStatus != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QString("Unable to load QRC file data from '%1'").arg(outputDirectoryPath + '/' + currentNamespace + QStringLiteral(".qrc")));

		return TS_INVALID;
	}

	const int prefixParamsCount = qrcParamsPtr->GetParamsSetsCount();
	iprm::IParamsSet* qmlPrefixParams = nullptr;
	for (int paramIndex = 0; paramIndex< prefixParamsCount; ++paramIndex){
		iprm::IParamsSet* prefixParamsSetPtr = qrcParamsPtr->GetParamsSet(paramIndex);
		iprm::TParamsPtr<iprm::INameParam> prefixNameParamPtr(prefixParamsSetPtr, QtResourceModelParamIds::PrefixName);
		if (prefixNameParamPtr.IsValid()){
			const QString prefixName = prefixNameParamPtr->GetName();
			if (prefixName == QStringLiteral("/qml")){
				qmlPrefixParams = prefixParamsSetPtr;

				break;
			}
		}
	}
	if (qmlPrefixParams == nullptr){
		const int prefixParamsSetIndex = qrcParamsPtr->InsertParamsSet();
		qmlPrefixParams = qrcParamsPtr->GetParamsSet(prefixParamsSetIndex);
	}
	Q_ASSERT(qmlPrefixParams != nullptr);

	iprm::IParamsManager* objectsManagerPtr = dynamic_cast<iprm::IParamsManager*>(qmlPrefixParams->GetEditableParameter(QtResourceModelParamIds::QrcObjects));
	if (objectsManagerPtr == nullptr){
		SendCriticalMessage(0, QString("Params manager  '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcObjects)));
		I_CRITICAL();

		return TS_INVALID;
	}

	const int fileParamsIndex = objectsManagerPtr->InsertParamsSet();
	iprm::IParamsSet* fileParamsSet = objectsManagerPtr->GetParamsSet(fileParamsIndex);
	Q_ASSERT(fileParamsSet != nullptr);

	iprm::INameParam* fileNameParam = dynamic_cast<iprm::INameParam*>(fileParamsSet->GetEditableParameter(QtResourceModelParamIds::QrcFileName));
	if (fileNameParam == nullptr){
		SendCriticalMessage(0, QString("Name param '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcFileName)));
		I_CRITICAL();

		return TS_INVALID;
	}
	fileNameParam->SetName(GetQmlCommandIdsFileName() + QStringLiteral(".qml"));

	iprm::INameParam* aliasNameParam = dynamic_cast<iprm::INameParam*>(fileParamsSet->GetEditableParameter(QtResourceModelParamIds::QrcAlias));
	if (aliasNameParam == nullptr){
		SendCriticalMessage(0, QString("Name param '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcAlias)));
		I_CRITICAL();

		return TS_INVALID;
	}
	aliasNameParam->SetName(m_argumentParserCompPtr->GetNamespace() + '/' + GetQmlCommandIdsFileName() + QStringLiteral(".qml"));

	const int qrcSaveStatus = m_qrcFilePersistanceCompPtr->SaveToFile(*qrcParamsPtr, outputDirectoryPath + '/' + currentNamespace + QStringLiteral(".qrc"));
	if (qrcSaveStatus != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QString("Unable to save QRC file data to '%1'").arg(outputDirectoryPath + '/' + currentNamespace + QStringLiteral(".qrc")));

		return TS_INVALID;
	}


	return TS_OK;
}


// private methods

bool CQmlCodeCommandIdGeneratorComp::ReOpenFiles()
{
	if (!m_qmlCommandsFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlCommandsFilePtr->fileName(), m_qmlCommandsFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	return true;
}


bool CQmlCodeCommandIdGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = retVal && m_qmlCommandsFilePtr->flush();
	m_qmlCommandsFilePtr->close();

	return retVal;
}


bool CQmlCodeCommandIdGeneratorComp::ProcessQmlFile()
{
	if (!m_qmlCommandsFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlCommandsFilePtr->fileName(), m_qmlCommandsFilePtr->errorString()));

		return false;
	}

	// -----fill QML commands file
	QTextStream qmlFileStream(m_qmlCommandsFilePtr.GetPtr());

	// Begin QtObject
	qmlFileStream << QStringLiteral("pragma Singleton");
	FeedStream(qmlFileStream, 2, false);
	qmlFileStream << QStringLiteral("import QtQuick 2.0");
	FeedStream(qmlFileStream, 2, false);

	qmlFileStream << QStringLiteral("QtObject {");

	const SdlRequestList requestList = m_sdlRequestListCompPtr->GetRequests();
	for (const CSdlRequest& request: requestList){
		FeedStream(qmlFileStream, 1, false);
		FeedStreamHorizontally(qmlFileStream, 1);

		// use 's_' prefix to avoid ambiguity
		qmlFileStream << QStringLiteral("readonly property string s_");
		qmlFileStream << GetDecapitalizedValue(request.GetName());
		qmlFileStream << QStringLiteral(": '");
		qmlFileStream << request.GetName() << '\'';
	}
	FeedStream(qmlFileStream, 1, false);

	qmlFileStream << '}';
	FeedStream(qmlFileStream, 2);

	return true;
}


void CQmlCodeCommandIdGeneratorComp::AbortCurrentProcessing()
{
	m_qmlCommandsFilePtr->close();

	I_CRITICAL();

	m_qmlCommandsFilePtr->remove();
}


QString CQmlCodeCommandIdGeneratorComp::GetQmlCommandIdsFileName() const
{
	if (!m_argumentParserCompPtr.IsValid()){
		I_CRITICAL();

		return QString();
	}

	return GetCapitalizedValue(m_argumentParserCompPtr->GetNamespace()) + *m_qmlCommandIdsFileSuffixAttrPtr;
}


QString CQmlCodeCommandIdGeneratorComp::GetQmlCommandIdsFilePath() const
{
	if (!m_argumentParserCompPtr.IsValid()){
		I_CRITICAL();

		return QString();
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());

	return outputDirectoryPath + '/' + GetQmlCommandIdsFileName() + QStringLiteral(".qml");
}


} // namespace imtsdl

