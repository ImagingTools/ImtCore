#include <imtddl/CConstVarHCreatorProcessorComp.h>


// STL includes
#include <iostream>

// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QJsonObject>


namespace imtddl
{


// reimplemented (CConstVarCreatorProcessorCompBase)
bool CConstVarHCreatorProcessorComp::OpenFiles(const iprm::IParamsSet* paramsPtr)
{
	QString moduleName = GetModuleName(paramsPtr);
	QString filePath = GetCppDirPath(paramsPtr);
	if (filePath.isEmpty()){
		return false;
	}
	filePath += "/Globals.h";

	if (!OpenFile(filePath, m_outputFile, QFile::ReadWrite)){
		return false;
	}
	m_outputFile.resize(0);

	QTextStream hStream(&m_outputFile);
	hStream << "#pragma once" << "\n\n";
	hStream << "// Qt includes" << "\n";
	hStream << "#include <QtCore/QByteArray>" << "\n\n\n";
	hStream << "namespace " + moduleName << "\n";
	hStream << "{" << "\n\n\n";

	return true;
}


bool CConstVarHCreatorProcessorComp::CloseFiles(const iprm::IParamsSet* paramsPtr)
{
	QString moduleName = GetModuleName(paramsPtr);
	QTextStream hStream(&m_outputFile);
	hStream << "} // namespace " + moduleName << "\n\n\n";

	return true;
}


QString CConstVarHCreatorProcessorComp::GetModuleName(const iprm::IParamsSet* paramsPtr)
{
	if (paramsPtr == nullptr){
		return QString();
	}

	iprm::TParamsPtr<iprm::ITextParam> moduleNameParamPtr(paramsPtr, "ModuleCppName");

	if (!moduleNameParamPtr.IsValid()){
		SendErrorMessage(__LINE__, QString("The params module name is invalid"));

		return QString();
	}

	return moduleNameParamPtr->GetText();
}


bool CConstVarHCreatorProcessorComp::CreateBody(
			const QString moduleName,
			const QJsonDocument& templateDocument,
			const iprm::IParamsSet* /*paramsPtr*/)
{

	QJsonObject rootObject = templateDocument.object();

	QString name = rootObject.value("name").toString();
	QJsonObject properties = rootObject.value("properties").toObject();
	QStringList propertyKeys = properties.keys();

	QTextStream hStream(&m_outputFile);

	hStream << "struct " << name << "\n";
	hStream << "{" << "\n";

	for (QString key: propertyKeys){
		QJsonObject property = properties.value(key).toObject();
		if (property.value("type") == "int"){
			hStream << "static const int s_" << key << ";";
			hStream << "\n";
		}
		else{
			hStream << "static const QByteArray s_" << key << ";";
			hStream << "\n";
		}
	}

	hStream << "};" << "\n\n\n";

	return true;
}


} // namespace imtddl


