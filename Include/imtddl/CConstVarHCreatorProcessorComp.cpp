// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtddl/CConstVarHCreatorProcessorComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>


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
			const QString& /*moduleName*/,
			const QJsonDocument& templateDocument,
			const iprm::IParamsSet* /*paramsPtr*/)
{

	QJsonObject rootObject = templateDocument.object();

	QString name = rootObject.value("name").toString();

	QTextStream hStream(&m_outputFile);

	hStream << "struct " << name << "\n";
	hStream << "{" << "\n";

	if (rootObject.contains("key")){
		hStream << "static const QByteArray s_Key;\n";
	}

	if (rootObject.value("type") == "object"){
		QJsonObject properties = rootObject.value("properties").toObject();
		QStringList propertyKeys = properties.keys();

		for (QString key: propertyKeys){
			if (key.isEmpty()){
				Q_ASSERT(0);

				break;
			}
			QJsonObject property = properties.value(key).toObject();
			key[0] = key[0].toUpper();
			if (property.value("type") == "int"){
				hStream << "static const int s_" << key << ";";
				hStream << "\n";
			}
			else{
				hStream << "static const QByteArray s_" << key << ";";
				hStream << "\n";
			}
		}
	}
	else if (rootObject.value("type") == "enum"){
		QJsonArray enumArray = rootObject.value("enum").toArray();
		for (int index = 0; index < enumArray.count(); index++){
			QString key = enumArray[index].toString();
			if (key.isEmpty()){
				Q_ASSERT(0);

				break;
			}
			key[0] = key[0].toUpper();
			hStream << "static const QByteArray s_" << key << ";";
			hStream << "\n";
		}
	}

	hStream << "};" << "\n\n\n";

	return true;
}


} // namespace imtddl


