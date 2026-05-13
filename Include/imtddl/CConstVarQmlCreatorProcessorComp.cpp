// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtddl/CConstVarQmlCreatorProcessorComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

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
			const QString& /*moduleName*/,
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

	if (rootObject.contains("key")){
		textStream << "readonly property string s_Key: \"" + rootObject.value("key").toString() + "\"\n";
	}

	if (rootObject.value("type") == "object"){
		for (const QString& key: propertyKeys){
			QJsonObject property = properties.value(key).toObject();
			QString normalKey = key;
			if (normalKey.size() > 0){
				normalKey[0] = normalKey[0].toUpper();
			}

			if (property.value("type") == "int"){
				textStream << "readonly property int i_" + normalKey + ": " + QString::number(property.value("value").toInt());
				textStream << "\n";
			}
			else{
				textStream << "readonly property string s_" + normalKey + ": \"" + property.value("value").toString() + "\"";
				textStream << "\n";
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
			textStream << "readonly property string s_" + key + ": \"" + value + "\"";
			textStream << "\n";
		}
	}

	textStream << "}" << "\n\n";

	return true;
}


} // namespace imtddl


