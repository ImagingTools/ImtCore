#include <imtdesign/CDesignTokenProcessorsControllerComp.h>
#include <QtCore/QLockFile>

namespace imtdesign
{

void CDesignTokenProcessorsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QString beginInfoString =  "DesignToken started";
	if(m_argumentParserAttrPtr.IsValid()){

		const QString designTokenFilePath = m_argumentParserAttrPtr->GetDesignTokenFilePath();
		if(!designTokenFilePath.isEmpty()){
			beginInfoString += "\n\t| Theme file: " + designTokenFilePath;
		}

		const QString imagesInputDirectoryPath = m_argumentParserAttrPtr->GetImagesInputDirectoryPath();
		if(!imagesInputDirectoryPath.isEmpty()){
			beginInfoString += "\n\t| Images path: " + imagesInputDirectoryPath;
		}

		const QString outputDirectoryPath = m_argumentParserAttrPtr->GetOutputDirectoryPath();
		if(!outputDirectoryPath.isEmpty()){
			beginInfoString += "\n\t| Output path: " + outputDirectoryPath;
		}

		const QString sheetsInputDirectoryPath = m_argumentParserAttrPtr->GetStyleSheetsInputDirectoryPath();
		if(!sheetsInputDirectoryPath.isEmpty()){
			beginInfoString += "\n\t| Styles path: " + sheetsInputDirectoryPath;
		}

		const QString projectName = m_argumentParserAttrPtr->GetProjectName();
		if(!projectName.isEmpty()){
			beginInfoString += "\n\t| Project name: " + projectName;

		}

		if(m_argumentParserAttrPtr->IsErrorsIgnoreRequested()){
			SendWarningMessage(0, "Errors will be ignored!");
			beginInfoString += "\n\t| Errors Ignored";
		}
		if(m_argumentParserAttrPtr->IsSplitModeRequired()){
			beginInfoString += "\n\t| Split Mode";
		}
		if(m_argumentParserAttrPtr->IsCopyDesignTokenFileRequired()){
			beginInfoString += "\n\t| Copy Design Token File";
		}
		if(m_argumentParserAttrPtr->IsWebGenerateResourceRequired()){
			beginInfoString += "\n\t| Web Generate";
		}
		if(m_argumentParserAttrPtr->IsQmlGenerateRequired()){
			beginInfoString += "\n\t| Qml Generate";
		}
	}
	SendInfoMessage(0, ::qPrintable(beginInfoString));

	QString lockFileName = m_argumentParserAttrPtr->GetOutputDirectoryPath() + "/DesignToken.lock";
	QLockFile lockFile(lockFileName);
	if(!lockFile.lock()){
		SendWarningMessage(0, QString("Unable to lock '%1' ").arg(lockFileName));
	}

	for (int i = 0; i < m_designTokenProcessors.GetCount(); ++i){
		if(m_designTokenProcessors[i]->Exec() != 0){
			if(!m_argumentParserAttrPtr->IsErrorsIgnoreRequested()){
				lockFile.unlock();
				::exit(-1);
			}
		}
	}
	lockFile.unlock();
	QCoreApplication::exit(0);
}



} // namespace imtdesign


