// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesign/CDesignTokenProcessorsControllerComp.h>
#include <QtCore/QLockFile>

namespace imtdesign
{

void CDesignTokenProcessorsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QString beginInfoString =  "DesignToken started";
	if(m_argumentParserCompPtr.IsValid()){

		const QString designTokenFilePath = m_argumentParserCompPtr->GetDesignTokenFilePath();
		if(!designTokenFilePath.isEmpty()){
			beginInfoString += "\n\t| Theme file: " + designTokenFilePath;
		}

		const QString imagesInputDirectoryPath = m_argumentParserCompPtr->GetImagesInputDirectoryPath();
		if(!imagesInputDirectoryPath.isEmpty()){
			beginInfoString += "\n\t| Images path: " + imagesInputDirectoryPath;
		}

		const QString outputDirectoryPath = m_argumentParserCompPtr->GetOutputDirectoryPath();
		if(!outputDirectoryPath.isEmpty()){
			beginInfoString += "\n\t| Output path: " + outputDirectoryPath;
		}

		const QString sheetsInputDirectoryPath = m_argumentParserCompPtr->GetStyleSheetsInputDirectoryPath();
		if(!sheetsInputDirectoryPath.isEmpty()){
			beginInfoString += "\n\t| Styles path: " + sheetsInputDirectoryPath;
		}

		const QString projectName = m_argumentParserCompPtr->GetProjectName();
		if(!projectName.isEmpty()){
			beginInfoString += "\n\t| Project name: " + projectName;

		}

		if(m_argumentParserCompPtr->IsErrorsIgnoreRequested()){
			SendWarningMessage(0, "Errors will be ignored!");
			beginInfoString += "\n\t| Errors Ignored";
		}
		if(m_argumentParserCompPtr->IsSplitModeRequired()){
			beginInfoString += "\n\t| Split Mode";
		}
		if(m_argumentParserCompPtr->IsCopyDesignTokenFileRequired()){
			beginInfoString += "\n\t| Copy Design Token File";
		}
		if(m_argumentParserCompPtr->IsWebGenerateResourceRequired()){
			beginInfoString += "\n\t| Web Generate";
		}
		if(m_argumentParserCompPtr->IsCommonGenerateResourceRequired()){
			beginInfoString += "\n\t| Common res Generate";
		}
		if(m_argumentParserCompPtr->IsQmlGenerateRequired()){
			beginInfoString += "\n\t| Qml Generate";
		}
	}
	SendInfoMessage(0, ::qPrintable(beginInfoString));

	QString lockFileName = m_argumentParserCompPtr->GetOutputDirectoryPath() + "/DesignToken.lock";
	QLockFile lockFile(lockFileName);
	if(!lockFile.lock()){
		SendWarningMessage(0, QString("Unable to lock '%1' ").arg(lockFileName));
	}

	for (int i = 0; i < m_designTokenProcessors.GetCount(); ++i){
		if(m_designTokenProcessors[i]->Exec() != 0){
			if(!m_argumentParserCompPtr->IsErrorsIgnoreRequested()){
				lockFile.unlock();
				::exit(-1);
			}
		}
	}
	lockFile.unlock();
	QCoreApplication::exit(0);
}



} // namespace imtdesign


