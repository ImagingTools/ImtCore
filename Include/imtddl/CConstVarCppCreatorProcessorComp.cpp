#include <imtddl/CConstVarCppCreatorProcessorComp.h>


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
	cppStream << "#include \"Globals.h\"" << Qt::endl << Qt::endl;
	cppStream << "namespace " + moduleName << Qt::endl;
	cppStream << "{" << Qt::endl << Qt::endl << Qt::endl;

	return true;
}


bool CConstVarCppCreatorProcessorComp::CloseFiles(const iprm::IParamsSet* paramsPtr)
{
	QString moduleName = GetModuleName(paramsPtr);
	QTextStream cppStream(&m_outputFile);
	cppStream << Qt::endl << Qt::endl;
	cppStream << "} // namespace " + moduleName << Qt::endl << Qt::endl;

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
			const iprm::IParamsSet* paramsPtr)
{

	QJsonObject rootObject = templateDocument.object();

	QString name = rootObject.value("name").toString();
	QJsonObject properties = rootObject.value("properties").toObject();
	QStringList propertyKeys = properties.keys();

	QTextStream stream(&m_outputFile);

	stream << Qt::endl;

	for (QString key: propertyKeys){
		QJsonObject property = properties.value(key).toObject();
		if (property.value("type") == "int"){
			stream << "const QString " << name << "::s_" << key << " = " << QString::number(property.value("value").toInt()) << ";" << Qt::endl;
		}
		else{
			stream << "const QString " << name << "::s_" << key << " = \"" << property.value("value").toString() << "\";" << Qt::endl;
		}
	}

	stream << Qt::endl;

	return true;
}


} // namespace imtddl


