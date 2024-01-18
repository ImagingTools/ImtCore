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
//	iprm::CTextParam templateFilePathParam;
	iprm::CTextParam templateDirPathParam;
	iprm::CTextParam cppDirPathParam;
	iprm::CTextParam qmlDirPathParam;
//	iprm::CTextParam outputCppFilePathParam;
//	iprm::CTextParam outputHFilePathParam;
//	iprm::CTextParam outputQmlFilePathParam;
//	iprm::CTextParam outputQmldirFilePathParam;
	iprm::CTextParam moduleCppNameParam;
	iprm::CTextParam moduleQmlNameParam;

	QString beginInfoString =  "DDL code creator started";
	if(m_argumentParserAttrPtr.IsValid()){

//		const QString templateFilePath = m_argumentParserAttrPtr->GetTemplateFilePath();
//		if(!templateFilePath.isEmpty()){
//			beginInfoString += "\n\t| Template file: " + templateFilePath;
//			templateFilePathParam.SetText(templateFilePath);
//			ddlParams.SetEditableParameter("TemplateFilePath", &templateFilePathParam);
//		}

		const QString templateDirPath = m_argumentParserAttrPtr->GetTemplateDirPath();
		if(!templateDirPath.isEmpty()){
			beginInfoString += "\n\t| Template directory: " + templateDirPath;
			templateDirPathParam.SetText(templateDirPath);
			ddlParams.SetEditableParameter("TemplateDirPath", &templateDirPathParam);
		}

		const QString cppDirPath = m_argumentParserAttrPtr->GetCppDirPath();
		if(!cppDirPath.isEmpty()){
			beginInfoString += "\n\t| Output cpp directory: " + cppDirPath;
			cppDirPathParam.SetText(cppDirPath);
			ddlParams.SetEditableParameter("CppDirPath", &cppDirPathParam);
		}

		const QString qmlDirPath = m_argumentParserAttrPtr->GetQmlDirPath();
		if(!qmlDirPath.isEmpty()){
			beginInfoString += "\n\t| Output qml directory: " + qmlDirPath;
			qmlDirPathParam.SetText(qmlDirPath);
			ddlParams.SetEditableParameter("QmlDirPath", &qmlDirPathParam);
		}

		const QString moduleCppName = m_argumentParserAttrPtr->GetModuleCppName();
		if(!moduleCppName.isEmpty()){
			beginInfoString += "\n\t| Module cpp name: " + moduleCppName;
			moduleCppNameParam.SetText(moduleCppName);
			ddlParams.SetEditableParameter("ModuleCppName", &moduleCppNameParam);
		}

		const QString moduleQmlName = m_argumentParserAttrPtr->GetModuleQmlName();
		if(!moduleQmlName.isEmpty()){
			beginInfoString += "\n\t| Module qml name: " + moduleQmlName;
			moduleQmlNameParam.SetText(moduleQmlName);
			ddlParams.SetEditableParameter("ModuleQmlName", &moduleQmlNameParam);
		}

	}
	SendInfoMessage(0, ::qPrintable(beginInfoString));

	// QString lockFileName = m_argumentParserAttrPtr->GetOutputDirectoryPath() + "/DesignToken.lock";
	// QLockFile lockFile(lockFileName);
	// if(!lockFile.lock()){
	// 	SendWarningMessage(0, QString("Unable to lock '%1' ").arg(lockFileName));
	// }

	for (int i = 0; i < m_ddlProcessors.GetCount(); ++i){
		if(m_ddlProcessors[i]->DoProcessing(&ddlParams, nullptr, nullptr) != iproc::IProcessor::TS_OK){
			// if(!m_argumentParserAttrPtr->IsErrorsIgnoreRequested()){
			// 	lockFile.unlock();
				::exit(-1);
			// }
		}
	}
	// lockFile.unlock();

	QCoreApplication::exit(0);
}



} // namespace imtddl


