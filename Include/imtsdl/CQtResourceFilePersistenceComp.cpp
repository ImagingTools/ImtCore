// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <QtCore/qdir.h>
#include <imtsdl/CQtResourceFilePersistenceComp.h>


// Qt includes
#include <QtCore/QXmlStreamWriter>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IParamsSet.h>
#include <iprm/INameParam.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


// public methods

// reimplemented (ifile::IFilePersistence)

bool CQtResourceFilePersistenceComp::IsOperationSupported(
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

	if (flags & QF_SAVE){
		const iprm::IParamsManager* paramsManager = dynamic_cast<const iprm::IParamsManager*>(dataObjectPtr);
		if (paramsManager == nullptr){
			return false;
		}
	}

	return true;
}


ifile::IFilePersistence::OperationState CQtResourceFilePersistenceComp::LoadFromFile(
	istd::IChangeable& data,
	const QString& filePath,
	ibase::IProgressManager* /*progressManagerPtr*/) const
{
	iprm::IParamsManager* prefixParamsPtr = nullptr;

	try {
		iprm::IParamsManager& paramsSetRef = dynamic_cast<iprm::IParamsManager&>(data);
		prefixParamsPtr = &paramsSetRef;
	}
	catch (std::bad_cast&){
		SendCriticalMessage(0, "Unexpected input data", __func__);
		I_CRITICAL();

		return OS_FAILED;
	}
	Q_ASSERT(prefixParamsPtr != nullptr);

	QFile loadFile(filePath);
	if (!loadFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(loadFile.fileName()), __func__);

		return OS_FAILED;
	}

	// setup QRC-DOM document
	QDomDocument qrcDocument;
	QString errorString;
	int errorLine = -1;
	int errorSymbol = -1;
	if (!qrcDocument.setContent(&loadFile, &errorString, &errorLine, &errorSymbol)){
		SendErrorMessage(0, QString("XML error on line %1, col %2: %3")
								.arg(errorLine).arg(errorSymbol).arg(errorString));

		return OS_FAILED;
	}

	QDomElement rootElement = qrcDocument.firstChildElement(QStringLiteral("RCC"));
	if (rootElement.isNull()){
		SendErrorMessage(0, "The <RCC> root element is missing.");

		return OS_FAILED;
	}

	// reset params
	prefixParamsPtr->ResetData();

	// read data
	QDomElement prefixElement = rootElement.firstChildElement(QStringLiteral("qresource"));
	for (; !prefixElement.isNull(); prefixElement = prefixElement.nextSiblingElement(QStringLiteral("qresource"))){

		const int prefixParamsSetIndex = prefixParamsPtr->InsertParamsSet();
		iprm::IParamsSet* prefixParamsSet = prefixParamsPtr->GetParamsSet(prefixParamsSetIndex);
		Q_ASSERT(prefixParamsSet != nullptr);

		QString prefixName = prefixElement.attribute(QStringLiteral("prefix"));
		if (!prefixName.startsWith('/')){
			prefixName.prepend('/');
		}

		iprm::INameParam* prefixNameParamPtr = dynamic_cast<iprm::INameParam*>(prefixParamsSet->GetEditableParameter(QtResourceModelParamIds::PrefixName));
		if (prefixNameParamPtr == nullptr){
			SendCriticalMessage(0, QString("Name param '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::PrefixName)));
			I_CRITICAL();

			return OS_FAILED;
		}
		prefixNameParamPtr->SetName(prefixName);

		iprm::IParamsManager* objectsManagerPtr = dynamic_cast<iprm::IParamsManager*>(prefixParamsSet->GetEditableParameter(QtResourceModelParamIds::QrcObjects));
		if (objectsManagerPtr == nullptr){
			SendCriticalMessage(0, QString("Params manager  '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcObjects)));
			I_CRITICAL();

			return OS_FAILED;
		}

		QDomElement fileElement = prefixElement.firstChildElement(QStringLiteral("file"));
		for (; !fileElement.isNull(); fileElement = fileElement.nextSiblingElement(QStringLiteral("file"))){
			const int fileParamsIndex = objectsManagerPtr->InsertParamsSet();
			iprm::IParamsSet* fileParamsSetPtr = objectsManagerPtr->GetParamsSet(fileParamsIndex);
			Q_ASSERT(fileParamsSetPtr != nullptr);

			const QString fileName = fileElement.text();
			iprm::INameParam* fileNameParamPtr = dynamic_cast<iprm::INameParam*>(fileParamsSetPtr->GetEditableParameter(QtResourceModelParamIds::QrcFileName));
			if (fileNameParamPtr == nullptr){
				SendCriticalMessage(0, QString("Name param '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcFileName)));
				I_CRITICAL();

				return OS_FAILED;
			}
			fileNameParamPtr->SetName(fileName);

			const QString alias = fileElement.attribute(QStringLiteral("alias"));
			iprm::INameParam* aliasNameParamPtr = dynamic_cast<iprm::INameParam*>(fileParamsSetPtr->GetEditableParameter(QtResourceModelParamIds::QrcAlias));
			if (aliasNameParamPtr == nullptr){
				SendCriticalMessage(0, QString("Name param '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcAlias)));
				I_CRITICAL();

				return OS_FAILED;
			}
			aliasNameParamPtr->SetName(alias);
		}
	}

	return OS_OK;
}


// reimplemented (ifile::IFileTypeInfo)

ifile::IFilePersistence::OperationState CQtResourceFilePersistenceComp::SaveToFile(
	const istd::IChangeable& data,
	const QString& filePath,
	ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const iprm::IParamsManager* prefixParamsPtr = nullptr;

	try {
		const iprm::IParamsManager& paramsSetRef = dynamic_cast<const iprm::IParamsManager&>(data);
		prefixParamsPtr = &paramsSetRef;
	}
	catch (std::bad_cast&){
		SendCriticalMessage(0, "Unexpected input data", __func__);
		I_CRITICAL();

		return OS_FAILED;
	}
	Q_ASSERT(prefixParamsPtr != nullptr);

	QFile qrcFile(filePath);
	if (!qrcFile.open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(qrcFile.fileName(), qrcFile.errorString()));
		I_CRITICAL();

		return OS_FAILED;
	}

	QXmlStreamWriter xmlWriter(&qrcFile);
	xmlWriter.setAutoFormatting(true);

	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement(QStringLiteral("RCC"));// start RCC

	// add prefixes
	const int prefixCount = prefixParamsPtr->GetParamsSetsCount();
	for (int prefixParamsSetIndex = 0; prefixParamsSetIndex < prefixCount; ++prefixParamsSetIndex){
		xmlWriter.writeStartElement(QStringLiteral("qresource"));

		const iprm::IParamsSet* prefixParamsSetPtr = prefixParamsPtr->GetParamsSet(prefixParamsSetIndex);
		Q_ASSERT(prefixParamsSetPtr != nullptr);

		iprm::TParamsPtr<iprm::INameParam> prefixNameParamPtr (prefixParamsSetPtr, QtResourceModelParamIds::PrefixName);
		if (!prefixNameParamPtr.IsValid()){
			SendCriticalMessage(0, QString("Name param '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::PrefixName)));
			I_CRITICAL();

			return OS_FAILED;
		}
		QString prefixName = prefixNameParamPtr->GetName();
		if (!prefixName.startsWith('/')){
			prefixName.prepend('/');
		}
		xmlWriter.writeAttribute(QStringLiteral("prefix"), prefixName);

		iprm::TParamsPtr<iprm::IParamsManager> objectsManagerPtr(prefixParamsSetPtr, QtResourceModelParamIds::QrcObjects);
		if (!objectsManagerPtr.IsValid()){
			SendCriticalMessage(0, QString("Params manager  '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcObjects)));
			I_CRITICAL();

			return OS_FAILED;
		}

		// add files
		const int fileParamsCount = objectsManagerPtr->GetParamsSetsCount();
		for (int fileParamsIndex = 0; fileParamsIndex < fileParamsCount; ++fileParamsIndex){
			xmlWriter.writeStartElement(QStringLiteral("file")); // start qresource

			const iprm::IParamsSet* fileParamsSetPtr = objectsManagerPtr->GetParamsSet(fileParamsIndex);
			Q_ASSERT(fileParamsSetPtr != nullptr);

			iprm::TParamsPtr<iprm::INameParam> aliasNameParamPtr(fileParamsSetPtr, QtResourceModelParamIds::QrcAlias);
			if (!aliasNameParamPtr.IsValid()){
				SendCriticalMessage(0, QString("Name param '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcAlias)));
				I_CRITICAL();

				return OS_FAILED;
			}
			const QString aliasName = aliasNameParamPtr->GetName();
			xmlWriter.writeAttribute(QStringLiteral("alias"), aliasName);

			iprm::TParamsPtr<iprm::INameParam> fileNameParamPtr(fileParamsSetPtr, QtResourceModelParamIds::QrcFileName);
			if (!fileNameParamPtr.IsValid()){
				SendCriticalMessage(0, QString("Name param '%1' is not valid!").arg(qPrintable(QtResourceModelParamIds::QrcFileName)));
				I_CRITICAL();

				return OS_FAILED;
			}
			const QString fileNameParam = fileNameParamPtr->GetName();
			xmlWriter.writeCharacters(fileNameParam);
			xmlWriter.writeEndElement();
		}

		xmlWriter.writeEndElement(); // end qresource
	}

	xmlWriter.writeEndElement(); // end RCC
	xmlWriter.writeEndDocument();

	qrcFile.flush();
	qrcFile.close();

	return OS_OK;
}


bool CQtResourceFilePersistenceComp::GetFileExtensions(
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


QString CQtResourceFilePersistenceComp::GetTypeDescription(const QString* extensionPtr) const
{
	if (extensionPtr != nullptr && !extensionPtr->isNull() && extensionPtr->isEmpty()){
		return QStringLiteral("Qt qmldir file");
	}

	return QString();
}


// reimplemented (ifile::IFilePersistenceInfo)

QString CQtResourceFilePersistenceComp::GetLastFilePath(OperationType operationType, PathType pathType) const
{
	switch (operationType){
	case OT_SAVE:
		return GetPathForType(m_lastSaveInfo, pathType);

	default:
		return GetPathForType(m_lastOpenInfo, pathType);
	}
}


// protected methods

QString CQtResourceFilePersistenceComp::GetPathForType(const QFileInfo& fileInfo, PathType pathType) const
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

