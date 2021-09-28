#include <imtstyle/CDesignTokenProcessorsControllerComp.h>

namespace imtstyle
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

	qCritical() << ::qPrintable(beginInfoString);

	for (int i = 0; i < m_designTokenProcessors.GetCount(); ++i){
		if(m_designTokenProcessors[i]->Exec() != 0){
			::exit(-1);
		}
	}
	QCoreApplication::exit(0);
}



} // namespace imtstyle


