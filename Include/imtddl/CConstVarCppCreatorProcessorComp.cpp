#include <imtddl/CConstVarCppCreatorProcessorComp.h>


// STL includes
#include <iostream>

// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>
#include <istd/CSystem.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QJsonObject>


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
			const QString moduleName,
			const QJsonDocument& templateDocument,
			const iprm::IParamsSet* /*paramsPtr*/)
{

	QJsonObject rootObject = templateDocument.object();

	QString name = rootObject.value("name").toString();
	QJsonObject properties = rootObject.value("properties").toObject();
	QStringList propertyKeys = properties.keys();

	QTextStream stream(&m_outputFile);

	stream << "\n";

	for (QString key: propertyKeys){
		QJsonObject property = properties.value(key).toObject();
		if (property.value("type") == "int"){
			stream << "const QByteArray " << name << "::s_" << key << " = " << QString::number(property.value("value").toInt()) << ";" << "\n";
		}
		else{
			stream << "const QByteArray " << name << "::s_" << key << " = \"" << property.value("value").toString() << "\";" << "\n";
		}
	}

	stream << "\n";

	return true;
}


} // namespace imtddl


