// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtddl/CConstVarQmldirCreatorProcessorComp.h>

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
bool CConstVarQmldirCreatorProcessorComp::OpenFiles(const iprm::IParamsSet* paramsPtr)
{
	QString filePath = GetQmlDirPath(paramsPtr);
	if (filePath.isEmpty()){
		return false;
	}

	filePath += "/qmldir";
	if (OpenFile(filePath, m_outputFile, QFile::ReadWrite)){
		m_outputFile.resize(0);
		QTextStream textStream(&m_outputFile);
		QString moduleName = GetModuleName(paramsPtr);
		if (m_outputFile.size() == 0){
			textStream << "module " << moduleName << "\n\n";
		}

		return true;
	}

	return false;
}


QString CConstVarQmldirCreatorProcessorComp::GetModuleName(const iprm::IParamsSet* paramsPtr)
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


bool CConstVarQmldirCreatorProcessorComp::CreateBody(const QString& /*moduleName*/, const QJsonDocument& templateDocument, const iprm::IParamsSet* paramsPtr)
{
	QJsonObject rootObject = templateDocument.object();

	QString name = rootObject.value("name").toString();

	QString filePath = GetQmlDirPath(paramsPtr) + "/qmldir";
	QFile outputFile;
	if (!OpenFile(filePath, outputFile, QFile::ReadWrite)){
		return false;
	}

	QTextStream textStream(&outputFile);

	bool isFind = false;
	textStream.device()->seek(0);
	while(!textStream.atEnd()){
		QString lineData = textStream.readLine();
		QString dataName = lineData.left(lineData.indexOf(" "));
		if (dataName == "singleton"){
			lineData = lineData.right(lineData.length() - lineData.indexOf(" ") - 1);
			dataName = lineData.left(lineData.indexOf(" "));
		}
		if (dataName == name){
			isFind = true;

			break;
		}
	}

	if (!isFind){
		textStream << "singleton " << name << " 1.0 " << name << ".qml" << "\n";
	}

	return true;
}


} // namespace imtddl


