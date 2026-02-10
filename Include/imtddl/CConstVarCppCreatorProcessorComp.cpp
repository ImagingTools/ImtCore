// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtddl/CConstVarCppCreatorProcessorComp.h>


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
bool CConstVarCppCreatorProcessorComp::OpenFiles(const iprm::IParamsSet* paramsPtr)
{
	QString moduleName = GetModuleName(paramsPtr);
	QString filePath = GetCppDirPath(paramsPtr);
	if (filePath.isEmpty()){
		return false;
	}
	filePath += + "/Globals.cpp";

	if (!OpenFile(filePath, m_outputFile, QFile::ReadWrite)){
		return false;
	}
	m_outputFile.resize(0);
	QTextStream cppStream(&m_outputFile);
	cppStream << "#include \"Globals.h\"" << "\n\n";
	cppStream << "namespace " + moduleName << "\n";
	cppStream << "{" << "\n\n\n";

	return true;
}


bool CConstVarCppCreatorProcessorComp::CloseFiles(const iprm::IParamsSet* paramsPtr)
{
	QString moduleName = GetModuleName(paramsPtr);
	QTextStream cppStream(&m_outputFile);
	cppStream << "\n\n";
	cppStream << "} // namespace " + moduleName << "\n\n";

	return true;
}


QString CConstVarCppCreatorProcessorComp::GetModuleName(const iprm::IParamsSet* paramsPtr)
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



bool CConstVarCppCreatorProcessorComp::CreateBody(
			const QString& /*moduleName*/,
			const QJsonDocument& templateDocument,
			const iprm::IParamsSet* /*paramsPtr*/)
{

	QJsonObject rootObject = templateDocument.object();

	QString name = rootObject.value("name").toString();

	QTextStream stream(&m_outputFile);

	stream << "\n";

	if (rootObject.contains("key")){
		stream << "const QByteArray " << name << "::s_Key = \"" << rootObject.value("key").toString() << "\";" << "\n";
	}

	if (rootObject.value("type") == "object"){
		QJsonObject properties = rootObject.value("properties").toObject();
		QStringList propertyKeys = properties.keys();

		for (QString& key: propertyKeys){
			if (key.isEmpty()){
				Q_ASSERT(0);

				break;
			}
			QJsonObject property = properties.value(key).toObject();
			key[0] = key[0].toUpper();
			if (property.value("type") == "int"){
				stream << "const QByteArray " << name << "::s_" << key << " = " << QString::number(property.value("value").toInt()) << ";" << "\n";
			}
			else{
				stream << "const QByteArray " << name << "::s_" << key << " = \"" << property.value("value").toString() << "\";" << "\n";
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
			QString value = key;
			value[0] = value[0].toLower();
			stream << "const QByteArray " << name << "::s_" << key << " = \"" << value << "\";" << "\n";
		}
	}

	stream << "\n";

	return true;
}


} // namespace imtddl


