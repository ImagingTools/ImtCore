#include <imtddl/CConstVarQmlCreatorProcessorComp.h>


// STL includes
#include <iostream>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QJsonObject>

// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>


namespace imtddl
{


// reimplemented (CConstVarCreatorProcessorCompBase)

QString CConstVarQmlCreatorProcessorComp::GetModuleName(const iprm::IParamsSet* paramsPtr)
{
	if (paramsPtr == nullptr){
		return QString();
	}

	iprm::TParamsPtr<iprm::ITextParam> moduleNameParamPtr(paramsPtr, "ModuleQmlName");

	if (!moduleNameParamPtr.IsValid()){
		SendErrorMessage(__LINE__, QString("The params module name is invalid"));

		return QString();
	}

	return moduleNameParamPtr->GetText();
}


bool CConstVarQmlCreatorProcessorComp::CreateBody(
			const QString moduleName,
			const QJsonDocument& templateDocument,
			const iprm::IParamsSet* paramsPtr)
{
	QJsonObject rootObject = templateDocument.object();

	QString name = rootObject.value("name").toString();
	QJsonObject properties = rootObject.value("properties").toObject();
	QStringList propertyKeys = properties.keys();

	QString filePath = GetQmlDirPath(paramsPtr);
	if (filePath.isEmpty()){
		return false;
	}
	filePath += "/" + name + ".qml";

	QFile outputFile;
	if (!OpenFile(filePath, outputFile, QFile::ReadWrite)){
		return false;
	}
	outputFile.resize(0);
	QTextStream textStream(&outputFile);

	textStream << "pragma Singleton" << "\n";
	textStream << "import QtQuick 2.12" << "\n\n";
	textStream << "QtObject {" << "\n";

	for (QString key: propertyKeys){
		QJsonObject property = properties.value(key).toObject();
		QString normalKey = key;
		if (normalKey.size() > 0){
			normalKey[0] = normalKey[0].toUpper();
		}

		if (property.value("type") == "int"){
			textStream << "    readonly property int i_" + normalKey + ": " + QString::number(property.value("value").toInt());
			textStream << "\n";
		}
		else{
			textStream << "readonly property string s_" + normalKey + ": \"" + property.value("value").toString() + "\"";
			textStream << "\n";
		}
	}

	textStream << "}" << "\n\n";


	return true;
}


} // namespace imtddl


