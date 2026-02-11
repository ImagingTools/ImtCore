// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtddl/CConstVarQrcCreatorProcessorComp.h>

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

bool CConstVarQrcCreatorProcessorComp::OpenFiles(const iprm::IParamsSet* paramsPtr)
{
	QString filePath = GetQmlDirPath(paramsPtr);
	if (filePath.isEmpty()){
		return false;
	}

	QString moduleName = GetModuleName(paramsPtr);
	filePath += QString("/../%1.qrc").arg(moduleName);
	if (OpenFile(filePath, m_outputFile, QFile::ReadWrite)){
		m_outputFile.resize(0);
		QTextStream textStream(&m_outputFile);

		textStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";
		textStream << "<RCC>" << "\n";
		textStream << "	<qresource prefix=\"/qml\">" << "\n";
		textStream << QString("		<file alias=\"%1/qmldir\">%1/qmldir</file>").arg(moduleName) << "\n";


		return true;
	}

	return false;
}


bool CConstVarQrcCreatorProcessorComp::CloseFiles(const iprm::IParamsSet* /*paramsPtr*/)
{
	QTextStream textStream(&m_outputFile);
	textStream.device()->seek(m_outputFile.size());

	textStream << "	</qresource>" << "\n";
	textStream << "</RCC>" << "\n";

	m_outputFile.close();

	return true;
}



QString CConstVarQrcCreatorProcessorComp::GetModuleName(const iprm::IParamsSet* paramsPtr)
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


bool CConstVarQrcCreatorProcessorComp::CreateBody(const QString& moduleName, const QJsonDocument& templateDocument, const iprm::IParamsSet* paramsPtr)
{
	QJsonObject rootObject = templateDocument.object();

	QString name = rootObject.value("name").toString();

	QString newLineData = QString("		<file alias=\"%1/%2.qml\">%1/%2.qml</file>").arg(moduleName).arg(name);

	QString filePath = GetQmlDirPath(paramsPtr);
	if (filePath.isEmpty()){
		return false;
	}

	filePath += QString("/../%1.qrc").arg(moduleName);
	QFile outputFile;
	if (!OpenFile(filePath, outputFile, QFile::ReadWrite)){
		return false;
	}

	QTextStream textStream(&outputFile);

	bool isFind = false;
	textStream.device()->seek(0);
	while(!textStream.atEnd()){
		QString lineData = textStream.readLine();
		if (lineData.contains(newLineData)){
			isFind = true;

			break;
		}
	}

	if (!isFind){
		textStream << newLineData << "\n";
	}

	return true;
}


} // namespace imtddl


