// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtddl/CDdlCodeCreatorProcessorsControllerComp.h>


// Qt includes
#include <QtCore/QLockFile>

// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>


namespace imtddl
{

void CDdlCodeCreatorProcessorsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	iprm::CParamsSet ddlParams;
	iprm::CTextParam templateDirPathParam;
	iprm::CTextParam cppDirPathParam;
	iprm::CTextParam qmlDirPathParam;
	iprm::CTextParam moduleCppNameParam;
	iprm::CTextParam moduleQmlNameParam;

	QString beginInfoString = QStringLiteral("DDL code creator started");
	if(m_argumentParserCompPtr.IsValid()){
		const QString templateDirPath = m_argumentParserCompPtr->GetTemplateDirPath();
		if(!templateDirPath.isEmpty()){
			beginInfoString += QStringLiteral("\n\t| Template directory: ") + templateDirPath;
			templateDirPathParam.SetText(templateDirPath);
			ddlParams.SetEditableParameter(QByteArrayLiteral("TemplateDirPath"), &templateDirPathParam);
		}

		const QString cppDirPath = m_argumentParserCompPtr->GetCppDirPath();
		if(!cppDirPath.isEmpty()){
			beginInfoString += QStringLiteral("\n\t| Output cpp directory: ") + cppDirPath;
			cppDirPathParam.SetText(cppDirPath);
			ddlParams.SetEditableParameter(QByteArrayLiteral("CppDirPath"), &cppDirPathParam);
		}

		const QString qmlDirPath = m_argumentParserCompPtr->GetQmlDirPath();
		if(!qmlDirPath.isEmpty()){
			beginInfoString += QStringLiteral("\n\t| Output qml directory: ") + qmlDirPath;
			qmlDirPathParam.SetText(qmlDirPath);
			ddlParams.SetEditableParameter(QByteArrayLiteral("QmlDirPath"), &qmlDirPathParam);
		}

		const QString moduleCppName = m_argumentParserCompPtr->GetModuleCppName();
		if(!moduleCppName.isEmpty()){
			beginInfoString += QStringLiteral("\n\t| Module cpp name: ") + moduleCppName;
			moduleCppNameParam.SetText(moduleCppName);
			ddlParams.SetEditableParameter(QByteArrayLiteral("ModuleCppName"), &moduleCppNameParam);
		}

		const QString moduleQmlName = m_argumentParserCompPtr->GetModuleQmlName();
		if(!moduleQmlName.isEmpty()){
			beginInfoString += QStringLiteral("\n\t| Module qml name: ") + moduleQmlName;
			moduleQmlNameParam.SetText(moduleQmlName);
			ddlParams.SetEditableParameter(QByteArrayLiteral("ModuleQmlName"), &moduleQmlNameParam);
		}
	}

	SendInfoMessage(0, beginInfoString);
	for (int i = 0; i < m_ddlProcessors.GetCount(); ++i){
		if(m_ddlProcessors[i]->DoProcessing(&ddlParams, nullptr, nullptr, nullptr, nullptr) != iproc::IProcessor::TS_OK){
			::exit(-1);
		}
	}

	QCoreApplication::exit(0);
}


} // namespace imtddl

