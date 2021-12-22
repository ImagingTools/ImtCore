#include <imtdesign/CDesignTokenProcessorsControllerComp.h>
#include <QtCore/QLockFile>

namespace imtdesign
{

void CDesignTokenProcessorsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QString beginInfoString =  "Creating style";
	if(m_argumentParserAttrPtr.IsValid()){
		beginInfoString += "\n\t| Theme file: " + m_argumentParserAttrPtr->GetDesignTokenFilePath();
		beginInfoString += "\n\t| Images path: " + m_argumentParserAttrPtr->GetImagesInputDirectoryPath();
		beginInfoString += "\n\t| Output path: " + m_argumentParserAttrPtr->GetOutputDirectoryPath();
		beginInfoString += "\n\t| Styles path: " + m_argumentParserAttrPtr->GetStyleSheetsInputDirectoryPath();
	}
	qInfo() << ::qPrintable(beginInfoString);

	QString lockFileName = m_argumentParserAttrPtr->GetOutputDirectoryPath() + "/DesignToken.lock";
	QLockFile lockFile(lockFileName);
	if(!lockFile.lock()){
		qWarning() << "Unable to lock " << lockFileName;
	}

	int procc = m_designTokenProcessors.GetCount();
	qDebug() << procc;

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


